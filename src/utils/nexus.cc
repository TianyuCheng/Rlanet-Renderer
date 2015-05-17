#include <QSurfaceFormat>
#include <QObject>
#include <QtCore/QThread>
#include "nexus.h"

std::vector<QThread*> nexus::threads_;

QSurfaceFormat nexus::select_gl(QSurfaceFormat fmt)
{
	fmt.setVersion(3,3);
	fmt.setProfile(QSurfaceFormat::CoreProfile);
	return fmt;
}

QString nexus::get_scene_name()
{
	return QObject::tr("Test scene");
}

void nexus::terminate()
{
	for(auto th : threads_) {
		th->wait();
		delete th;
	}
}

void nexus::register_thread(QThread* th)
{
	threads_.emplace_back(th);
}
