#ifndef VENDINGMACHINE_H
#define VENDINGMACHINE_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QStack>
#include "money.h"
#include <QVector>
#include <qqmlintegration.h>
#include "linkedlist.h"
#include "beverage.h"
#include "consumer.h"

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
        PERFECT,
        SUCCESS,
        FAIL
    };

    explicit VendingMachine(QObject *parent = nullptr);

    // 반환할 돈이 있는지 검사하는 함수
    bool isReturnMoney() const{
        return !returnMoneyList.empty();
    }

    int nowInputMoney() const {return m_nowInputMoney;}
    void setNowInputMoney(int nowInputMoney){
        if(m_nowInputMoney == nowInputMoney) return;
        m_nowInputMoney = nowInputMoney;
        emit nowInputMoneyChanged();
    }

    QString topOutputName() const {
        if(outputBeverageStack.empty()) return "";
        return outputBeverageStack.top()->name();
    }

    QString topOutputImagePath() const{
        if(outputBeverageStack.empty()) return "";
        return outputBeverageStack.top()->imagePath();
    }

    Q_INVOKABLE QVariantList getReturnMoneyList(){
        QVariantList ret;
        for(const auto& iter : returnMoneyList){
            ret.append(QVariant::fromValue(iter.data()));
        }
        return ret;
    }

    Q_INVOKABLE void printAll(){

        qDebug()<<"개수: "<<beverageList.getSize();
        for(int i=0;i<beverageList.getSize();i++){
            auto data = beverageList.get(i);
            if(data == nullptr){
                qDebug()<<"탐지 불가";
                return;
            }
            qDebug()<<i<<"번째 데이터";
            qDebug()<<data->index();
            qDebug()<<data->name();
            qDebug()<<data->cost();
            qDebug()<<data->count();
            qDebug()<<data->imagePath();
            qDebug()<<"--------------------------";
        }
    }

    Q_INVOKABLE Beverage* getBeverage(int index);

    // 사용자 넣은 돈을 추가해주는 함수
    Q_INVOKABLE SuccessType addConsumerMoney(int cost);

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
    Q_INVOKABLE SuccessType buyBeverage(Beverage* beverage);

    Q_INVOKABLE void returnMoney(Consumer* consumer);

    Q_INVOKABLE void removeOutputBeverage();

    const QJsonArray openJson(const QString& path);

signals:
    void isReturnMoneyChanged();
    void nowInputMoneyChanged();
    void stackTopChanged();
private:
    LinkedList<QSharedPointer<Beverage>> beverageList;
    QStack<QSharedPointer<Beverage>> outputBeverageStack;
    QVector<QSharedPointer<Money>> vmMoneyList;
    QVector<QSharedPointer<Money>> consumerMoneyList;
    QVector<QSharedPointer<Money>> returnMoneyList;
    int m_nowInputMoney = 0;

    // index 별로 오름차순 입력을 유지하기 위함.
    void insertBeverage(QSharedPointer<Beverage> beverage){
        beverageList.insert(beverage,
                            beverageList.search([&](QSharedPointer<Beverage> a){
                                return beverage->index() < a->index();
                            }));
    }

    // 구매를 하였을 때 반환해야하는 금액 리스트를 전달
    // first가 false라면 반환을 실패한 것이므로 새로 투입하라고 사용자에게 안내해야함.
    // first가 true라면 second에 반환할 moneyList가 제공됨.
    bool calcuateReturnMoney(int remainedCost);

    void addVMMoney();

    void copyToMoneyList(const QVector<QSharedPointer<Money>>& to, QVector<QSharedPointer<Money>>& from);

    void addOutputBeverage(Beverage* beverage);
};

#endif // VENDINGMACHINE_H
