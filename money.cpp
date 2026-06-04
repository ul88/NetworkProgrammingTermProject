#include "money.h"

Money::Money(QObject *parent)
    : QObject{parent}, m_cost{0}, m_count{0}, m_imagePath{""}
{
}

Money::Money(QString imagePath, int cost, int count, QObject* parent)
    : m_imagePath{imagePath}, m_cost{cost}, m_count{count}, QObject{parent}
{

}

Money::Money(QString imagePath, int cost, QObject* parent)
    : m_imagePath{imagePath}, m_cost{cost}, m_count{0}, QObject{parent}
{

}
