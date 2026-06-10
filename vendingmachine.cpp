#include "vendingmachine.h"

#include <algorithm>

VendingMachine::VendingMachine(QObject *parent)
    : QObject{parent},
    vmMoneyList{Money::makeMoneyVector()}
{
    filePaths = FileUtil::copyQrcFile("json", ":/json", "json");

    logCsv.insert("log", FileUtil::Csv("/log.csv"));
    logCsv.insert("collection_log", FileUtil::Csv("/collection_log.csv"));

    if(logCsv["log"].isFileEmpty()){
        logCsv["log"].setCsvHeader({"날짜", "음료명", "음료가격", "남은개수"});
    }

    if(logCsv["collection_log"].isFileEmpty()){
        logCsv["collection_log"].setCsvHeader({"날짜", "총 벌어들인 금액", "수금 후 남은 금액"});
    }

    beverageList = readVMBeverageDataFromJson(filePaths["vm_beverage_data.json"]);
    vmMoneyList = readVMMoneyDataFromJson(filePaths["vm_money_data.json"]);
}

Beverage* VendingMachine::getBeverage(int index){
    int i = beverageList.search([&](QSharedPointer<Beverage> a){
        return index == a->index();
    });

    if(i == beverageList.getSize()){
        return nullptr;
    }

    auto ptr = beverageList.get(i);
    return ptr.data();
}

Money* VendingMachine::getMoney(int index){
    if(vmMoneyList.size() <= index || index < 0){
        WARNING("잘못된 index (%1) 로의 접근입니다.",index);
        return new Money();
    }
    return vmMoneyList[index].data();
}

VendingMachine::SuccessType VendingMachine::addConsumerMoney(int cost){
    if(consumerMoneyList.empty()) consumerMoneyList = Money::makeMoneyVector();
    if(m_nowInputMoney >= 7000) {
        if(returnMoneyList.empty()) returnMoneyList = Money::makeMoneyVector();
        for(auto& iter : returnMoneyList){
            if(iter->cost() == cost) iter->setCount(iter->count() + 1);
        }
        emit isReturnMoneyChanged();
        return SuccessType::FAIL_7000;
    }
    if(cost == 1000 && consumerMoneyList[0]->cost() == cost &&
        consumerMoneyList[0]->count() >= 5) {
        if(returnMoneyList.empty()) returnMoneyList = Money::makeMoneyVector();
        for(auto& iter : returnMoneyList){
            if(iter->cost() == cost) iter->setCount(iter->count() + 1);
        }
        emit isReturnMoneyChanged();
        return SuccessType::FAIL_5000;
    }

    for(const auto& iter : consumerMoneyList){
        if(iter->cost() == cost){
            iter->setCount(iter->count() + 1);
            setNowInputMoney(m_nowInputMoney + cost);
            return SuccessType::SUCCESS;
        }
    }

    return SuccessType::FAIL;
}


// 음료 구매 함수
// 음료 구매를 성공했는지 실패했는지에 따라
// SuccessType을 반환함.
// 성공하면 SUCCESS
// 실패하면 FAIL_SOLD_OUT, FAIL_EMPTY, FAIL_INCORRECT_COST 반환
// 각각, 메뉴 소진, 투입된 돈이 없음, 잘못된 금액 투입
VendingMachine::SuccessType VendingMachine::buyBeverage(Beverage* beverage){
    if(beverage->count() <= 0) return SuccessType::FAIL_SOLD_OUT;
    if(consumerMoneyList.empty()) return SuccessType::FAIL_EMPTY;
    if(m_nowInputMoney == 0) return SuccessType::FAIL_EMPTY;
    if(beverage->cost() == m_nowInputMoney){
        consumerMoneyList.clear();
        addOutputBeverage(beverage);
        setNowInputMoney(0);
        jsonUpdate(beverage);
        return SuccessType::SUCCESS;
    }

    if(beverage->cost() < m_nowInputMoney){
        if(calcuateReturnMoney(m_nowInputMoney - beverage->cost())) {
            addOutputBeverage(beverage);
            setNowInputMoney(0);
            jsonUpdate(beverage);
            return SuccessType::SUCCESS;
        }else{
            returnAsMoney();
            return SuccessType::FAIL_NOT_HAVE_MONEY;
        }
    }

    returnAsMoney();
    return SuccessType::FAIL_INCORRECT_COST;
}

