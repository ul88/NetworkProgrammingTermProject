#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QQmlEngine>

class Manager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    explicit Manager(QObject *parent = nullptr);
    Q_INVOKABLE bool login(QString password);


signals:
};

#endif // MANAGER_H
