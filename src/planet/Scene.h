#ifndef SCENE_H
#define SCENE_H

#include <QVector>
#include <QOpenGLFramebufferObject>
#include <QGLFunctions>
#include <QImage>
#include <QMatrix4x4>
#include <QVector3D>
#include <QMap>

#include "SceneObject.h"

class Scene
{
public:
    Scene(QString name, int width, int height);
    virtual ~Scene();

    /**
     * Add object into the scene
     * */
    void addObject(SceneObject* object);

    /**
     * Render all the visible objects in the scene.
     * Currently just draw all the objects in the 
     * scene, but possible to optimize. 
     * Render uses a framebuffer, therefore possible
     * for render-to-texture and post-processing.
     * */
    QImage render();

private:
    // uniform the necessary matrices
    void uniformMatrices(QGLShaderProgram &program);

    /**
     * Replace the OpenGL's matrices.
     * This requires all shaders for this engine
     * to provide uniform variables for these three
     * matrices.
     */
    QMatrix4x4 uMVMatrix;   // model-view matrix (model -> view space)
    QMatrix4x4 uPMatrix;    // projection matrix (view -> projection space)
    QMatrix4x4 uNMatrix;    // normal matrix (inverse of transpose of model-view matrix)

private:
    // Name for debugging and displaying
    QString name;

    // List of objects inside the scene
    QVector<SceneObject*> objects;

    // Framebuffer for texture rendering
    QOpenGLFramebufferObject framebuffer;
    // Result of rendering
    QImage image;
};

#endif /* end of include guard: SCENE_H */
