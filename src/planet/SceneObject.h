#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <QMap>
#include <QVector>
#include <QString>
#include <QFile>
#include <QDir>

#include <QVector3D>
#include <QOpenGLShader>
#include <QOpenGLFunctions>

class Scene;

class SceneObject
{
    friend class Scene;
public:

    // Constructor with only name
    SceneObject(QString n);

    // Constructor with name and shader sources
    SceneObject(QString n, QString _vShader, QString _fShader);

    // Constructor with name and shader
    SceneObject(QString n, QOpenGLShader *_vShader, QOpenGLShader *_fShader);

    // Copy Constructor
    SceneObject(SceneObject &obj);

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

    // setter and getter
    void setName(QString n) { name = n; }
    QString getName() const { return name; }

protected:
    // rendering related variables
    // vertex array, index array, locations, etc.
    QMap<QString, GLuint> attributes;
    QMap<QString, GLuint> uniforms;
    QVector<QVector3D> vertices;
    QVector<unsigned int> indices;

    // used for user customization
    QOpenGLShaderProgram program;

private:
    // name of the objects, for debugging and display
    QString name;

    // shader-based rendering
    QOpenGLShader *vShader;
    QOpenGLShader *fShader;
};

#endif /* end of include guard: SCENEOBJECT_H */
