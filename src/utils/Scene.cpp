#include <Scene.h>

Scene::Scene(QString n, int w, int h) 
    : SceneObject(n), 
      name(n), width(w), height(h),
      camera(n + QString("'s camera'")),
      framebuffer(width, height) {

    camera.setPerspective(60.0, (float)width/(float)height, 0.01, 1000.0);

    camera.lookAt(
            QVector3D(0.0, 0.0, -5.0),
            QVector3D(0.0, 0.0, 0.0),
            QVector3D(0.0, 1.0, 0.0)
    );
}


Scene::~Scene() {

}

void Scene::addObject(SceneObject* object) {
    objects.append(object);
    object->initialize();
}

QImage Scene::renderScene() {
    
    // use this framebuffer instead of the default buffer
    if (!framebuffer.bind()) {
        qDebug() << "Failed to bind framebuffer";
        exit(-1);
    }

    // Change the viewport to the whole screen
    glViewport(0, 0, width, height);

    // Clear out buffer before drawing anything
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // camera.moveForward(0.1);
    camera.moveBackward(0.1);

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

        // Uniform the camera matrix
        camera.uniformMatrices(object->program);
        // Uniform the object variables
        object->program.setUniformValue("uTransform", object->transform);

        // User defined uniform variables
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

// Time control functions for the scene
void Scene::start() { 
    time.start();
    elapsedTime = 0;
}

void Scene::pause() { 
    elapsedTime = time.elapsed(); 
}

void Scene::resume() {
    // subtracting the time just before we pause it
    // go back in time to fake pause/resume
    int diff = time.elapsed() - elapsedTime;
    time.addMSecs(-elapsedTime);
}

void Scene::restart() {
    time.restart();
}

double Scene::timeElapsed() const
{ 
    return time.elapsed() / 1000.0; 
}
