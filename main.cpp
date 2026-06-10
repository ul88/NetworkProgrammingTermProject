#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "manager.h"
#include <QQuickStyle>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    QQuickStyle::setStyle("Fusion");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("vending_machine", "Main");

    return app.exec();
}
