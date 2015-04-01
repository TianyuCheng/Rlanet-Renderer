#include <QSurfaceFormat>
#include "nexus.h"

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
