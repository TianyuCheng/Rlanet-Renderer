#ifndef SCENE_H
#define SCENE_H

#include <memory>
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
    Scene(QString name, int width, int height);
    virtual ~Scene();

    /**
     * Add object into the scene
     * */
    void addObject(SceneObject* object);
    void addSecondPassObject(SceneObject* object);

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
    void first_frame();
private:
    // Name for debugging and displaying
    QString name;
    int width, height;

    // Camera object encapsulation
    Camera camera;

    // List of objects inside the scene
    QVector<SceneObject*> objects;
    QVector<SceneObject*> secondPassObjects;

    // Framebuffer for texture rendering
    unique_ptr<QOpenGLFramebufferObject> fbo_;
    // QImage onePass;

    // Use QTime for underlying time management
    QTime time;
    QTime frametimer_;
    int elapsedTime;

    int fps;        // fps counter
    int fpsCounter;
};

#endif /* end of include guard: SCENE_H */
