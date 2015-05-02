#ifndef NEXUS_H
#define NEXUS_H

#include <vector>
#include <QtGui/QSurfaceFormat>
#include <QtCore/QString>

class QThread;

class nexus {
public:
	static QSurfaceFormat select_gl(QSurfaceFormat);
	static QString get_scene_name();
	static void register_thread(QThread*);
	static void terminate();
private:
	static std::vector<QThread*> threads_;
};

#endif
