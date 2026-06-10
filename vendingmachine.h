#ifndef VENDINGMACHINE_H
#define VENDINGMACHINE_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QStack>
#include "money.h"
#include <QList>
#include <qqmlintegration.h>
#include "linkedlist.h"
#include "beverage.h"
#include "consumer.h"
#include "csv.h"
#include "FileUtil.h"
#include <QDateTime>
#include "manager.h"

class VendingMachine : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool isReturnMoney READ isReturnMoney NOTIFY isReturnMoneyChanged FINAL)
    Q_PROPERTY(int nowInputMoney READ nowInputMoney WRITE setNowInputMoney NOTIFY nowInputMoneyChanged FINAL)
    Q_PROPERTY(QString topOutputName READ topOutputName NOTIFY stackTopChanged FINAL)
    Q_PROPERTY(QString topOutputImagePath READ topOutputImagePath NOTIFY stackTopChanged FINAL)
public:

    enum SuccessType{
        SUCCESS,
        FAIL_7000,
        FAIL_5000,
        FAIL_EMPTY,
        FAIL_SOLD_OUT,
        FAIL_INCORRECT_COST,
        FAIL_NOT_HAVE_MONEY,
        FAIL
    };
    Q_ENUM(SuccessType)

    enum SortType{
        ASC,
        DESC
    };
    Q_ENUM(SortType)

    explicit VendingMachine(QObject *parent = nullptr);

    // 반환할 돈이 있는지 검사하는 함수
    bool isReturnMoney() const{
        return !returnMoneyList.empty();
    }

    // nowInputMoney getter
    int nowInputMoney() const {return m_nowInputMoney;}

    // nowInputMoney setter
    void setNowInputMoney(int nowInputMoney){
        if(m_nowInputMoney == nowInputMoney) return;
        m_nowInputMoney = nowInputMoney;
        emit nowInputMoneyChanged();
    }

    // 구매 완료된 음료 객체들을 담은 Stack의 top의 name
    QString topOutputName() const {
        if(outputBeverageStack.empty()) return "";
        return outputBeverageStack.top()->name();
    }

    // 구매 완료된 음료 객체들을 담은 Stack의 top의 imagePath
    QString topOutputImagePath() const{
        if(outputBeverageStack.empty()) return "";
        return outputBeverageStack.top()->imagePath();
    }

    // QML에서 사용할 수 있도록 등록된 함수
public slots:
    // 반환될 돈의 리스트를 반환하는 getter
    QVariantList getReturnMoneyList(){
        QVariantList ret;
        for(const auto& iter : returnMoneyList){
            ret.append(QVariant::fromValue(iter.data()));
        }
        return ret;
    }

    // index에 해당하는 음료 포인터를 반환하는 getter
    Beverage* getBeverage(int index);

    Money* getMoney(int index);

    // 사용자 넣은 돈을 추가해주는 함수
    VendingMachine::SuccessType addConsumerMoney(int cost);

    // 음료 구매 함수
    // 음료 구매에 실패하면 넣은 금액 그대로 반환됨.
    // 반환된 금액은 returnMoneyList에 담기게 됨.
    // returnMoney 함수를 통해 Consumer에 돈이 그대로 옮겨지게 됨.
    //
    // 동작 :
    //  consumerMoneyList는 사용자가 넣은 돈을 저장하는 변수. -> 이 변수는 구매할 수 없을 때 그대로 뱉기 위함.
    //  moneySum을 통해 음료를 살 수 있는지 검사
    //      만약 음료와 낸 돈이 동일하다면 사용자에게 반환할 돈이 없으므로 그대로 종료
    //      만약 음료가 낸 돈보다 적은 돈이 필요하다면 사용자에게 반환할 돈을 계산
    //          이때, 사용자에게 반환이 불가능하다면, 반환 불가능을 나타내고 그대로 반환함.
    VendingMachine::SuccessType buyBeverage(Beverage* beverage);

    // 사용자가 반환된 돈을 회수하려고 할 때 동작하는 함수
    void returnMoney(Consumer* consumer);

    // 반환된 음료를 제거하는 함수 -> 이 함수가 돌아야 쓸모없어진 음료 객체가 삭제됨
    void removeOutputBeverage();

    // log name에 해당하는 csv에 담긴 로그를 반환하는 getter
    // sortType이 ASC라면 오름차순 정렬이고 DESC이면 내림차순 정렬
    // 정렬 기준은 headerName에 따라 결정됨
    QVariantList getLogList(QString name, QString headerName ,VendingMachine::SortType sortType = VendingMachine::ASC);

    // log name에 해당하는 csv에 담긴 로그의 헤더를 반환하는 getter
    QVariantList getCsvHeader(QString name);

    // 가지고 있는 모든 logFile list를 반환하는 함수
    QVariantList getLogFileList();

    // 음료의 이름을 수정하는 함수
    void changeBeverageName(Beverage* beverage, QString newName);

    // 음료의 인덱스 즉, 출구 위치를 수정하는 함수
    void changeBeverageIndex(Beverage* beverage, int newIndex);

    // 음료의 금액을 수정하는 함수
    void changeBeverageCost(Beverage* beverage, int newCost);

    // 음료의 개수를 수정하는 함수
    void changeBeverageCount(Beverage* beverage, int newCount);

    // 새로운 음료를 추가하는 함수
    void insertNewBeverage(int index, QString name, int cost, int count, QString imagePath);

    // beverageList에서 beverage를 삭제하는 함수
    void removeBeverage(Beverage* beverage);

    // 파일을 다시 불러와서 파일대로 재설정하는 함수
    void reload();

    // 자판기의 음료 값들을 json에 다시 쓰는 함수
    void jsonUpdateVMBeverage();

    // 자판기의 돈 값들을 json에 다시 쓰는 함수
    void jsonUpdateVMMoney();

    // 수금을 위한 함수
    void collection();
