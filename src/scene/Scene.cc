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
    
    {
        // Create my own framebuffer for two pass rendering
        QOpenGLFramebufferObjectFormat fmt;
        fmt.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
        fbo_.reset(new QOpenGLFramebufferObject(QSize(w, h),fmt));
    }

    camera.setPerspective(45.0, (float)width/(float)height, 0.01, 5000.0);

    // camera.lookAt(
    //         QVector3D(0.0, 750.0, 0.0),
    //         QVector3D(0.0, 750.0, 20.0),
    //         QVector3D(0.0, 1.0, 0.0)
    // );

    camera.lookAt(
            QVector3D(0.0, 150.0, 0.0),
            QVector3D(0.0, 150.0, 20.0),
            QVector3D(0.0, 1.0, 0.0)
    );
    // camera.moveForward(50);
    // camera.turnLeft(20);

    // camera.lookAt(
    //         QVector3D(0.0, 2.0, 0.0),
    //         QVector3D(0.0, 0.0, 0.0),
    //         QVector3D(1.0, 0.0, 0.0)
    // );
    // camera.moveBackward(1000);

    // fps
    fps = fpsCounter = 0;
    start();
}


Scene::~Scene() {

}

void Scene::addObject(SceneObject* object) {
    objects.append(object);
    secondPassObjects.append(object);
    object->initialize();
}

void Scene::addSecondPassObject(SceneObject* object) {
    secondPassObjects.append(object);
    object->initialize();
}

void Scene::renderScene(QOpenGLFramebufferObject* fbo)
{
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

    // // ---------------- first pass ------------------
    // fbo_->bind();
    // CHECK_GL_ERROR("Before Clear\n");
    //
    // // Change the viewport to the whole screen
    // glViewport(0, 50, width, height);
    // // Clear out buffer before drawing anything
    // glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glClearColor(0.0, 0.0, 0.0, 0.0);
    // //glClearColor(drand48(), drand48(), drand48(), 1.0);
    // //glClearColor(sin(::time(NULL)), 0.0, 0.0, 0.0);
    // CHECK_GL_ERROR("After Clear\n");
    //
    // // camera.moveForward(100 * float(msec)/1e3);
    // // camera.turnLeft(0.1);
    //
    // // camera.moveBackward(500 * float(msec)/1e3);
    //
    // // float t =  timeElapsed() / 10.0;
    // // camera.lookAt(
    // //         QVector3D(6000 * qCos(t), 710.0, 6000 * qSin(t)),
    // //         QVector3D(0.0, 710.0, 0.0),
    // //         QVector3D(0.0, 1.0, 0.0)
    // // );
    // camera.lookAt(
    //         QVector3D(-3500, -250.0, 100.0),
    //         QVector3D(-3500, -250.0, 90.0),
    //         QVector3D(0.0, 1.0, 0.0)
    // );
    // camera.moveBackward(2000);
    //
    // // Render all objects in the scene.
    // // This could be done in a smarter way
    // for (auto iter = objects.constBegin(); iter != objects.constEnd(); ++iter) {
    //     /**
    //      * Every object in the scene would have the same
    //      * set of transformation matrix. Therefore the 
    //      * scene takes care of that.
    //      * */
    //     SceneObject *object = *iter;
    //     object->program.bind();
    //     //qDebug("Handling %p", object);
    //
    //     object->setDrawMode(drawMode);
    //     object->update();
    //
    //     // Uniform the camera matrix
    //     camera.uniformMatrices(object->program);
    //     // Uniform the object variables
    //     object->program.setUniformValue("uTransform", object->transform);
    //
    //     // // For culling terrain under water
    //     // int loc = program.uniformLocation("uUnderWaterCull");
    //     // if (loc >= 0) { object->program.setUniformValue(loc, 0.0f); }
    //
    //     // User defined uniform variables
    //     object->uniform();
    //     CHECK_GL_ERROR("Before render some Obj");
    //     object->render();
    //     CHECK_GL_ERROR("After render some Obj");
    //     object->program.release();
    //     CHECK_GL_ERROR("After release the program of some Obj");
    // }
    //
    // glFlush();
    // CHECK_GL_ERROR("After flush");
    // fbo_->release();

    // ----------------- Second Pass ----------------------
    fbo->bind();
    CHECK_GL_ERROR("Before Clear\n");

    // Change the viewport to the whole screen
    glViewport(0, 50, width, height);
    // Clear out buffer before drawing anything
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0, 1.0, 1.0, 0.0);
    //glClearColor(drand48(), drand48(), drand48(), 1.0);
    //glClearColor(sin(::time(NULL)), 0.0, 0.0, 0.0);
    CHECK_GL_ERROR("After Clear\n");


    camera.moveForward(100 * float(msec)/1e3);
    camera.turnLeft(0.1);

    // camera.lookAt(
    //         QVector3D(-3500, 250.0, 100.0),
    //         QVector3D(-3500, 250.0, 90.0),
    //         QVector3D(0.0, 1.0, 0.0)
    // );
    // camera.moveBackward(2000);
    
    // camera.moveBackward(10 * float(msec)/1e3);

    // int t = timeElapsed();
    // camera.lookAt(
    //         QVector3D(t * 10, 2.0, t * 10),
    //         QVector3D(t * 10, 0.0, t * 10),
    //         QVector3D(1.0, 0.0, 0.0)
    // );
    // camera.moveBackward(1000);

    // // ----------------- Second Pass -----------------------
    // QImage onePass = fbo_->toImage(true);
    // QOpenGLTexture *renderTarget = new QOpenGLTexture(onePass);
    // renderTarget->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    // renderTarget->setMagnificationFilter(QOpenGLTexture::Linear);

    // Render all objects in the scene.
    // This could be done in a smarter way
    for (auto iter = secondPassObjects.constBegin(); iter != secondPassObjects.constEnd(); ++iter) {
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

        // renderTarget->bind(0);
        // object->program.setUniformValue("uRenderTexture", 0);

        // // For culling terrain under water
        // int loc = program.uniformLocation("uUnderWaterCull");
        // if (loc >= 0) { object->program.setUniformValue(loc, -1000.0f); }

        // User defined uniform variables
        object->uniform();
        CHECK_GL_ERROR("Before render some Obj");
        object->render();
        CHECK_GL_ERROR("After render some Obj");
        object->program.release();
        CHECK_GL_ERROR("After release the program of some Obj");
    }
    // delete renderTarget;

    glFlush();
    CHECK_GL_ERROR("After flush");
    fbo->release();
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
