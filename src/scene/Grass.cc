#include "Grass.h"

Grass::Grass(int seed)
    : SceneObject(QString("Grass"), parent) 
{
    this->setShader(QOpenGLShader::Vertex, "../glsl/grass.vert");
    this->setShader(QOpenGLShader::Fragment, "../glsl/grass.frag");

    srand(seed);
    generateBillboard();
    scale(10.0);
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
    offset = (rand() / (float)RAND_MAX) * 0.5;
    addition = QVector3D(offset, 0.0, 0.0);
    
    // one face
    q = QQuaternion::fromAxisAndAngle(QVector3D(0.0, 1.0, 0.0), angle);
    vertices << q.rotatedVector(corners[0] + addition)
             <<  q.rotatedVector(corners[1] + addition)
             <<  q.rotatedVector(corners[2] + addition)
             <<  q.rotatedVector(corners[3] + addition);
    
    // second face
    q = QQuaternion::fromAxisAndAngle(QVector3D(0.0, 1.0, 0.0), -angle);
    vertices << q.rotatedVector(corners[0] - addition)
             <<  q.rotatedVector(corners[1] - addition)
             <<  q.rotatedVector(corners[2] - addition)
             <<  q.rotatedVector(corners[3] - addition);

    // third face
    addition = QVector3D(0.0, 0.0, -offset);
    vertices << (corners[0] + addition) 
             << (corners[1] + addition) 
             << (corners[2] + addition) 
             << (corners[3] + addition);


    qDebug() << vertices;

    // Add indices here
    indices << 0 << 1 << 2;
    indices << 2 << 3 << 0;

    indices << 4 << 5 << 6;
    indices << 6 << 7 << 4;

    indices << 8 << 9 << 10;
    indices << 10 << 11 << 8;
}

void Grass::uniform() {

}

void Grass::update() {

}

// void Grass::render() {
// }

GrassFactory::GrassFactory(int n) : numVariations(n)
    {

    QImage grass("../textures/grass.png");

    if (grass.isNull()) {
        qDebug() << "Grass texture has not been found!";
        exit(-1);
    }

    grassBlade.reset(new QOpenGLTexture(grass));
    prepareGrassBlades();
}

GrassFactory::~GrassFactory() {
    for (int i = 0; i < numVariations; i++) {
        delete blades[i];
    }
    blades.clear();
}

void GrassFactory::prepareGrassBlades() {
    int seed = clock();
    for (int i = 0; i < numVariations; i++) {
        blades << new Grass(seed + i);
    }
}
