#ifndef SELECTGL_H
#define SELECTGL_H

#include <QOpenGLFunctions_3_3_Core>
#include <GraphicsDevice.h>

#define CHECK_GL_ERROR(device, loc)	do {	\
	int ec = device->glGetError();		\
	if (ec != GL_NO_ERROR) {	\
		qDebug("GL Error %d in function %s. Location: %s", ec, __func__, loc);\
	}				\
} while(0)

#define REPORT_GL_STATUS(device, loc)  do {	\
	int ec = device->glGetError();		\
	if (ec != GL_NO_ERROR) {	\
		qDebug("[GL Error] %d in function %s. Location: %s", ec, __func__, loc);\
	} else {			\
		qDebug("[GL Works] at function %s. Location: %s", __func__, loc);\
	}				\
} while(0)

#endif
