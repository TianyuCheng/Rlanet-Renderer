#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <memory>
using std::unique_ptr;
#include <QMap>
#include <QtMath>
#include <QString>
#include <QVector>
#include <QFile>
#include <QDir>

#include <QVector3D>
#include <QOpenGLShader>
#include <QOpenGLBuffer>
#include "SelectGL.h"

class Scene;

class SceneObject : protected GLCore
{
    friend class Scene;
public:

    // Constructor with only name
    SceneObject(QString n, SceneObject *parent = nullptr);

    // Constructor with name and shader sources
    SceneObject(QString n, QString _vShader, QString _fShader, SceneObject *parent = nullptr);

    // Constructor with name and shader
    SceneObject(QString n, QOpenGLShader *_vShader, QOpenGLShader *_fShader, SceneObject *parent = nullptr);

#if 0
    // Copy Constructor
    // DON'T, copy will cause serious problems right now.
    SceneObject(SceneObject &obj);
#endif

    virtual ~SceneObject();

    // Set the vertex shader or fragment shader
    void setShader(QOpenGLShader *shader);
    void setShader(QOpenGLShader::ShaderType type, QString shaderFile);

    /**
     * Initialization of locations
     * */
    virtual void initialize();

    /**
     * Wrapper for the real render function.
     * It performs shader programs switching.
     * One could overwrite it, but it is not
     * encouraged.
     * */ 
    virtual void render();

    /**
     * Update all kinds of information, 
     * include vertex positions, material,
     * colors, etc.
     * */
    virtual void update() = 0;

    /**
     * Uniforms the variables to GPU.
     * This is a pure virtual method, 
     * because different shader program 
     * usually require different uniform
     * variables.
     * */
    virtual void uniform() = 0;

    /**
     * Double check before rendering.
     * Every SceneObject requires a vertex shader
     * and a fragment shader during rendering.
     * */
    void doubleCheck() const;

    /**
     * Object transformation manipulation
     * translate, scale, rotate
     * */
    void loadIdentity();
    void scale(double s);
    void translate(QVector3D offset);
    void rotate(double angle, QVector3D axis);

    // Compute the transformation matrix with scene graph
    QMatrix4x4 computeTransformFromRoot();

    // setter and getter
    void setName(QString n) { name = n; }
    QString getName() const { return name; }

    // Render with GL_TRIANGLES, GL_LINES, GL_QUADS, GL_TRIANGLE_STRIPS, etc
    void setDrawMode(GLenum mode) { drawMode = mode; }

protected:
    // rendering related variables
    // vertex array, index array, locations, etc.
    QMap<QString, GLuint> attributes;
    QMap<QString, GLuint> uniforms;
    QVector<QVector3D> vertices;
    QVector<unsigned int> indices;

    // used for user customization
    QOpenGLShaderProgram program;

    QMatrix4x4 transform;
    SceneObject *parent;

    // GL_FILL, GL_LINE, etc
    GLenum drawMode;
private:
    // name of the objects, for debugging and display
    QString name;

protected:
    // shader-based rendering
	unique_ptr<QOpenGLShader> vShader;
	unique_ptr<QOpenGLShader> fShader;
	QOpenGLBuffer vbo_ = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
	QOpenGLBuffer ibo_ = QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	GLuint vertexLocation_;
};

#endif /* end of include guard: SCENEOBJECT_H */
