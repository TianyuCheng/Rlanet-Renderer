#ifndef TERRAIN_H
#define TERRAIN_H

#include <QMap>
#include <QPair>
#include <QtMath>
#include <QString>
#include <QVector>
#include <QVector2D>
#include <QVector3D>

#include <QOpenGLTexture>

#include <SceneObject.h>
#include <Scene.h>
#include <BoundingBox.h>
#include <NoiseGenerator.h>

using std::unique_ptr;

class Terrain;
class HTileSys;

class TerrainPatch {
	friend class Terrain;
public:
	TerrainPatch(QVector2D pos, int level, QVector< QPair<double, double> > *ranges);
	virtual ~TerrainPatch ();

	void selectPatches(Camera &camera, QVector3D &cameraPos, QVector<TerrainPatch*> &selectedPatches);

protected:
	void initializeChildren();

protected:
	QVector2D pos;		// the position of pos of patch
	int level;
	BoundingBox bounds;

	// Quad-Tree like structure
	/**
	 * Indexing by quadrant
	 * +---------+---------+
	 * |         |         |
	 * |   p2    |    p1   |
	 * |         |         |
	 * +---------+---------+
	 * |         |         |
	 * |   p3    |    p4   |
	 * |         |         |
	 * +---------+---------+
	 * */
	TerrainPatch *children[4];
	QVector< QPair<double, double> > ranges;
};


/**
 *  Terrain class is used for infinite terrain generation and management.
 *  It is designed to be implemented with CDLOD (Continuous Distance-Dependent Level-Of-Detail).
 *  Terrain class also encapsulates the rendering of all the patches of terrain.
 **/
class Terrain : public SceneObject {
public:
	// Must provide a scene, because it needs to update 
	// according to the move of camera
	Terrain(GraphicsDevice *device, int grid, int levels, Scene *parent);

	virtual ~Terrain ();

	void uniform();

	void update();

	void render();

	void initialize();

	void bindHeightmap(QOpenGLShaderProgram &program, QString name, GLuint textureUnit = 1) {
		heightmap->bind(textureUnit);
		int heightLocation = program.uniformLocation(name);
		program.setUniformValue(heightLocation, textureUnit);
	}

	void disableUnderWaterCulling() {
		underWaterCull = false;
	}

	void underWaterCulling(QVector4D plane) {
		underWaterCull = true;
		waterPlane = plane;
	}

private:

	/**
	 * updatePatches() checks whether it needs 
	 * new patches of terrain. If camera moves
	 * to some new region, it will check the 
	 * patches within * the square of length 
	 * = 2 * zFar has been generated. If not,
	 * generate the patch.
	 * */
	void updatePatches();

	/**
	 * select patches to render from all patches.
	 * This is a recursive call into TerrainPatch.
	 * Selected patches will be put into selectedPatches.
	 * */
	void selectPatches(Camera &camera, QVector3D &cameraPos);

	// Debugging ranges and morph area info
	void rangesInfo();
	void allPatchesInfo();

protected:
	// Number of vertices on one side
	int grid;
	int levels;
	int size;

	bool underWaterCull;
	QVector4D waterPlane;

	unique_ptr<QOpenGLTexture> decalmap[3];
	unique_ptr<QOpenGLTexture> heightmap;
	unique_ptr<QOpenGLTexture> noisemap;
	unique_ptr<QOpenGLTexture> waterCaustics;

	// List of distances ranges for LOD <range, morph>
	QVector< QPair<double, double> > ranges;

	// List of children patch
	QMap< QPair<int, int>, TerrainPatch*> children;

	// List of patch to render
	QVector<TerrainPatch*> selectedPatches;

	QTime time;

	// The Earth
	unique_ptr<HTileSys> hsys_;
protected:
	virtual void init_gl_resource();
	virtual void init_vert();
};


#endif /* end of include guard: TERRAIN_H */