// 들어온 돈을 그대로 반환하는 함수
void VendingMachine::returnAsMoney(){
    if(returnMoneyList.empty()) returnMoneyList = Money::makeMoneyVector();
    copyToMoneyList(consumerMoneyList, returnMoneyList);
    consumerMoneyList.clear();
    setNowInputMoney(0);
    emit isReturnMoneyChanged();
}

bool VendingMachine::calcuateReturnMoney(int remainedCost){
    auto temp = Money::makeMoneyVector();
    for(int i=0;i<vmMoneyList.size();i++){
        if(vmMoneyList[i]->cost() != consumerMoneyList[i]->cost()){
            WARNING("두 MoneyList의 Money 순서가 다릅니다.");
            return false;
        }
        if(vmMoneyList[i]->cost() > remainedCost) continue;
        int nowVMMoneyCount = vmMoneyList[i]->count() + consumerMoneyList[i]->count();
        int count = (remainedCost / vmMoneyList[i]->cost() > nowVMMoneyCount
                         ? nowVMMoneyCount : remainedCost / vmMoneyList[i]->cost());

        remainedCost -= vmMoneyList[i]->cost() * count;
        temp[i]->setCount(count);
    }

    if(remainedCost == 0){
        if(returnMoneyList.empty()) returnMoneyList = Money::makeMoneyVector();
        for(int i=0;i<temp.size();i++){
            if(vmMoneyList[i]->count() < temp[i]->count()){
                CRITICAL("자판기 계산 결과 음수가 발생했습니다. 코드 수정하세요.");
                return false;
            }
            returnMoneyList[i]->setCount(returnMoneyList[i]->count() + temp[i]->count());
            vmMoneyList[i]->setCount(vmMoneyList[i]->count() - temp[i]->count());
        }
        addVMMoney();
        consumerMoneyList.clear();
        emit isReturnMoneyChanged();
        return true;
    }else{
        return false;
    }
}

void VendingMachine::addVMMoney(){
    for(int i=0;i<vmMoneyList.size();i++){
        if(vmMoneyList[i]->cost() != consumerMoneyList[i]->cost()){
            WARNING("두 MoneyList의 Money 순서가 다릅니다.");
            return;
        }
        vmMoneyList[i]->setCount(vmMoneyList[i]->count()
                                 + consumerMoneyList[i]->count());
    }
}

void VendingMachine::returnMoney(Consumer* consumer){
    for(int i=0;i<returnMoneyList.size();i++){
        Money* m = consumer->get(i);
        if(m->cost() != returnMoneyList[i]->cost()){
            WARNING("두 MoneyList의 Money 순서가 다릅니다.");
            return;
        }

        m->setCount(m->count() + returnMoneyList[i]->count());
    }

    returnMoneyList.clear();
    emit isReturnMoneyChanged();
}

void VendingMachine::copyToMoneyList(const QList<QSharedPointer<Money>>& to, QList<QSharedPointer<Money>>& from){
    for(int i=0;i<to.size();i++){
        if(to[i]->cost() != from[i]->cost()){
            WARNING("두 MoneyList의 Money 순서가 다릅니다.");
            return;
        }        
        from[i]->setCount(to[i]->count());
    }
}

void VendingMachine::addOutputBeverage(Beverage* beverage){
    beverage->setCount(beverage->count() - 1);

    outputBeverageStack.push(QSharedPointer<Beverage>(new Beverage(0, beverage->name(), 0, 0, beverage->imagePath())));

    emit stackTopChanged();
}