signals:
    void isReturnMoneyChanged();
    void nowInputMoneyChanged();
    void stackTopChanged();
private:
    QMap<QString, QString> filePaths;
    QMap<QString, FileUtil::Csv> logCsv;
    LinkedList<QSharedPointer<Beverage>> beverageList;
    QStack<QSharedPointer<Beverage>> outputBeverageStack;
    QList<QSharedPointer<Money>> vmMoneyList;
    QList<QSharedPointer<Money>> consumerMoneyList;
    QList<QSharedPointer<Money>> returnMoneyList;
    int m_nowInputMoney = 0;

    // 구매를 하였을 때 반환해야하는 금액 리스트를 전달
    // first가 false라면 반환을 실패한 것이므로 새로 투입하라고 사용자에게 안내해야함.
    // first가 true라면 second에 반환할 moneyList가 제공됨.
    bool calcuateReturnMoney(int remainedCost);

    void addVMMoney();

    void copyToMoneyList(const QList<QSharedPointer<Money>>& to, QList<QSharedPointer<Money>>& from);

    void addOutputBeverage(Beverage* beverage);

    void returnAsMoney();

    void jsonUpdate(Beverage* beverage);

    void insertBeverageList(LinkedList<QSharedPointer<Beverage>>& list,
                            QSharedPointer<Beverage>& beverage){
        list.insert(beverage,
                    list.search([&](QSharedPointer<Beverage> a){
                        return beverage->index() < a->index();
                    }));
    }

    LinkedList<QSharedPointer<Beverage>> readVMBeverageDataFromJson(const QString& path){
        auto doc = FileUtil::openJson(path);
        if(doc.array().empty()) {
            WARNING("정해진 Json 규격에 맞지 않음.");
            exit(1);
        }

        LinkedList<QSharedPointer<Beverage>> ret;

        for(const QJsonValue& value : doc.array()){
            if(!value.isObject()) continue;

            QJsonObject obj = value.toObject();

            int index = obj["index"].toInt();
            QString name = obj["name"].toString();
            int cost = obj["cost"].toInt();
            int count = obj["count"].toInt();
            QString imagePath = obj["imagePath"].toString();

            QSharedPointer<Beverage> beverage(new Beverage(index, name, cost, count, imagePath));
            insertBeverageList(ret, beverage);
        }
        return ret;
    }

    QList<QSharedPointer<Money>> readVMMoneyDataFromJson(const QString& path){
        auto doc = FileUtil::openJson(path);
        if(doc.isEmpty()) {
            WARNING("정해진 Json 규격에 맞지 않음.");
            exit(1);
        }

        QList<QSharedPointer<Money>> ret = Money::makeMoneyVector();

        auto jsonObj = doc.object();
        auto moneyArray = jsonObj["money"].toArray();

        for(int i=0;i<moneyArray.size();i++){
            ret[i]->setCount(moneyArray[i].toInt());
        }

        return ret;
    }
};

#endif // VENDINGMACHINE_H
