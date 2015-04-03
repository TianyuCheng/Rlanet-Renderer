#ifndef SELECTGL_H
#define SELECTGL_H

#include <QOpenGLFunctions_3_3_Core>

typedef QOpenGLFunctions_3_3_Core GLCore;

#define CHECK_GL_ERROR(loc)    do {    \
       int ec = glGetError();          \
       if (ec != GL_NO_ERROR) {        \
               qDebug("GL Error %d in function %s. Location: %s", ec, __func__, loc);\
       }                               \
} while(0)\

#define REPORT_GL_STATUS(loc)  do {    \
       int ec = glGetError();          \
       if (ec != GL_NO_ERROR) {        \
               qDebug("[GL Error] %d in function %s. Location: %s", ec, __func__, loc);\
       } else {                                \
               qDebug("[GL Works] at function %s. Location: %s", __func__, loc);\
       }\
} while(0)\

#endif
