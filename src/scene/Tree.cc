#include "Tree.h"

Tree::Tree(TreeFactory *f)
    : Billboard(f), factory(f)
{
    setShader(f->vShader.get());
    setShader(f->gShader.get());
    setShader(f->fShader.get());
}

Tree::~Tree() {

}

void Tree::uniform() {
    // uniform height map
    factory->terrain->bindHeightmap(program, "uHeightmap", 1);
	CHECK_GL_ERROR("After uniform");

    // uniform the tree texture and alpha map
    factory->treeBlade->bind(2);
    factory->treeBladeAlpha->bind(3);
    program.setUniformValue("uDecalmap", 2);
    program.setUniformValue("uAlphamap", 3);
	CHECK_GL_ERROR("After uniform");

    // uniform the size of tree
    program.setUniformValue("uSize", float(size));
	CHECK_GL_ERROR("After uniform");
}

void Tree::update() {
}

TreeFactory::TreeFactory(Terrain *t) 
    : BillboardFactory("Tree Factory"), terrain(t) {

    QImage tree("../textures/billboard/tree.jpg");
    QImage treeAlpha("../textures/billboard/tree_alpha.jpg");

    if (tree.isNull() || treeAlpha.isNull()) {
        qDebug() << "Tree texture has not been found!";
        exit(-1);
    }

    setShader(QOpenGLShader::Vertex, "../glsl/billboard.vert");
    setShader(QOpenGLShader::Geometry, "../glsl/tree.geom");
    setShader(QOpenGLShader::Fragment, "../glsl/billboard.frag");

    treeBlade.reset(new QOpenGLTexture(tree));
    treeBlade->setWrapMode(QOpenGLTexture::Repeat);
    treeBlade->setMinificationFilter(QOpenGLTexture::Linear);
    treeBlade->setMagnificationFilter(QOpenGLTexture::Linear);
    treeBlade->setWrapMode(QOpenGLTexture::Repeat);

    treeBladeAlpha.reset(new QOpenGLTexture(treeAlpha));
    treeBladeAlpha->setWrapMode(QOpenGLTexture::Repeat);
    treeBladeAlpha->setMinificationFilter(QOpenGLTexture::Linear);
    treeBladeAlpha->setMagnificationFilter(QOpenGLTexture::Linear);
    treeBladeAlpha->setWrapMode(QOpenGLTexture::Repeat);
}

TreeFactory::~TreeFactory() {
    treeBlade.reset();
    treeBladeAlpha.reset();
}

Tree* TreeFactory::createTree(QVector2D center, double radius, double spacing, double size, double height, int seed) {
    Tree* tree = new Tree(this);
    createBillboard(tree, center, radius, spacing, size, height, seed);
    return tree;
}
