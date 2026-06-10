#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QQmlEngine>
#include "ErrorHandler.h"

class Manager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(bool isLogined READ isLogined FINAL)
public:
    // 싱글톤을 위한 인스턴스 함수
    static Manager& getInstance() {
        static Manager instance;
        return instance;
    }

    // QML, C++에 전부 싱글톤 등록을 자동화하기 위한 함수
    static Manager* create(QQmlEngine*, QJSEngine*){
        Manager* instance = &getInstance();
        QQmlEngine::setObjectOwnership(instance, QQmlEngine::CppOwnership);
        return instance;
    }

    Q_INVOKABLE bool login(QString password);

    bool isLogined() const {return m_isLogined;}

signals:

private:
    explicit Manager(QObject *parent = nullptr);

    bool m_isLogined;
    Q_DISABLE_COPY(Manager)
};

#endif // MANAGER_H
