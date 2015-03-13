#include "SceneObject.h"

SceneObject::SceneObject(QString n) : name(n) {
    vShader = nullptr;
    fShader = nullptr;
}

SceneObject::SceneObject(QString n, QString _vShader, QString _fShader)
    : name(n) {
    vShader = nullptr;
    fShader = nullptr;
    setShader(QGLShader::Vertex, _vShader);
    setShader(QGLShader::Fragment, _fShader);
}

SceneObject::SceneObject(QString n, QGLShader *_vShader, QGLShader *_fShader) 
    : name(n) {
    vShader = nullptr;
    fShader = nullptr;
    setShader(_vShader);
    setShader(_fShader);
}

SceneObject::SceneObject(SceneObject &obj) {
    vShader = nullptr;
    fShader = nullptr;
    name    = obj.name;
    setShader(obj.vShader);
    setShader(obj.fShader);
}

SceneObject::~SceneObject() {
    if (vShader) delete vShader;
    if (fShader) delete fShader;
    program.removeAllShaders();
}

// Set the vertex shader or fragment shader
void SceneObject::setShader(QGLShader *shader) {
    // Check whether shader is compiled
    QString where = "SceneObject.cpp: setShader()";
    QString what  = "%1 Shader of %2 is null";
    Q_ASSERT_X(
            type == QGLShader::ShaderType::Vertex || type == QGLShader::ShaderType::Fragment,
            "SceneObject.cpp: setShader()", 
            QString("The type of shader for ") + name + QString(" is neither Vertex nor Fragment"));
    Q_ASSERT_X(shader->isCompiled(), "SceneObject.cpp: setShader()", 
            QString("The type of shader for ") + name + QString(" is not compiled."));

    // assign the shader
    switch (shader->shaderType()) {
        case QGLShader::Vertex:
            if (vShader) delete vShader;
            vShader = shader;
            break;
        case QGLShader::Fragment:
            if (fShader) delete fShader;
            fShader = shader;
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

void SceneObject::setShader(QGLShader::ShaderType type, QString filename) {
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
    
    QGLShader *shader = new QGLShader(type);
    
    // Check whether the shader compiles
    if (!shader->compileSourceCode(source)) {
        qDebug() << QString("Shader compiles failure: ") + shader->log();
        exit(-1);
    }

    // call the other setShader to complete linking
    setShader(shader);
}

void SceneObject::initialize() {
    attributes["vertex"] = program.attributeLocation("vertex");
    // uniforms["matrix"] = program.uniformLocation("matrix");
    // uniforms["color"] = program.uniformLocation("color");
}

void SceneObject::doubleCheck() const { 
    Q_ASSERT(program.hasOpenGLShaderPrograms());
}

void SceneObject::render() {
    int vertexLocation = attributes["vertex"];
    program.bind();                                         // glUseProgram(program.id)
    program.enableAttributeArray(vertexLocation);           // enable attribute array
    // program.setAttributeArray(vertexLocation, vertices, 3); // vertices attributes
    uniform();

    // Draw the triangles !
    glDrawElements(
            GL_TRIANGLES,      // mode
            indices.size(),    // count
            GL_UNSIGNED_INT,   // type
            (void*)0);         // element array buffer offset

    program.disableAttributeArray(vertexLocation);

    program.release();                                       // glUseProgram(0)
}
