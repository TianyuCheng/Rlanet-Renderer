#include "Grass.h"

Grass::Grass(GrassFactory *f)
    : SceneObject(QString("Grass"), f), factory(f)
{
    setShader(f->vShader.get());
    setShader(f->gShader.get());
    setShader(f->fShader.get());

}

Grass::~Grass() {

}

void Grass::plantGrass(QVector2D coordinate, double height) {
    vertices << QVector3D(coordinate.x(), coordinate.y(), height);
}

void Grass::uniform() {
    // uniform height map
    factory->terrain->bindHeightmap(program, "uHeightmap", 1);

    // uniform the grass texture and alpha map
    factory->grassBlade->bind(2);
    factory->grassBladeAlpha->bind(3);
    program.setUniformValue("uDecalmap", 2);
    program.setUniformValue("uAlphamap", 3);

    // uniform the size of grass
    program.setUniformValue("uSize", float(size));
}

void Grass::update() {

}

void Grass::render() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	vbo_.bind();
	program.setAttributeBuffer(vertexLocation_, GL_FLOAT, 0, 3);
	program.enableAttributeArray(vertexLocation_);
	CHECK_GL_ERROR("After buffer");

	glDrawArrays(GL_POINTS, 0, vertices.size());
	CHECK_GL_ERROR("After render");

	vbo_.release();

    glDisable(GL_BLEND);
}

GrassFactory::GrassFactory(Terrain *t) 
    : SceneObject("Grass Factory"), terrain(t) {

    QImage grass("../textures/grass.jpg");
    QImage grassAlpha("../textures/grass_alpha.jpg");

    if (grass.isNull() || grassAlpha.isNull()) {
        qDebug() << "Grass texture has not been found!";
        exit(-1);
    }

    setShader(QOpenGLShader::Vertex, "../glsl/grass.vert");
    setShader(QOpenGLShader::Geometry, "../glsl/grass.geom");
    setShader(QOpenGLShader::Fragment, "../glsl/grass.frag");

    grassBlade.reset(new QOpenGLTexture(grass));
    grassBlade->setWrapMode(QOpenGLTexture::Repeat);
    grassBlade->setMinificationFilter(QOpenGLTexture::Linear);
    grassBlade->setMagnificationFilter(QOpenGLTexture::Linear);
    grassBlade->setWrapMode(QOpenGLTexture::Repeat);

    grassBladeAlpha.reset(new QOpenGLTexture(grassAlpha));
    grassBladeAlpha->setWrapMode(QOpenGLTexture::Repeat);
    grassBladeAlpha->setMinificationFilter(QOpenGLTexture::Linear);
    grassBladeAlpha->setMagnificationFilter(QOpenGLTexture::Linear);
    grassBladeAlpha->setWrapMode(QOpenGLTexture::Repeat);
}

GrassFactory::~GrassFactory() {
}

Grass* GrassFactory::createGrass(QVector2D center, double radius, double spacing, double size, double height, int seed) {
    if (!seed) { seed = clock(); }
    srand(seed);

    Grass* grass = new Grass(this);
    grass->setSize(size);
    double cx = center.x();
    double cy = center.y();
    for (double x = -radius; x <= radius; x += spacing) {
        for (double y = -radius; y <= radius; y += spacing) {
            QVector2D point(x, y);
            // Check if the point is in the circle
            if (point.length() <= radius) {
                double rnd = ((rand() / (float)RAND_MAX) * 0.25 - 0.125) * height;
                grass->plantGrass(QVector2D(cx + x, cy + y), height + rnd);
            }
        }
    }
    return grass;
}
