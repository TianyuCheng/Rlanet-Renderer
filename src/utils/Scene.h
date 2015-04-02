#ifndef SCENE_H
#define SCENE_H

#include <QVector>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QImage>
#include <QMatrix4x4>
#include <QVector3D>
#include <QMap>
#include <QTime>

#include <SceneObject.h>
#include <Camera.h>

class Scene : public SceneObject
{
public:
    Scene(QString name, int width, int height);
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
    void renderScene();

    /**
     * Scene Update
     * */
    void update() {}
    void uniform() {}

    /**
     * Camera
     * */
    Camera* getCamera() { return &camera; }

    /**
     * Time Utils
     * Scene time control with start, pause, resume, and restart.
     * As there is no pause and stop in the Qt, we fake it through
     * subtracting the elapsed time right before paused.
     * */
    void start();
    void pause();
    void resume();
    void restart();
    double timeElapsed() const;

    void resize(GLuint width, GLuint height);
private:
    // Name for debugging and displaying
    QString name;
    int width, height;

    // Camera object encapsulation
    Camera camera;

    // List of objects inside the scene
    QVector<SceneObject*> objects;

    // Framebuffer for texture rendering
    QOpenGLFramebufferObject framebuffer;

    // Result of rendering
    QImage image;

    // Use QTime for underlying time management
    QTime time;
    int elapsedTime;
};

#endif /* end of include guard: SCENE_H */
