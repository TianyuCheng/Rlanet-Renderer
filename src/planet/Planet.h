#ifndef PLANET_H_
#define PLANET_H_

#include <QString>

#include "SceneObject.h"

class Planet : public SceneObject
{
public:
    Planet(QString n) : SceneObject(n, "../glsl/sphere.vert", "../glsl/red.frag") {

        vertices.append(QVector3D( 1, -1, 1));
        vertices.append(QVector3D( 1,  1, 1));
        vertices.append(QVector3D(-1,  1, 1));
        vertices.append(QVector3D(-1, -1, 1));

        vertices.append(QVector3D( 1, -1, -1));
        vertices.append(QVector3D( 1,  1, -1));
        vertices.append(QVector3D(-1,  1, -1));
        vertices.append(QVector3D(-1, -1, -1));

        // front face
        indices.append(0); indices.append(1); indices.append(2);
        indices.append(2); indices.append(3); indices.append(0);

        // back face
        indices.append(4); indices.append(5); indices.append(6); 
        indices.append(6); indices.append(7); indices.append(4);

        // left face
        indices.append(3); indices.append(2); indices.append(6); 
        indices.append(6); indices.append(7); indices.append(3);

        // right face
        indices.append(0); indices.append(1); indices.append(5); 
        indices.append(5); indices.append(4); indices.append(0);

        // top face
        indices.append(1); indices.append(5); indices.append(6);
        indices.append(6); indices.append(3); indices.append(1);

        // bottom face
        indices.append(0); indices.append(4); indices.append(7); 
        indices.append(7); indices.append(3); indices.append(1);
    }

    virtual ~Planet () {
    }

    void update() {
        // qDebug() << "Planet Update";
    }

    void uniform() {
        // qDebug() << "Planet uniform";
    }

private:
    /* data */
};

#endif /* end of include guard: PLANET_H_ */
