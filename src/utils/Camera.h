#ifndef CAMERA_H
#define CAMERA_H

#include <QtMath>
#include <QString>
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QOpenGLShaderProgram>

#include <BoundingBox.h>

class Camera
{
public:
    Camera(QString n);
    virtual ~Camera ();

    void initialize();

    /**
     * ModelView matrix manipulatiuon
     * */
    void setLook(QVector3D look);
    void setCenter(QVector3D center);
    void setUp(QVector3D up);
    void lookAt(QVector3D look, QVector3D center, QVector3D up);
    void moveForward(double distance);      // move towards the look
    void moveBackward(double distance);     // move away from look
    void turnLeft(double angle, QVector3D axis = QVector3D(0, 1, 0));            // angle is in degree
    void turnRight(double angle, QVector3D axis = QVector3D(0, 1, 0));           // angle is in degree

    /**
     * Projection matrix manipulation
     * */
    void setLeft(double left);
    void setRight(double right);
    void setbottom(double bottom);
    void setTop(double top);
    void setNear(double near);
    void setFar(double far);
    void setFrustum(double left,    double right,
                    double bottom,  double top,
                    double near,    double far);

    void setFOV(double fov);        // this fov is only for vertical angle of view
    void setAspect(double aspect);  // this aspect is for horizontal
    void setPerspective(double fovy, double aspect, double zNear, double zFar);

    // GLSL related functions
    void uniformMatrices(QOpenGLShaderProgram &program);

    // Spatial optimization with cull
    enum Cullable { 
        TOTALLY_CULLABLE = 0,
        PARTIALLY_CULLABLE,
        NOT_CULLABLE
    };
    Cullable isCullable(BoundingBox box);
    
private:
    QString name;       // For debugging purpose

    // Camera position
    QVector3D look;
    QVector3D center;
    QVector3D up;

    // direction is computed using (look - center)
    // It is not normalized because the length of 
    // direction can be used for turn left/right
    QVector3D direction;  

    // Viewing frustum
    double near, far;
    double left, right;
    double top, bottom;

    /**
     * Replace the OpenGL's matrices.
     * This requires all shaders for this engine
     * to provide uniform variables for these three
     * matrices.
     */
    QMatrix4x4 uMVMatrix;   // model-view matrix (model -> view space)
    QMatrix4x4 uPMatrix;    // projection matrix (view -> projection space)
    QMatrix4x4 uNMatrix;    // normal matrix (inverse of transpose of model-view matrix)
};

#endif /* end of include guard: CAMERA_H */
