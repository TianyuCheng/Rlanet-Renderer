#include "Grass.h"

Grass::Grass(GrassFactory *f, int seed)
    : SceneObject(QString("Grass"), f), factory(f)
{
    setShader(f->vShader.get());
    setShader(f->fShader.get());

    srand(seed);
    generateBillboard();

    offset = QVector3D(0, 500.0, 1000);
    scale = QVector3D(100, 150, 100);
}

Grass::~Grass() {

}

void Grass::generateBillboard() {
    float angle;
    float offset;
    QQuaternion q;
    QVector3D addition;

    QVector3D corners[4];
    corners[0] = QVector3D(0.5, 0.0, 0.0);
    corners[1] = QVector3D(0.5, 0.5, 0.0);
    corners[2] = QVector3D(-0.5, 0.5, 0.0);
    corners[3] = QVector3D(-0.5, 0.0, 0.0);

    angle = ((rand() / (float)RAND_MAX) * 0.3 + 0.3) * 90.0;
    offset = (rand() / (float)RAND_MAX) * 0.25;
    addition = QVector3D(offset, 0.0, 0.0);
    
    // one face
    q = QQuaternion::fromAxisAndAngle(QVector3D(0.0, 1.0, 0.0), angle);
    vertices << q.rotatedVector(corners[0] + addition)
             <<  q.rotatedVector(corners[1] + addition)
             <<  q.rotatedVector(corners[2] + addition)
             <<  q.rotatedVector(corners[3] + addition);
    texcoords << QVector2D(0.0, 1.0)
              << QVector2D(0.0, 0.0)
              << QVector2D(1.0, 0.0)
              << QVector2D(1.0, 1.0);
    indices << 0 << 1 << 2;
    indices << 2 << 3 << 0;

    // second face
    q = QQuaternion::fromAxisAndAngle(QVector3D(0.0, 1.0, 0.0), -angle);
    vertices << q.rotatedVector(corners[0] - addition)
             <<  q.rotatedVector(corners[1] - addition)
             <<  q.rotatedVector(corners[2] - addition)
             <<  q.rotatedVector(corners[3] - addition);
    texcoords << QVector2D(0.0, 1.0)
              << QVector2D(0.0, 0.0)
              << QVector2D(1.0, 0.0)
              << QVector2D(1.0, 1.0);
    indices << 4 << 5 << 6;
    indices << 6 << 7 << 4;

    // third face
    addition = QVector3D(0.0, 0.0, -offset);
    vertices << (corners[0] + addition) 
             << (corners[1] + addition) 
             << (corners[2] + addition) 
             << (corners[3] + addition);
    texcoords << QVector2D(0.0, 1.0)
              << QVector2D(0.0, 0.0)
              << QVector2D(1.0, 0.0)
              << QVector2D(1.0, 1.0);
    indices << 8 << 9 << 10;
    indices << 10 << 11 << 8;
}

void Grass::uniform() {
    factory->grassBlade->bind(1);
    factory->grassBladeAlpha->bind(2);

    program.setUniformValue("uDecalmap", 1);
    program.setUniformValue("uAlphamap", 2);

    program.setUniformValue("uScale", scale);
    program.setUniformValue("uOffset", offset);
}

void Grass::update() {

}

void Grass::render() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    SceneObject::render();
    glDisable(GL_BLEND);
}

GrassFactory::GrassFactory(int n) : SceneObject("Grass Factory"),  numVariations(n) {

    QImage grass("../textures/grass.jpg");
    QImage grassAlpha("../textures/grass_alpha.jpg");

    if (grass.isNull() || grassAlpha.isNull()) {
        qDebug() << "Grass texture has not been found!";
        exit(-1);
    }

    setShader(QOpenGLShader::Vertex, "../glsl/grass.vert");
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

    prepareGrassBlades();
}

GrassFactory::~GrassFactory() {
    for (int i = 0; i < numVariations; i++) {
        delete blades[i];
    }
    blades.clear();
}

Grass* GrassFactory::getGrass(int index) {
    Q_ASSERT(index < numVariations);
    return blades[index];
}

void GrassFactory::prepareGrassBlades() {
    int seed = clock();
    for (int i = 0; i < numVariations; i++) {
        blades << new Grass(this, seed + i);
    }
}
