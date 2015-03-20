#include "Scene.h"

Scene::Scene(QString n, int width, int height) 
    : name(n), framebuffer(width, height) {
    // Initialize the matrices
    uMVMatrix.setToIdentity();
    uPMatrix.setToIdentity();
    uNMatrix.setToIdentity();

    uPMatrix.perspective(45.0, (float)width/(float)height, 1.0, 500.0);
}


Scene::~Scene() {

}

void Scene::addObject(SceneObject* object) {
    objects.append(object);
    object->initialize();
}


void Scene::uniformMatrices(QGLShaderProgram &program) {
    // uniform modelview matrix
    program.setUniformValue("uMVMatrix", uMVMatrix);
    // uniform project matrix
    program.setUniformValue("uPMatrix", uPMatrix);
    // uniform normal matrix
    program.setUniformValue("uNMatrix", uNMatrix);
}

QImage Scene::render() {
    
    // use this framebuffer instead of the default buffer
    if (!framebuffer.bind()) {
        qDebug() << "Failed to bind framebuffer";
        exit(-1);
    }

    // Clear out buffer before drawing anything
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    uMVMatrix.setToIdentity();
    uMVMatrix.translate(0.0, 0.0, -20.0);
    // uMVMatrix.rotate(90, 1.0, 0.0, 0.0);
    uMVMatrix.rotate(time.elapsed() * 0.005, 0.0, 1.0, 0.0);

    // Render all objects in the scene.
    // This could be done in a smarter way
    for (auto iter = objects.constBegin(); iter != objects.constEnd(); ++iter) {
        /**
         * Every object in the scene would have the same
         * set of transformation matrix. Therefore the 
         * scene takes care of that.
         * */
        SceneObject *object = *iter;
        object->program.bind();

        object->update();

        uniformMatrices(object->program);
        object->uniform();
        object->render();

        object->program.release();
    }
    
    // Render the buffer to image
    image = framebuffer.toImage();

    // use default buffer instead of this buffer
    if (!framebuffer.release()) {
        qDebug() << "Failed to release framebuffer";
    }

    return image;
}
