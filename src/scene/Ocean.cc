#include "Noise.h"
#include "Ocean.h"

Ocean::Ocean(
        GraphicsDevice *device, 
        int grid, 
        int levels, 
        Scene *parent) : Terrain(device, grid, levels, parent) {

}

Ocean::~Ocean() {
}

void Ocean::initialize() {
    this->setShader(QOpenGLShader::Vertex, "../glsl/ocean.vert");
    this->setShader(QOpenGLShader::Geometry, "../glsl/ocean.geom");
    this->setShader(QOpenGLShader::Fragment, "../glsl/ocean.frag");

    SceneObject::initialize();
    time.start();
}

void Ocean::uniform() {
    decalmap[0]->bind(1);
    int decalLocation0 = program.uniformLocation("uVertexDisplacement");
    program.setUniformValue(decalLocation0, 1);
	heightmap->bind(2);
	int heightloc = program.uniformLocation("uHeightmap");
	program.setUniformValue(heightloc, 2);

    float elapsedTime = float(time.elapsed()) / 1e3;
    program.setUniformValue("uTime", elapsedTime);

    program.setUniformValue("uGrid", float(grid));

    Camera* camera = dynamic_cast<Scene*>(parent)->getCamera();
    QVector3D cameraPos = camera->getPosition();
    program.setUniformValue("uCamera", cameraPos);

    CHECK_GL_ERROR(device, "after sets uniforms");
    // this->setDrawMode(GL_LINE);
}

void Ocean::init_gl_resource()
{
    QImage ocean_displacement("../textures/ocean/vtf.jpg");
    // Check whether texture are loaded
    if (ocean_displacement.isNull()) {
        qDebug() << "Decal/Height map for ocean has not been found!";
        exit(-1);
    }

    decalmap[0].reset(new QOpenGLTexture(ocean_displacement));
    decalmap[0]->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    decalmap[0]->setMagnificationFilter(QOpenGLTexture::Linear);
}
