#ifndef BEVERAGE_H
#define BEVERAGE_H

#include <QObject>
#include <qqmlintegration.h>

// 음료에 대한 정보를 담는 QObject
// 음료의 이름, 음료의 비용, 음료의 개수를 담을 수 있음
// 다음 정보들이 변할 때마다 서버에게 전달하게 됨.
class Beverage : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged FINAL)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(int cost READ cost WRITE setCost NOTIFY costChanged FINAL)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged FINAL)
    Q_PROPERTY(QString imagePath READ imagePath CONSTANT)
public:
    explicit Beverage();
    explicit Beverage(int index, QString name, int cost, int count, QString imagePath);

    int index() const {return m_index;}
    QString name() const {return m_name;}
    int cost() const {return m_cost;}
    int count() const {return m_count;}
    QString imagePath() const {return m_imagePath;}

    void setIndex(int index){
        if(m_index == index) return;
        m_index = index;
        emit indexChanged();
    }

    void setName(QString& name){
        if(m_name == name) return;
        m_name = name;
        emit nameChanged();
    }

    void setCost(int cost){
        if(m_cost == cost) return;
        m_cost = cost;
        emit costChanged();
    }

    void setCount(int count){
        if(m_count == count) return;
        m_count = count;
        emit countChanged();
    }
signals:
    void indexChanged();
    void nameChanged();
    void costChanged();
    void countChanged();
private:
    int m_index;
    QString m_name;
    int m_cost;
    int m_count;
    QString m_imagePath;
};

#endif // BEVERAGE_H
