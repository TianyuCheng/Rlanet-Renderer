#ifndef APPLICATION_H
#define APPLICATION_H

#include <QGuiApplication>  
#include <QQuickView>
#include <QString>
#include <QVector>
#include <QSize>

#include <Mangekyou.h>
#include <nexus.h>

#include <RenderManager.h>
#include <RenderThread.h>

class Application
{
public:
    Application(QString name, int argc, char *argv[]);
    virtual ~Application ();

    // run the application
    int exec();

    void registerRenderManager(RenderManager *r);

private:
    QString name;
	QGuiApplication app;
};

#endif /* end of include guard: APPLICATION_H */
