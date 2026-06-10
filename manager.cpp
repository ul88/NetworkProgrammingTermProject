#include "manager.h"

Manager::Manager(QObject *parent)
    : QObject{parent}, m_isLogined{false}
{
}

bool Manager::login(QString password){
    return m_isLogined = true;
}
