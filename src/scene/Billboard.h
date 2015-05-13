#ifndef BILLBOARD_H
#define BILLBOARD_H

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

class BillboardFactory;

class Billboard : public SceneObject {
public:
	Billboard(BillboardFactory *factory);
	virtual ~Billboard();

	void setSize(double s) { size = s; }
	void placeBillboard(QVector2D coordinate, double height);

	void render();

protected:
	double size;
};

/**
 * BillboardFactory dummily inherits from SceneObject,
 * yet it does so only because it needs to reuse the 
 * shader methods in SceneObject
 * */
class BillboardFactory : protected SceneObject {
	friend class Billboard;
public:
	BillboardFactory(QString name);
	virtual ~BillboardFactory();

	/* void the render procedure */
	void render() {} 

	void createBillboard(Billboard *board,
			QVector2D center,
			double radius,
			double spacing,
			double size,
			double height,
			int seed = 0,
			double chance = 1.0
			);
};

#endif /* end of include guard: BILLBOARD_H */