void VendingMachine::removeOutputBeverage(){
    outputBeverageStack.pop();
    emit stackTopChanged();
}

QVariantList VendingMachine::getLogList(QString name, QString headerName, VendingMachine::SortType sortType){
    if(!logCsv.contains(name)){
        WARNING("존재하지 않는 로그 이름입니다: %1", name);
        return QVariantList();
    }

    QVariantList list = logCsv[name].read();

    if(list.empty() || headerName.isEmpty()){
        return list;
    }

    QVariantMap firstRow = list.first().toMap();
    if(!firstRow.contains(headerName)){
        WARNING("존재하지 않는 헤더 이름입니다: %1", headerName);
        return list;
    }

    std::sort(list.begin(), list.end(), [headerName, sortType](const QVariant& left, const QVariant& right){
        QVariantMap leftRow = left.toMap();
        QVariantMap rightRow = right.toMap();

        QString leftText = leftRow.value(headerName).toString();
        QString rightText = rightRow.value(headerName).toString();

        bool leftIsNumber = false;
        bool rightIsNumber = false;
        double leftNumber = leftText.toDouble(&leftIsNumber);
        double rightNumber = rightText.toDouble(&rightIsNumber);

        int compareResult = 0;

        if(leftIsNumber && rightIsNumber){
            if(leftNumber < rightNumber){
                compareResult = -1;
            }else if(leftNumber > rightNumber){
                compareResult = 1;
            }
        }else{
            compareResult = QString::localeAwareCompare(leftText, rightText);
        }

        if(sortType == VendingMachine::ASC){
            return compareResult < 0;
        }

        return compareResult > 0;
    });

    return list;
}

QVariantList VendingMachine::getCsvHeader(QString name){
    if(!logCsv.contains(name)){
        WARNING("존재하지 않는 로그 이름입니다: %1", name);
        return QVariantList();
    }

    QVariantList list = logCsv[name].getHeaderRead();

    if(list.empty()){
        WARNING("현재 로그의 헤더가 비어있습니다. 확인하세요.");
    }

    return list;
}

QVariantList VendingMachine::getLogFileList(){
    QVariantList ret;
    for(const auto& iter : logCsv.keys()){
        ret.append(iter);
    }
    return ret;
}

void VendingMachine::jsonUpdate(Beverage* beverage){
    //로그 파일에 저장
    logCsv["log"].write({QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"),
                  beverage->name(),
                  QString::number(beverage->cost()),
                  QString::number(beverage->count())});

    jsonUpdateVMBeverage();
    jsonUpdateVMMoney();
}

void VendingMachine::jsonUpdateVMBeverage(){
    auto doc = FileUtil::openJson(filePaths["vm_beverage_data.json"]);
    if(doc.isEmpty() || doc.array().empty()){
        CRITICAL("Json 파일을 열 수 없거나 Json 규격이 망가졌습니다.");
        return;
    }

    QJsonArray arr;

    for(int i = 0;i< beverageList.getSize();i++){
        auto b = beverageList.get(i);

        QJsonObject obj;
        obj["index"] = b->index();
        obj["name"] = b->name();
        obj["cost"] = b->cost();
        obj["count"] = b->count();
        obj["imagePath"] = b->imagePath();
        arr.append(obj);
    }

    doc.setArray(arr);
    if(!FileUtil::saveJson(filePaths["vm_beverage_data.json"], doc)){
        CRITICAL("Json 갱신에 실패했습니다.");
        return;
    }
}

void VendingMachine::jsonUpdateVMMoney(){
    auto doc = FileUtil::openJson(filePaths["vm_money_data.json"]);
    if(doc.isEmpty()){
        CRITICAL("Json 파일을 열 수 없거나 Json 규격이 망가졌습니다.");
        return;
    }

    auto obj = doc.object();
    QJsonArray arr;

    for(int i=0;i<vmMoneyList.size();i++){
        arr.append(vmMoneyList[i]->count());
    }

    obj["money"] = arr;

    doc.setObject(obj);

    if(!FileUtil::saveJson(filePaths["vm_money_data.json"], doc)){
        CRITICAL("Json 갱신에 실패했습니다.");
        return;
    }
}

