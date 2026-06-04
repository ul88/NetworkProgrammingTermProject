#include "consumer.h"

Consumer::Consumer(QObject* parent)
    : QObject{parent}, moneyList{Money::makeMoneyVector()}
{
    for(auto iter : moneyList){
        iter->setCount(10);
    }
}
