#include "Grass.h"

GrassBlade::GrassBlade() {

}

GrassBlade::~GrassBlade() {

}

Grass::Grass() : numGrassVariations(10),
    SceneObject(QString("Grass"), parent) {

    QImage grass("../textures/grass.png");

    if (grass.isNull()) {
        qDebug() << "Grass texture has not been found!";
        exit(-1);
    }

    grassBlade.reset(new QOpenGLTexture(grass));
    prepareGrassBlades();
}

Grass::~Grass() {
    for (int i = 0; i < numGrassVariations; i++) {
        delete blades[i];
    }
    blades.clear();
}

void Grass::prepareGrassBlades() {
    for (int i = 0; i < numGrassVariations; i++) {
        blades << new GrassBlade();
    }
}
