#ifndef GRASS_H
#define GRASS_H

#include <QVector>
#include <QImage>
#include <QOpenGLTexture>
#include <memory>

#include <Scene.h>

class GrassBlade
{
public:
    GrassBlade();
    virtual ~GrassBlade();

private:
    /* data */
};

class Grass : public SceneObject
{
public:
    Grass();
    virtual ~Grass();

    // prepare the structure of grass blades
    void prepareGrassBlades();

private:
    int numGrassVariations;

    std::unique_ptr<QOpenGLTexture> grassBlade;
    // std::unique_ptr<QOpenGLTexture> grassAlpha;

    QVector<GrassBlade*> blades;
};

#endif /* end of include guard: GRASS_H */
