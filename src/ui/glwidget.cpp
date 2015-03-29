#include <QtGui>
#include <QtOpenGL>

#include <cmath>

#include "glwidget.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
    scene = nullptr;
    init = true;
}

GLWidget::~GLWidget()
{
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}

void GLWidget::setScene(Scene *scene) {
    this->scene = scene;
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360 * 16)
        angle -= 360 * 16;
}

void GLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // Disable lighting because we are drawing
    // scenes to textures. 2D image is sufficent
    glDisable(GL_LIGHTING);

    glShadeModel(GL_SMOOTH);
}

void GLWidget::paintGL()
{
    if (init && scene) {
        scene->start();
        init = false;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glLoadIdentity();

    // Draw the scene's snapshot if there is a scene
    if (scene) {
        QImage image = scene->renderScene();
        GLuint texture = bindTexture(image, GL_TEXTURE_2D, GL_RGBA);
        drawTexture(QRectF(QPointF(-1, 1), QPointF(1, -1)), texture);
        deleteTexture(texture);
    }

    // could use an interval here, but currently just refresh it
    swapBuffers();
    update();
}

void GLWidget::resizeGL(int width, int height)
{
    // Change the viewport to the whole screen
    glViewport(0, 0, width, height);

    // Clear current buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Adjust the project matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // use gluOrtho2D
    glOrtho(-1.0, +1.0, -1.0, +1.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + 8 * dy);
        setYRotation(yRot + 8 * dx);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + 8 * dy);
        setZRotation(zRot + 8 * dx);
    }
    lastPos = event->pos();
}
