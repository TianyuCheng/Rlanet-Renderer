#ifndef GRASS_H
#define GRASS_H


#include <cmath>
#include <ctime>
#include <cstdlib>
#include <memory>

#include <QVector>
#include <QImage>
#include <QVector2D>
#include <QVector3D>
#include <QQuaternion>
#include <QOpenGLTexture>
#include <QOpenGLShader>

#include <SceneObject.h>

class GrassFactory;

class Grass : public SceneObject
{
    friend class GrassFactory;
public:
    Grass(GrassFactory *factory);
    virtual ~Grass();

    void setSize(double s) { size = s; }
    void plantGrass(QVector2D coordinate, double height);

    void uniform();
    void update();
    void render();

private:
    GrassFactory *factory;
    double size;
};

/**
 * GrassFactory dummily inherits from SceneObject,
 * yet it does so only because it needs to reuse the 
 * shader methods in SceneObject
 * */
class GrassFactory : private SceneObject
{
    friend class Grass;
public:
    GrassFactory();
    virtual ~GrassFactory();

    /* dummy functions that we do not use */
    void uniform() {}
    void update() {}
    void render() {} 

    Grass* createGrass(QVector2D center, double radius, double spacing, double size, double height, int seed = 0);

private:
    std::unique_ptr<QOpenGLTexture> grassBlade;
    std::unique_ptr<QOpenGLTexture> grassBladeAlpha;
};

#endif /* end of include guard: GRASS_H */
