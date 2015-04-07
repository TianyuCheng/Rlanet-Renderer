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

class TerrainPatch
{
    friend class Terrain;
public:
    TerrainPatch(QVector2D pos, int level, QVector< QPair<double, double> > *ranges);
    virtual ~TerrainPatch ();

    void selectPatches(Camera &camera, QVector3D &cameraPos, QVector<TerrainPatch*> &selectedPatches);
private:
    QVector2D pos;     // the position of pos of patch
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
class Terrain : public SceneObject
{
public:
    // Must provide a scene, because it needs to update 
    // according to the move of camera
    Terrain(int grid, int levels, Scene *parent);

    virtual ~Terrain ();

    void uniform();

    void update();

    void render();

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

private:
    // Number of vertices on one side
    int grid;
    int levels;

    unique_ptr<QOpenGLTexture> decalmap[3];
    unique_ptr<QOpenGLTexture> heightmap;

    // List of distances ranges for LOD <range, morph>
    QVector< QPair<double, double> > ranges;

    // List of children patch
    QMap< QPair<int, int>, TerrainPatch*> children;

    // List of patch to render
    QVector<TerrainPatch*> selectedPatches;
};


#endif /* end of include guard: TERRAIN_H */
