#include <SceneObject.h>

SceneObject::SceneObject(QString n, SceneObject *p) : name(n), parent(p) {
	initializeOpenGLFunctions();
	transform.setToIdentity();
	drawMode = GL_FILL;
}

SceneObject::SceneObject(QString n, QString _vShader, QString _fShader, SceneObject *p)
    : name(n), parent(p) {
	initializeOpenGLFunctions();
	setShader(QOpenGLShader::Vertex, _vShader);
	setShader(QOpenGLShader::Fragment, _fShader);
	transform.setToIdentity();
	drawMode = GL_TRIANGLES;
}

SceneObject::SceneObject(QString n, QOpenGLShader *_vShader, QOpenGLShader *_fShader, SceneObject *p) 
    : name(n), parent(p) {
	initializeOpenGLFunctions();
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
	vbo_.create();
	vbo_.bind();
	int size = vertices.size() * sizeof(QVector3D);
	vbo_.allocate(size);
	vbo_.write(0, vertices.constData(), size);

	ibo_.create();
	ibo_.bind();
	size = indices.size() * sizeof(unsigned int);
	ibo_.allocate(size);
	ibo_.write(0, indices.constData(), size);
	ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	REPORT_GL_STATUS("Create GL Buffers");

	glBindFragDataLocation(program.programId(), 0, "frag_color");
	vertexLocation_ = program.attributeLocation("aVertex");
	attributes["vertex"] = vertexLocation_;
	//qDebug("Vertex attribute location %d\n", attributes["vertex"]);

#if 0 // These should be done in render, not here.
	program.bind();
	REPORT_GL_STATUS("bind program");

	// Send vertex data
	vertexLocation = attributes["vertex"];
	program.enableAttributeArray(vertexLocation);                    // enable attribute array
	REPORT_GL_STATUS("enable Attribute Array");
	program.setAttributeArray(vertexLocation, vertices.constData()); // vertices attributes
	REPORT_GL_STATUS("set Attribute Array");

	program.release();
#endif
	ibo_.release();
	vbo_.release();

	qDebug("VBO: %d, IBO %d", vbo_.bufferId(), ibo_.bufferId());
}

void SceneObject::doubleCheck() const { 
    Q_ASSERT(program.hasOpenGLShaderPrograms());
}

/** 
 * rendering goes here,
 * called by Scene
 *
 * TODO: unify rendering procedure, which allows us use fewer QOpenGLContext
 * object.
 */
void SceneObject::render() {
#if 0 // Old stuff
	//CHECK_GL_ERROR("Before render");
	glPolygonMode(GL_FRONT_AND_BACK, drawMode);
	glDrawElements(
			GL_TRIANGLES, 
			indices.size(),    // count
			GL_UNSIGNED_INT,   // type
			indices.constData());         // element array buffer offset
	//CHECK_GL_ERROR("After render");
#endif
	// Assume program has been binded
	// program.bind();
	// Feed data from VBO
	vbo_.bind();
	program.setAttributeBuffer(vertexLocation_, GL_FLOAT, 0, 3);
	program.enableAttributeArray(vertexLocation_);
	ibo_.bind();
	CHECK_GL_ERROR("After buffer");
	glDrawElements(
			GL_TRIANGLES, 
			indices.size(),		// count
			GL_UNSIGNED_INT,	// type
			NULL);			// Does not matter with ibo
	CHECK_GL_ERROR("After render");
	vbo_.release();
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
