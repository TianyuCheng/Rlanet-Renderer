#include <Scene.h>

Scene::Scene(QString n, QSize r) 
    : SceneObject(n), 
      name(n), resolution(r), camera(nullptr)
{
}


Scene::~Scene() {

}

void Scene::addObject(SceneObject* object) {
    objects.append(object);
    object->initialize();
}

void Scene::renderScene(QOpenGLFramebufferObject* fbo)
{
    fbo->bind();
    CHECK_GL_ERROR("Before Clear\n");

    // Change the viewport to the whole screen
    glViewport(0, 50, resolution.width(), resolution.height());
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
    fbo->release();
}

void Scene::resize(QSize res) {
    resolution = res;
}
