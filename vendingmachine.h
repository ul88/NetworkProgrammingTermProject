#ifndef VENDINGMACHINE_H
#define VENDINGMACHINE_H

#include <QObject>
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <qqmlintegration.h>
#include "linkedlist.h"
#include "beverage.h"

class VendingMachine : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    explicit VendingMachine(QObject *parent = nullptr);

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

    Q_INVOKABLE Beverage* getBeverage(int index){
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

    void openJson(const QString& path);

signals:

private:
    LinkedList<QSharedPointer<Beverage>> beverageList;

    // index 별로 오름차순 입력을 유지하기 위함.
    void insertBeverage(QSharedPointer<Beverage> beverage){
        beverageList.insert(beverage,
                            beverageList.search([&](QSharedPointer<Beverage> a){
                                return beverage->index() < a->index();
                            }));
    }
};

#endif // VENDINGMACHINE_H
