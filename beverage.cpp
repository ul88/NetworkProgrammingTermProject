#include "beverage.h"

Beverage::Beverage()
    : m_index{0}, m_name{""}, m_cost{0}, m_count{0}, m_imagePath{""}
{

}

Beverage::Beverage(int index, QString name, int cost, int count, QString imagePath)
    : m_index{index}, m_name{name}, m_cost{cost}, m_count{count}, m_imagePath{imagePath}
{

}
