#ifndef NEXUS_H
#define NEXUS_H

#include <QtGui/QSurfaceFormat>
#include <QtCore/QString>

class QThread;

class nexus {
public:
	static QSurfaceFormat select_gl(QSurfaceFormat);
	static QString get_scene_name();
	static void register_thread(QThread*);
	static void terminate();
};

#endif
