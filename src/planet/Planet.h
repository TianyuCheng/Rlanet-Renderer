#ifndef PLANET_H_
#define PLANET_H_

#include <QString>

#include "SceneObject.h"

class Planet : public SceneObject
{
public:
    Planet(QString n) : SceneObject(n, "../glsl/torus.vert", "../glsl/red.frag") {
        vertices.append(QVector3D( 1, -1, 0));
        vertices.append(QVector3D( 1,  1, 0));
        vertices.append(QVector3D(-1,  1, 0));
        vertices.append(QVector3D(-1, -1, 0));

        indices.append(0);
        indices.append(1);
        indices.append(2);

        indices.append(2);
        indices.append(3);
        indices.append(0);
    }

    virtual ~Planet () {
    }

    void update() {
        qDebug() << "Planet Update";
    }

    void uniform() {
        qDebug() << "Planet uniform";
    }

private:
    /* data */
};

#endif /* end of include guard: PLANET_H_ */
