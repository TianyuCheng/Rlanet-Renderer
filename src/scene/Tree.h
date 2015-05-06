#ifndef TREE_H
#define TREE_H


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

class TreeFactory;

enum TreeType { 
    PALM = 0,
    TREE1,
    NUM_TREES
};

class Tree : public Billboard
{
    friend class TreeFactory;
public:
    Tree(TreeFactory *factory, TreeType treeType);
    virtual ~Tree();

    void uniform();
    void update();

private:
    TreeFactory *factory;
    TreeType treeType;
};

/**
 * TreeFactory dummily inherits from SceneObject,
 * yet it does so only because it needs to reuse the 
 * shader methods in SceneObject
 * */
class TreeFactory : public BillboardFactory
{
    friend class Tree;
public:
    TreeFactory(Terrain *terrain);
    virtual ~TreeFactory();

    /* dummy functions that we do not use */
    void uniform() {}
    void update() {}
    void render() {} 

    Tree* createTree(TreeType treeType, QVector2D center, double radius, double spacing, double size, double height, int seed = 0);

private:
    QVector<QOpenGLTexture*> treeBlade;
    QVector<QOpenGLTexture*> treeBladeAlpha;
    Terrain *terrain;
};

#endif /* end of include guard: TREE_H */
