#ifndef CONSUMER_H
#define CONSUMER_H

#include <QObject>
#include <QQuickItem>
#include "money.h"
#include <QVector>
#include <QSharedPointer>

class Consumer : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    explicit Consumer(QObject* parent = nullptr);

    Q_INVOKABLE Money* get(int index){
        if(index < 0 || index >= moneyList.size()){
            qWarning() << "존재하지 않는 moneyList index";
            return nullptr;
        }

        return moneyList[index].data();
    }

    // money 객체를 하나 차감하는 함수
    Q_INVOKABLE void spendMoney(Money* money){
        money->setCount(money->count() - 1);
    }
private:
    QVector<QSharedPointer<Money>> moneyList;
};

#endif // CONSUMER_H
