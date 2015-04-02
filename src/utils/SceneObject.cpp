#include <SceneObject.h>

SceneObject::SceneObject(QString n, SceneObject *p) : name(n), parent(p) {
    transform.setToIdentity();
    drawMode = GL_FILL;
}

SceneObject::SceneObject(QString n, QString _vShader, QString _fShader, SceneObject *p)
    : name(n), parent(p) {
    setShader(QOpenGLShader::Vertex, _vShader);
    setShader(QOpenGLShader::Fragment, _fShader);
    transform.setToIdentity();
    drawMode = GL_TRIANGLES;
}

SceneObject::SceneObject(QString n, QOpenGLShader *_vShader, QOpenGLShader *_fShader, SceneObject *p) 
    : name(n), parent(p) {
    setShader(_vShader);
    setShader(_fShader);
    transform.setToIdentity();
    drawMode = GL_TRIANGLES;
}

#if 0
SceneObject::SceneObject(SceneObject &obj) {
    name      = obj.name;
    parent    = obj.parent;
    transform = obj.transform;
    drawMode = obj.drawMode;
    setShader(obj.vShader);
    setShader(obj.fShader);
}
#endif

SceneObject::~SceneObject() {
    program.removeAllShaders();
}

// Set the vertex shader or fragment shader
void SceneObject::setShader(QOpenGLShader *shader) {
    // Check whether shader is compiled
    QString where = "SceneObject.cpp: setShader()";
    QString what  = "%1 Shader of %2 is null";
    Q_ASSERT_X(
            type == QOpenGLShader::ShaderType::Vertex || type == QOpenGLShader::ShaderType::Fragment,
            "SceneObject.cpp: setShader()", 
            QString("The type of shader for ") + name + QString(" is neither Vertex nor Fragment"));
    Q_ASSERT_X(shader->isCompiled(), "SceneObject.cpp: setShader()", 
            QString("The type of shader for ") + name + QString(" is not compiled."));

    // assign the shader
    switch (shader->shaderType()) {
        case QOpenGLShader::Vertex:
            vShader.reset(shader);
            break;
        case QOpenGLShader::Fragment:
            fShader.reset(shader);
            break;
    }

    // make sure the shader is attached and linked
    if (!program.addShader(shader)) {
        qDebug() << "add shader for object " << name << " failed!";
        exit(-1);
    }
    if (!program.link()) {
        qDebug() << "link shader for object " << name << " failed!";
        exit(-1);
    }
}

void SceneObject::setShader(QOpenGLShader::ShaderType type, QString filename) {
    // Read from file to compile the source
    QFile file(QDir::currentPath() + QDir::separator() + filename);

    qDebug() << "Load shader file: " << QDir::currentPath() + QDir::separator() + filename;

    // Check for validity of the file
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        qDebug() << "shader file " << filename << " cannot be opened!";
        exit(-1);
    }

    QTextStream in(&file);
    QString source = in.readAll();
    
    QOpenGLShader *shader = new QOpenGLShader(type);
    
    // Check whether the shader compiles
    if (!shader->compileSourceCode(source)) {
        qDebug() << QString("Shader compiles failure: ") + shader->log();
        exit(-1);
    }

    // call the other setShader to complete linking
    setShader(shader);
}

void SceneObject::initialize() {
	initializeOpenGLFunctions();
    attributes["vertex"] = program.attributeLocation("aVertex");

    program.bind();

    // Send vertex data
    GLuint vertexLocation = attributes["vertex"];
    program.enableAttributeArray(vertexLocation);                    // enable attribute array
    program.setAttributeArray(vertexLocation, vertices.constData()); // vertices attributes

    program.release();
}

void SceneObject::doubleCheck() const { 
    Q_ASSERT(program.hasOpenGLShaderPrograms());
}

void SceneObject::render() {
    /** 
     * program.bind() will be called by Scene.
     * This allows us to focus on rendering.
     * */
    glPolygonMode(GL_FRONT_AND_BACK, drawMode);
    glDrawElements(
            GL_TRIANGLES, 
            indices.size(),    // count
            GL_UNSIGNED_INT,   // type
            indices.constData());         // element array buffer offset
}

void SceneObject::loadIdentity() {
    transform.setToIdentity();
}

void SceneObject::scale(double s) {
    transform.scale(s);
}

void SceneObject::translate(QVector3D offset) {
    transform.translate(offset);
}

void SceneObject::rotate(double angle, QVector3D axis) {
    transform.rotate(angle, axis);
}

// Compute the transformation matrix with scene graph
QMatrix4x4 SceneObject::computeTransformFromRoot() {
    if (parent == nullptr) 
        return transform;
    return transform * parent->computeTransformFromRoot();
}
