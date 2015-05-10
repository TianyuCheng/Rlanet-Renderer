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
#include <Terrain.h>
#include <Billboard.h>

class GrassFactory;

class Grass : public Billboard
{
	friend class GrassFactory;
public:
	Grass(GrassFactory *factory);
	virtual ~Grass();

	void uniform();
	void update();

private:
	GrassFactory *factory;
};

/**
 * GrassFactory dummily inherits from SceneObject,
 * yet it does so only because it needs to reuse the 
 * shader methods in SceneObject
 * */
class GrassFactory : public BillboardFactory
{
	friend class Grass;
public:
	GrassFactory(Terrain *terrain);
	virtual ~GrassFactory();

	/* void render */
	void render() {} 

	Grass* createGrass(QVector2D center, double radius, double spacing, double size, double height, int seed = 0);

private:
	std::unique_ptr<QOpenGLTexture> grassBlade;
	std::unique_ptr<QOpenGLTexture> grassBladeAlpha;
	Terrain *terrain;
};

#endif /* end of include guard: GRASS_H */
