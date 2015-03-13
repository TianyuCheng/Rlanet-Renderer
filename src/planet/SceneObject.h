#ifndef SCENEOBJECT_H
#define SCENEOBJECT_H

#include <QMap>
#include <QVector>
#include <QString>
#include <QGLShader>
#include <QFile>
#include <QDir>

class SceneObject
{
public:

    // Constructor with only name
    SceneObject(QString n);

    // Constructor with name and shader sources
    SceneObject(QString n, QString _vShader, QString _fShader);

    // Constructor with name and shader
    SceneObject(QString n, QGLShader *_vShader, QGLShader *_fShader);

    // Copy Constructor
    SceneObject(SceneObject &obj);

    virtual ~SceneObject();

    // Set the vertex shader or fragment shader
    void setShader(QGLShader *shader);
    void setShader(QGLShader::ShaderType type, QString shaderFile);

    /**
     * Wrapper for the real render function.
     * It performs shader programs switching
     * */ 
    void render();


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
    QMap<QString, int> attibutes;
    QMap<QString, int> uniforms;
    QVector<float> vertices;
    QVector<int> indices;

private:
    // name of the objects, for debugging and display
    QString name;

    QGLShaderProgram program;
    // shader-based rendering
    QGLShader *vShader;
    QGLShader *fShader;
};

#endif /* end of include guard: SCENEOBJECT_H */
