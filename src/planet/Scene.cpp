#include "Scene.h"

Scene::Scene(QString n, int width, int height) 
    : name(n), framebuffer(width, height) {
    // TODO: Create a framebuffer here
}


Scene::~Scene() {

}

void Scene::addObject(SceneObject* object) {
    objects.append(object);
}

void Scene::render() {
    
    if (!framebuffer.bind()) {
        qDebug() << "Failed to bind framebuffer";
        exit(-1);
    }

    for (auto iter = objects.constBegin(); iter != objects.constEnd(); ++iter) {
        (*iter)->render();
    }

    if (!framebuffer.release()) {
        qDebug() << "Failed to release framebuffer";
    }
}
