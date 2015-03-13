#ifndef PLANET_H_
#define PLANET_H_

#include <QString>

#include "SceneObject.h"

class Planet : public SceneObject
{
public:
    Planet(QString n) : SceneObject(n, "../glsl/torus.vert", "../glsl/red.frag") {
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
