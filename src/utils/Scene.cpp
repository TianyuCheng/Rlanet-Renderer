#include <Scene.h>

Scene::Scene(QString n, int w, int h) 
    : SceneObject(n), 
      name(n), width(w), height(h),
      camera(n + QString("'s camera'")),
      framebuffer(width, height, QOpenGLFramebufferObject::Depth)
{
    // Initialize GL
    glEnable(GL_DEPTH_TEST);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    camera.setPerspective(60.0, (float)width/(float)height, 0.01, 5000.0);

    // camera.lookAt(
    //         QVector3D(0.0, 2000.0, 0.0),
    //         QVector3D(0.0, 0.0, 0.0),
    //         QVector3D(0.0, 0.0, 1.0)
    // );

    camera.lookAt(
            QVector3D(0.0, 500.0, 0.0),
            QVector3D(0.0, 500.0, 120.0),
            QVector3D(0.0, 1.0, 0.0)
    );
}


Scene::~Scene() {

}

void Scene::addObject(SceneObject* object) {
    objects.append(object);
    object->initialize();
}

void Scene::renderScene()
{
#if 0 // We don't need this in quick version
    // use this framebuffer instead of the default buffer
    if (!framebuffer.bind()) {
        qDebug() << "Failed to bind framebuffer";
        exit(-1);
    }
#endif
    CHECK_GL_ERROR("Before Clear\n");

    // Change the viewport to the whole screen
    glViewport(0, 50, width, height);
    // Clear out buffer before drawing anything
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glClearColor(drand48(), drand48(), drand48(), 1.0);
    //glClearColor(sin(::time(NULL)), 0.0, 0.0, 0.0);
    CHECK_GL_ERROR("After Clear\n");

    camera.moveForward(10);
    // camera.moveForward(0.1);
    // camera.moveBackward(0.1);
    camera.turnLeft(1);
    // camera.turnRight(1);

    // camera.lookAt(
    //         QVector3D(0.0, 2000.0, 100 * timeElapsed()),
    //         QVector3D(0.0, 0.0,    100 * timeElapsed()),
    //         QVector3D(0.0, 0.0, 1.0)
    // );

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
        //qDebug("Handling %p", object);

        object->setDrawMode(drawMode);
        object->update();

        // Uniform the camera matrix
        camera.uniformMatrices(object->program);
        // Uniform the object variables
        object->program.setUniformValue("uTransform", object->transform);

        // User defined uniform variables
        object->uniform();
	CHECK_GL_ERROR("Before render some Obj");

        object->render();

	CHECK_GL_ERROR("After render some Obj");

        object->program.release();
	CHECK_GL_ERROR("After release the program of some Obj");
    }
    glFlush();
    CHECK_GL_ERROR("After flush");
    
#if 0
    // Render the buffer to image
    image = framebuffer.toImage();

    // use default buffer instead of this buffer
    if (!framebuffer.release()) {
        qDebug() << "Failed to release framebuffer";
    }
#endif
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
    time.addMSecs(-diff);
}

void Scene::restart() {
    time.restart();
}

double Scene::timeElapsed() const
{ 
    return time.elapsed() / 1000.0; 
}

void Scene::resize(GLuint width, GLuint height) {
    camera.setAspect((GLfloat)width / (GLfloat)height);
}
