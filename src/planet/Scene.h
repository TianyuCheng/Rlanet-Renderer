#ifndef SCENE_H
#define SCENE_H

#include "SceneObject.h"
#include <QVector>

class Scene
{
public:
    Scene() {
    }

    virtual ~Scene() {
    }

    void addObject(SceneObject* object) {
        objects.append(object);
    }

    void render() {
        for (auto iter = objects.constBegin(); iter != objects.constEnd(); ++iter) {
            (*iter)->render();
        }
    }

private:
    QVector<SceneObject*> objects;
};

#endif /* end of include guard: SCENE_H */
