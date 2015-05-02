#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <QSize>
#include <QVector>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QMap>
#include <QTime>
#include <QImage>
#include <QOpenGLTexture>

#include <SceneObject.h>
#include <Camera.h>

class Scene : public SceneObject
{
public:
    Scene(QString name, QSize resolution);
    virtual ~Scene();

    /**
     * Add object into the scene
     * */
    void addObject(SceneObject* object);

    /**
     * Render all visible objects to framebuffer.
     *
     * Output: current Framebuffer object
     *
     * Note: Currently it just draws all the objects in the 
     * scene, but possible to optimize. 
     * */
    void renderScene(QOpenGLFramebufferObject*);

    /**
     * Scene Update
     * */
    void update() {}
    void uniform() {}

    /**
     * Camera
     * */
    void setCamera(Camera *cam) { camera = cam; }
    Camera* getCamera() { return camera; }

    void resize(QSize res);

private:
    // Name for debugging and displaying
    QString name;
    QSize resolution;

    // Camera object encapsulation
    Camera *camera;

    // List of objects inside the scene
    QVector<SceneObject*> objects;
};

#endif /* end of include guard: SCENE_H */
