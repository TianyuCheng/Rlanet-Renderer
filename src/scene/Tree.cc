#include "Tree.h"

Tree::Tree(TreeFactory *f, TreeType t)
    : Billboard(f), factory(f), treeType(t)
{
    setShader(f->vShader.get());
    setShader(f->gShader.get());
    setShader(f->fShader.get());
}

Tree::~Tree() {

}

void Tree::uniform() {
	// uniform height map
	CHECK_GL_ERROR(factory->device, "Tree: before bind uHeightmap");
	factory->terrain->bindHeightmap(program, "uHeightmap", 1);
	CHECK_GL_ERROR(factory->device, "Tree: After uniform uHeightmap");

	// uniform the tree texture and alpha map
	factory->treeBlade[int(treeType)]->bind(2);
	program.setUniformValue("uDecalmap", 2);
	CHECK_GL_ERROR(factory->device, "Tree After uAlphamap");

	// uniform the size of tree
	program.setUniformValue("uSize", float(size));
	CHECK_GL_ERROR(factory->device, "Tree: After uSize");
}

void Tree::update() {
}

TreeFactory::TreeFactory(GraphicsDevice *device, Terrain *t) 
    : BillboardFactory(device, "Tree Factory"), terrain(t) {

    setShader(QOpenGLShader::Vertex, "../glsl/billboard.vert");
    setShader(QOpenGLShader::Geometry, "../glsl/tree.geom");
    setShader(QOpenGLShader::Fragment, "../glsl/billboard.frag");

    for (int i = 0; i < int(TreeType::NUM_TREES); i++) {
        QString filename = QString("../textures/billboard/tree%1.png");

        QImage tree(filename.arg(QString::number(i + 1)));

        if (tree.isNull()) {
            qDebug() << "Tree texture has not been found!";
            exit(-1);
        }

        QOpenGLTexture *blade = new QOpenGLTexture(tree);
        blade->setWrapMode(QOpenGLTexture::Repeat);
        blade->setMinificationFilter(QOpenGLTexture::Linear);
        blade->setMagnificationFilter(QOpenGLTexture::Linear);
        blade->setWrapMode(QOpenGLTexture::Repeat);

        treeBlade << blade;
    }

}

TreeFactory::~TreeFactory() {
    for (int i = 0; i < treeBlade.size(); i++) {
        delete treeBlade[i];
    }
    treeBlade.clear();
}

Tree* TreeFactory::createTree(TreeType treeType, QVector2D center, double radius, double spacing, double size, double height, int seed, double chance) {
    Tree* tree = new Tree(this, treeType);
    createBillboard(tree, center, radius, spacing, size, height, seed, chance);
    return tree;
}
