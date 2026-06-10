#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include<QObject>
#include <qqmlintegration.h>
#include <QDebug>
#include <QQmlEngine>
#include <QFileInfo>
#include <QJSEngine>
// send 함수를 축약하기 위한 매크로 함수
#define DEBUG(X, ...) \
    MyProgram::ErrorHandler::getInstance().send( \
        MyProgram::formatMessage(QString(X), ##__VA_ARGS__), \
        MyProgram::ErrorHandler::DebugLevel::Debug, \
        __FILE__, __LINE__, Q_FUNC_INFO \
    )
#define INFO(X, ...) \
    MyProgram::ErrorHandler::getInstance().send( \
        MyProgram::formatMessage(QString(X), ##__VA_ARGS__), \
        MyProgram::ErrorHandler::DebugLevel::Info, \
        __FILE__, __LINE__, Q_FUNC_INFO \
    )
#define WARNING(X, ...) \
    MyProgram::ErrorHandler::getInstance().send( \
        MyProgram::formatMessage(QString(X), ##__VA_ARGS__), \
        MyProgram::ErrorHandler::DebugLevel::Warning, \
        __FILE__, __LINE__, Q_FUNC_INFO \
    )
#define CRITICAL(X, ...) \
    MyProgram::ErrorHandler::getInstance().send( \
        MyProgram::formatMessage(QString(X), ##__VA_ARGS__), \
        MyProgram::ErrorHandler::DebugLevel::Critical, \
        __FILE__, __LINE__, Q_FUNC_INFO \
    )

namespace MyProgram{

// QString의 arg를 이용하기 위한 템플릿 함수
template<typename... Args>
static QString formatMessage(QString msg, Args&&... args)
{
    ((msg = msg.arg(std::forward<Args>(args))), ...);
    return msg;
}

// 에러 핸들링을 위한 함수
class ErrorHandler : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT
public:

    // Debug 레벨을 4개로 설정
    // 개발 단계에서는 Debug까지 뜨지만, 배포 단게에서는 Debug는 안뜨도록 설정
    enum DebugLevel{
        Debug,
        Info,
        Warning,
        Critical
    };
    Q_ENUM(DebugLevel)

    // 싱글톤을 위한 인스턴스 함수
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }

    // QML, C++에 전부 싱글톤 등록을 자동화하기 위한 함수
    static ErrorHandler* create(QQmlEngine*, QJSEngine*){
        ErrorHandler* instance = &getInstance();
        QQmlEngine::setObjectOwnership(instance, QQmlEngine::CppOwnership);
        return instance;
    }

    // Debug Level을 임의로 변경하기 위함
    void setDebugLevel(DebugLevel level){
        if(m_level == level) return;
        m_level = level;
    }

    // 에러 메세지 호출
    void send(QString e, DebugLevel level = DebugLevel::Debug,
            const char* file = nullptr,
            int line = 0,
            const char* func = nullptr)
    {
        if(m_level > level) return;

        QString location;

        if (file != nullptr) {
            location = QString("[%1:%2]")
                .arg(QFileInfo(file).fileName())
                .arg(line);
        }

        QString funcInfo;

        if (func != nullptr) {
            funcInfo = QString(" | %1").arg(func);
        }

        QString msg = QString("%1 %2")
                            .arg(location)
                            .arg(e);


        if(level == DebugLevel::Debug) qDebug() << "DEBUG : " << msg;
        else if(level == DebugLevel::Info) qInfo() << "INFO : " << msg;
        else if(level == DebugLevel::Warning) qWarning() << "WARNING : " << msg;
        else if(level == DebugLevel::Critical) qCritical() << "CRITICAL : " << msg;

        // QML에서는 함수 호출부까지 공개할 이유가 없기 때문에 처음 에러 메세지만 전달
        emit errorOccurred(e, level);
    }
signals:

    // QML에서 시그널을 받기 위한 에러 발생 시그널
    void errorOccurred(QString msg, DebugLevel level);
private:
    explicit ErrorHandler(QObject* parent = nullptr) : QObject{parent} {
#ifdef QT_DEBUG
        m_level = DebugLevel::Debug;
#else
        m_level = DebugLevel::Info;
#endif
    }
    Q_DISABLE_COPY(ErrorHandler)

    DebugLevel m_level;
};
}

#endif // ERRORHANDLER_H
