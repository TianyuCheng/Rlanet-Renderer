#include <Scene.h>

Scene::Scene(QString n, QSize r) 
    : SceneObject(n), 
      name(n), resolution(r), camera(nullptr), 
      fbo_ready(false), lastPass(-1)
{
}


Scene::~Scene() {

}

void Scene::init_fbo() {
    int w = resolution.width();
    int h = resolution.height();

    // Create my own framebuffer for two pass rendering
    QOpenGLFramebufferObjectFormat fmt;
    fmt.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    fbo_.reset(new QOpenGLFramebufferObject(QSize(w, h),fmt));

    // Mark the flag
    fbo_ready = true;
}

GLuint Scene::texture() const {
    Q_ASSERT_X(fbo_ready, __func__, "Error: no fbo to take from");
    return fbo_->texture();
}

GLuint Scene::takeTexture() const {
    Q_ASSERT_X(fbo_ready, __func__, "Error: no fbo to take from");
    return fbo_->takeTexture();
}

void Scene::addObject(SceneObject* object) {
    objects.append(object);
    if (!object->isInitialized()) object->initialize();
}

void Scene::renderScene(QOpenGLFramebufferObject* fbo)
{
    /**
     * If the input fbo is not null, then we render the 
     * scene into the input fbo, else we prepare its own
     * fbo and render.
     * */
    if (fbo) {
        fbo->bind();
    }
    else {
        if (!fbo_ready) init_fbo();
        fbo_->bind();
    }

    CHECK_GL_ERROR("Before Clear\n");

    // Change the viewport to the whole screen
    glViewport(0, 0, resolution.width(), resolution.height());
    // Clear out buffer before drawing anything
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    //glClearColor(drand48(), drand48(), drand48(), 1.0);
    //glClearColor(sin(::time(NULL)), 0.0, 0.0, 0.0);
    CHECK_GL_ERROR("After Clear\n");
    
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

        /* lastPass is initialized to -1,
         * it gets reseted to nonnegative when
         * assigned a texture unit
         */
        if (lastPass >= 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, lastPass);
            object->program.setUniformValue("uRenderTexture", 0);
        }

        // Uniform the camera matrix
        camera->uniformMatrices(object->program);
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

    if (fbo) fbo->release();
    else fbo_->release();
}

void Scene::resize(QSize res) {
    resolution = res;
}
