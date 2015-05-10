#include <QQmlEngine> 
#include <QQmlContext> 
#include <Application.h>

Application::Application(QString n, int argc, char *argv[]) 
: name(n), app(argc, argv)
{
	qmlRegisterType<Mangekyou>("Mangekyou", 1, 0, "Renderer");
}

Application::~Application() {

}

void Application::registerRenderManager(RenderManager *mgr) {
	RenderThread::renderMgr = mgr;
}

int Application::exec() {
	int ret;
	{
		QQuickView view;
		view.setFormat(nexus::select_gl(view.format()));
		view.setPersistentOpenGLContext(true);
		view.setPersistentSceneGraph(true);
		view.setResizeMode(QQuickView::SizeRootObjectToView);
		view.engine()->rootContext()->setContextProperty("appPath", QUrl(QString("file://%1").arg(app.applicationDirPath())));
		view.setSource(QUrl("qrc:///main.qml"));

		QObject::connect(view.engine(), SIGNAL(quit()), &app, SLOT(quit()));

		if (RenderThread::renderMgr) 
			RenderThread::renderMgr->setContext(view.engine()->rootContext());

		view.show();

		ret = app.exec();
	} // After this view will be destructed
	nexus::terminate();
	return ret;
}
