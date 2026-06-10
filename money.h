#ifndef MONEY_H
#define MONEY_H

#include <QObject>
#include <QQuickItem>
#include <QList>

class Money : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QString imagePath READ imagePath CONSTANT)
    Q_PROPERTY(int cost READ cost CONSTANT)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged FINAL)
public:
    explicit Money(QObject *parent = nullptr);
    explicit Money(QString imagePath, int cost, int count, QObject* parent = nullptr);
    explicit Money(QString imagePath, int cost, QObject* parent = nullptr);

    static QList<QSharedPointer<Money>> makeMoneyVector(){
        return {
            QSharedPointer<Money>(new Money("qrc:/images/1000won.png",1000)),
            QSharedPointer<Money>(new Money("qrc:/images/500won.png",500)),
            QSharedPointer<Money>(new Money("qrc:/images/100won.png",100)),
            QSharedPointer<Money>(new Money("qrc:/images/50won.png",50)),
            QSharedPointer<Money>(new Money("qrc:/images/10won.png",10))
        };
    }

    QString imagePath() const {return m_imagePath;}
    int cost() const {return m_cost;}
    int count() const {return m_count;}

    void setCount(int count){
        if(m_count == count) return;
        m_count = count;

        emit countChanged();
    }

signals:
    void countChanged();
private:
    QString m_imagePath;
    int m_cost;
    int m_count;
};

#endif // MONEY_H
