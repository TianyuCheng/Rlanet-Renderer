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

class Grass : public SceneObject
{
public:
    Grass(int seed);
    virtual ~Grass();

    void uniform();
    void update();
    // void render();

private:
    void generateBillboard();
};

class GrassFactory
{
    friend class Grass;
public:
    GrassFactory(int numVariations);
    virtual ~GrassFactory();

    Grass* getGrass(int index);

private:

    // prepare the structure of grass blades
    void prepareGrassBlades();

    int numVariations;

    std::unique_ptr<QOpenGLTexture> grassBlade;

    QVector<Grass*> blades;
};

#endif /* end of include guard: GRASS_H */
