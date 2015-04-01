/**utilities 
 * main.cpp
 * */
#include <QApplication>  

#include "mainwindow.h"
#include "Scene.h"
#include "Planet.h"
#include "Terrain.h"
#include "Mangekyou.h"
#include "nexus.h"

int main(int argc, char *argv[])  
{  
	QGuiApplication app(argc, argv);
	qmlRegisterType<Mangekyou>("Mangekyou", 1, 0, "Renderer");

	{
		QQuickView view;
		view.setFormat(nexus::select_gl(view.format()));
		view.setPersistentOpenGLContext(true);
		view.setPersistentSceneGraph(true);
		view.setResizeMode(QQuickView::SizeRootObjectToView);
		view.setSource(QUrl("qrc:///main.qml"));
		view.show();
		int ret = app.exec();
	} // After this view will be destructed
	nexus::terminate();
	return ret;
}