void VendingMachine::reload(){
    beverageList = readVMBeverageDataFromJson(filePaths["vm_beverage_data.json"]);
    vmMoneyList = readVMMoneyDataFromJson(filePaths["vm_money_data.json"]);
}

void VendingMachine::changeBeverageName(Beverage* beverage, QString newName){
    if(!Manager::getInstance().isLogined()){
        WARNING("로그인 하지 않은 사용자는 접근할 수 없는 기능입니다.");
        return;
    }

    for(int i = 0;i<beverageList.getSize();i++){
        auto b = beverageList.get(i);
        if(b.data() == beverage){
            beverage->setName(newName);
            break;
        }
    }
}

void VendingMachine::changeBeverageIndex(Beverage* beverage, int newIndex){
    if(!Manager::getInstance().isLogined()){
        WARNING("로그인 하지 않은 사용자는 접근할 수 없는 기능입니다.");
        return;
    }

    bool isSameIndex = false;
    for(int i=0;i<beverageList.getSize();i++){
        auto b = beverageList.get(i);
        if(b->index() == newIndex){
            isSameIndex = true;
            b->setIndex(beverage->index());
            beverage->setIndex(newIndex);
            break;
        }
    }

    if(!isSameIndex){
        for(int i = 0;i<beverageList.getSize();i++){
            auto b = beverageList.get(i);
            if(b.data() == beverage){
                beverage->setIndex(newIndex);
                break;
            }
        }
    }
}

void VendingMachine::changeBeverageCost(Beverage* beverage, int newCost){
    if(!Manager::getInstance().isLogined()){
        WARNING("로그인 하지 않은 사용자는 접근할 수 없는 기능입니다.");
        return;
    }

    for(int i=0;i<beverageList.getSize();i++){
        auto b = beverageList.get(i);

        if(b.data() == beverage){
            beverage->setCost(newCost);
            break;
        }
    }
}

void VendingMachine::changeBeverageCount(Beverage* beverage, int newCount){
    if(!Manager::getInstance().isLogined()){
        WARNING("로그인 하지 않은 사용자는 접근할 수 없는 기능입니다.");
        return;
    }

    for(int i = 0;i<beverageList.getSize();i++){
        auto b = beverageList.get(i);
        if(b.data() == beverage){
            beverage->setCount(newCount);
            break;
        }
    }
}

void VendingMachine::insertNewBeverage(int index, QString name, int cost, int count, QString imagePath){
    if(!Manager::getInstance().isLogined()){
        WARNING("로그인 하지 않은 사용자는 접근할 수 없는 기능입니다.");
        return;
    }

    imagePath = "file:///" + FileUtil::copyFile(imagePath, "image");
    QSharedPointer<Beverage> beverage(new Beverage(index, name, cost, count, imagePath));
    insertBeverageList(beverageList, beverage);
}

void VendingMachine::removeBeverage(Beverage* beverage){
    if(!Manager::getInstance().isLogined()){
        WARNING("로그인 하지 않은 사용자는 접근할 수 없는 기능입니다.");
        return;
    }

    beverageList.remove(beverageList.search([&](QSharedPointer<Beverage> a){
        return beverage == a.data();
    }));
}

void VendingMachine::collection(){
    int defaultMoney = 1000*10 + 500*10 + 100*10 + 50*10 + 10*10;

    int totalMoney=0;

    bool isSame = true;

    for(const auto& iter : vmMoneyList){
        totalMoney += iter->cost() * iter->count();
        if(iter->count() != 10) isSame = false;
        iter->setCount(10);
    }

    if(isSame) return;

    logCsv["collection_log"].write({QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"),
                            QString::number(totalMoney),
                            QString::number(totalMoney - defaultMoney)});

    jsonUpdateVMMoney();
}
