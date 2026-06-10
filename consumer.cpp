#include "consumer.h"

Consumer::Consumer(QObject* parent)
    : QObject{parent}, moneyList{Money::makeMoneyVector()}
{
    for(const auto& iter : moneyList){
        iter->setCount(10);
    }
}
