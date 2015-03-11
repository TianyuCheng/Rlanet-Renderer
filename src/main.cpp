/**
 * main.cpp
 * */
#include <QApplication>
#include <QObject>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlEngine>
#include <QtQuick/QQuickWindow>

#include "./gui/MainWindow.h"

int main(int argc, char *argv[])
{
    // QApplication app(argc, argv);
    //
    // // Register our component type with QML.
    // qmlRegisterType<MainWindow>("Planet Renderer", 1, 0, "MainWindow");
    //
    // int rc = 0;
    //
    // QQmlEngine engine;
    // QQmlComponent *component = new QQmlComponent(&engine);
    //
    // QObject::connect(&engine, SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));
    //
    // component->loadUrl(QUrl("qml/MainWindow.qml"));
    //
    // if (!component->isReady() ) {
    //     qWarning("%s", qPrintable(component->errorString()));
    //     return -1;
    // }
    //
    // QObject *topLevel = component->create();
    // QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
    //
    // QSurfaceFormat surfaceFormat = window->requestedFormat();
    // window->setFormat(surfaceFormat);
    // window->show();
    //
    // rc = app.exec();
    //
    // delete component;
    // return rc;
}

