#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include<QObject>
#include <qqmlintegration.h>
#include <QDebug>
#include <QQmlEngine>
#include <QJSEngine>

namespace MyProgram{
    class ErrorHandler : public QObject
    {
        Q_OBJECT
        QML_SINGLETON
        QML_ELEMENT
    public:

        enum DebugLevel{
            Debug,
            Info,
            Warning,
            Critical
        };
        Q_ENUM(DebugLevel)

        static ErrorHandler& getInstance() {
            static ErrorHandler instance;
            return instance;
        }

        static ErrorHandler* create(QQmlEngine*, QJSEngine*){
            ErrorHandler* instance = &getInstance();
            QQmlEngine::setObjectOwnership(instance, QQmlEngine::CppOwnership);
            return instance;
        }

        void setDebugLevel(DebugLevel level){
            if(m_level == level) return;
            m_level = level;
        }

        void send(QString e, DebugLevel level = DebugLevel::Debug){
            if(m_level > level) return;
            if(level == DebugLevel::Debug) qDebug() << e;
            else if(level == DebugLevel::Info) qInfo() << e;
            else if(level == DebugLevel::Warning) qWarning() << e;
            else if(level == DebugLevel::Critical) qCritical() << e;

            emit errorOccurred(e, level);
        }
    signals:
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
