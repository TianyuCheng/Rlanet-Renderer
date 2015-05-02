#include <Scene.h>

Scene::Scene(QString n, int w, int h) 
    : SceneObject(n), 
      name(n), width(w), height(h),
      camera(n + QString("'s camera'"))
{
    // Initialize GL
    glEnable(GL_DEPTH_TEST);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);

    camera.setPerspective(45.0, (float)width/(float)height, 0.01, 10000.0);

    // camera.lookAt(
    //         QVector3D(0.0, 8192 + 710.0, 0.0),
    //         QVector3D(0.0, 8192 + 710.0, 20.0),
    //         QVector3D(0.0, 1.0, 0.0)
    // );

    // camera.lookAt(
    //         QVector3D(0.0, 0.0, 0.0),
    //         QVector3D(0.0, 0.0, -20.0),
    //         QVector3D(0.0, 1.0, 0.0)
    // );

    // camera.moveBackward(7000);

    // fps
    fps = fpsCounter = 0;
    start();

    float t = 0;
    camera.lookAt(
            QVector3D(6000 * qCos(t), 0.0, 6000 * qSin(t)),
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

void Scene::renderScene(QOpenGLFramebufferObject* fbo)
{
	fbo_ = fbo;
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
    glClearColor(1.0, 1.0, 1.0, 0.0);
    //glClearColor(drand48(), drand48(), drand48(), 1.0);
    //glClearColor(sin(::time(NULL)), 0.0, 0.0, 0.0);
    CHECK_GL_ERROR("After Clear\n");

    int msec = frametimer_.elapsed();
    frametimer_.restart();

    // Bookkeeping for time 
    {
        fps++;
        fpsCounter += msec;
        if (fpsCounter > 1e3) {
            // qDebug() << "FPS:" << fps;
            fpsCounter = 0;
            fps = 0;
        }
    }

    // camera.moveForward(500 * float(msec)/1e3);
    // camera.turnLeft(0.1);

    // camera.moveBackward(500 * float(msec)/1e3);
    

#if 0
    float t =  timeElapsed() / 10.0;
    camera.lookAt(
            QVector3D(6000 * qCos(t), 0.0, 6000 * qSin(t)),
            QVector3D(0.0, 0.0, 0.0),
            QVector3D(0.0, 1.0, 0.0)
    );
#endif

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
    // camera.setAspect((GLfloat)width / (GLfloat)height);
}

void Scene::first_frame()
{
	frametimer_.start();
}
