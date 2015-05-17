#include "Grass.h"

	Grass::Grass(GrassFactory *f)
: Billboard(f), factory(f)
{
	setShader(f->vShader.get());
	setShader(f->gShader.get());
	setShader(f->fShader.get());
}

Grass::~Grass() {

}

void Grass::uniform() {
	// uniform height map
	CHECK_GL_ERROR(factory->device, "Grass: before bind uHeightmap");
	factory->terrain->bindHeightmap(program, "uHeightmap", 1);
	CHECK_GL_ERROR(factory->device, "Grass: After uniform uHeightmap");

	// uniform the grass texture and alpha map
	factory->grassBlade->bind(2);
	program.setUniformValue("uDecalmap", 2);
	CHECK_GL_ERROR(factory->device, "Grass: After set uAlphamap");

	// uniform the size of grass
	program.setUniformValue("uSize", float(size));
	CHECK_GL_ERROR(factory->device, "Grass: After uSize");
}

void Grass::update() {
}

GrassFactory::GrassFactory(GraphicsDevice *device, Terrain *t) 
	: BillboardFactory(device, "Grass Factory"), terrain(t)
{

	QImage grass("../textures/billboard/grass.png");

	if (grass.isNull()) {
		qDebug() << "Grass texture has not been found!";
		exit(-1);
	}

	setShader(QOpenGLShader::Vertex, "../glsl/billboard.vert");
	setShader(QOpenGLShader::Geometry, "../glsl/grass.geom");
	setShader(QOpenGLShader::Fragment, "../glsl/billboard.frag");

	grassBlade.reset(new QOpenGLTexture(grass));
	grassBlade->setWrapMode(QOpenGLTexture::Repeat);
	grassBlade->setMinificationFilter(QOpenGLTexture::Linear);
	grassBlade->setMagnificationFilter(QOpenGLTexture::Linear);
	grassBlade->setWrapMode(QOpenGLTexture::Repeat);
}

GrassFactory::~GrassFactory() {
	grassBlade.reset();
}

Grass* GrassFactory::createGrass(QVector2D center, double radius, double spacing, double size, double height, int seed, double chance) {
	Grass* grass = new Grass(this);
	createBillboard(grass,
			center,
			radius,
			spacing,
			size,
			height,
			seed,
			chance
			);
	return grass;
}
