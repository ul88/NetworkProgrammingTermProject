#include "manager.h"

Manager::Manager(QObject *parent)
    : QObject{parent}
{}

bool Manager::login(QString password){
    return true;
}
