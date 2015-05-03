#ifndef GRASS_H
#define GRASS_H


#include <cmath>
#include <ctime>
#include <cstdlib>
#include <memory>

#include <QVector>
#include <QImage>
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
    Grass(GrassFactory *factory, int seed);
    virtual ~Grass();

    void uniform();
    void update();
    void render();

private:
    void generateBillboard();

private:
    GrassFactory *factory;

    QVector3D offset;
    QVector3D scale;
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
    GrassFactory(int numVariations);
    virtual ~GrassFactory();

    Grass* getGrass(int index);

    void uniform() {}
    void update() {}
    void render() {} 

private:

    // prepare the structure of grass blades
    void prepareGrassBlades();

    int numVariations;

    std::unique_ptr<QOpenGLTexture> grassBlade;
    std::unique_ptr<QOpenGLTexture> grassBladeAlpha;

    QVector<Grass*> blades;
};

#endif /* end of include guard: GRASS_H */
