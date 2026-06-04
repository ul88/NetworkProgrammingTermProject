#include "vendingmachine.h"

VendingMachine::VendingMachine(QObject *parent)
    : QObject{parent},
    vmMoneyList{Money::makeMoneyVector()}
{
    auto array = openJson(":/json/vm_beverage_data.json");
    if(array.empty()) {
        qWarning() << "오류 발생 종료";
        exit(1);
    }

    for(const QJsonValue& value : array){
        if(!value.isObject()) continue;

        QJsonObject obj = value.toObject();

        int index = obj["index"].toInt();
        QString name = obj["name"].toString();
        int cost = obj["cost"].toInt();
        int count = obj["count"].toInt();
        QString imagePath = obj["imagePath"].toString();

        insertBeverage(QSharedPointer<Beverage>(
            new Beverage(index, name, cost, count, imagePath)));
    }

    array = openJson(":/json/vm_money_data.json");
    if(array.empty()) {
        qWarning() << "오류 발생 종료";
        exit(1);
    }

    for(const QJsonValue& value : array){
        if(!value.isObject()) continue;

        QJsonObject obj = value.toObject();

        QString username = obj["username"].toString();
        QJsonArray moneyArray = obj["money"].toArray();

        for(int i=0;i<moneyArray.size();i++){
            qDebug()<<"자판기에 존재하는 "<< vmMoneyList[i]->cost()<<" 돈 : "<<moneyArray[i].toInt();
            vmMoneyList[i]->setCount(moneyArray[i].toInt());
        }
    }
}

const QJsonArray VendingMachine::openJson(const QString& path){
    QFile file(path);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qWarning() << "파일 열기 실패" << path;
        return QJsonArray();
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonParseError parserError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parserError);

    if(parserError.error != QJsonParseError::NoError){
        qWarning() << "Json 파싱 실패:" << parserError.errorString();
        return QJsonArray();;
    }

    if(!doc.isArray()){
        qWarning() << "JSON 최상위 구조가 배열 X";
        return QJsonArray();;
    }

    return doc.array();

}

Beverage* VendingMachine::getBeverage(int index){
    int i = beverageList.search([&](QSharedPointer<Beverage> a){
        return index == a->index();
    });

    if(i == beverageList.getSize()){
        qWarning() <<"인덱스 ("<<index<<")에는 데이터가 존재하지 않음";
        return nullptr;
    }

    auto ptr = beverageList.get(i);
    return ptr.data();
}

VendingMachine::SuccessType VendingMachine::addConsumerMoney(int cost){
    if(consumerMoneyList.empty()) consumerMoneyList = Money::makeMoneyVector();
    if(m_nowInputMoney > 7000) return SuccessType::FAIL;
    if(cost == 1000 && consumerMoneyList[0]->cost() == cost &&
        consumerMoneyList[0]->count() >= 5) return SuccessType::FAIL;
    for(auto iter : consumerMoneyList){
        if(iter->cost() == cost){
            iter->setCount(iter->count() + 1);
            setNowInputMoney(m_nowInputMoney + cost);
            return SuccessType::SUCCESS;
        }
    }
}

VendingMachine::SuccessType VendingMachine::buyBeverage(Beverage* beverage){
    if(consumerMoneyList.empty()) return SuccessType::FAIL;
    if(m_nowInputMoney == 0) return SuccessType::FAIL;
    if(beverage->count() <= 0) return SuccessType::FAIL;
    if(beverage->cost() == m_nowInputMoney){
        consumerMoneyList.clear();
        addOutputBeverage(beverage);
        setNowInputMoney(0);
        return SuccessType::PERFECT;
    }


    if(beverage->cost() < m_nowInputMoney){
        if(calcuateReturnMoney(m_nowInputMoney - beverage->cost())) {
            addOutputBeverage(beverage);
            setNowInputMoney(0);
            return SuccessType::SUCCESS;
        }
    }

    if(!returnMoneyList.empty()) {
        qWarning()<<"returnMoneyList가 비어있지 않으면 안됩니다. 오류 확인 필요";
        return SuccessType::FAIL;
    }

    returnMoneyList = Money::makeMoneyVector();
    copyToMoneyList(consumerMoneyList, returnMoneyList);
    consumerMoneyList.clear();
    setNowInputMoney(0);
    emit isReturnMoneyChanged();
    return SuccessType::FAIL;
}

bool VendingMachine::calcuateReturnMoney(int remainedCost){
    returnMoneyList = Money::makeMoneyVector();
    for(int i=0;i<vmMoneyList.size();i++){
        if(vmMoneyList[i]->cost() != consumerMoneyList[i]->cost()){
            qWarning() << "두 MoneyList의 Money 순서가 다릅니다.";
            return false;
        }
        if(vmMoneyList[i]->cost() > remainedCost) continue;
        int nowVMMoneyCount = vmMoneyList[i]->count() + consumerMoneyList[i]->count();
        int count = (remainedCost / vmMoneyList[i]->cost() > nowVMMoneyCount
                         ? nowVMMoneyCount : remainedCost / vmMoneyList[i]->cost());

        remainedCost -= vmMoneyList[i]->cost() * count;
        returnMoneyList[i]->setCount(count);
    }

    if(remainedCost == 0){
        addVMMoney();
        emit isReturnMoneyChanged();
        return true;
    }else{
        returnMoneyList.clear();
        return false;
    }
}

void VendingMachine::addVMMoney(){
    for(int i=0;i<vmMoneyList.size();i++){
        if(vmMoneyList[i]->cost() != consumerMoneyList[i]->cost()){
            qWarning() << "두 MoneyList의 Money 순서가 다릅니다.";
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
            qWarning() << "두 MoneyList의 Money 순서가 다릅니다.";
            return;
        }

        m->setCount(m->count() + returnMoneyList[i]->count());
    }

    returnMoneyList.clear();
    emit isReturnMoneyChanged();
}

void VendingMachine::copyToMoneyList(const QVector<QSharedPointer<Money>>& to, QVector<QSharedPointer<Money>>& from){
    for(int i=0;i<to.size();i++){
        if(to[i]->cost() != from[i]->cost()){
            qWarning() << "두 MoneyList의 Money 순서가 다릅니다.";
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
