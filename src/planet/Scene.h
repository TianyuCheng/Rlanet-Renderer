#ifndef SCENE_H
#define SCENE_H

#include <QVector>
#include <QOpenGLFramebufferObject>

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
    void render();

private:
    // Name for debugging and displaying
    QString name;

    // list of objects inside the scene
    QVector<SceneObject*> objects;

    // framebuffer for texture rendering
    QOpenGLFramebufferObject framebuffer;
};

#endif /* end of include guard: SCENE_H */
