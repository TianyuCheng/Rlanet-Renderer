#ifndef TERRAIN_H
#define TERRAIN_H

#include <QString>
#include <QOpenGLTexture>

#include "SceneObject.h"


class Terrain : public SceneObject
{
public:
    Terrain (QString n) : SceneObject(n, "../glsl/terrain.vert", "../glsl/terrain.frag")
    {
        viewpos = QVector2D(0, 0);
        grid = 32;
        levels = 10;
        for (int j = 0; j < grid + 1; j++)
            for (int i = 0; i < grid + 2; i++) {
                for (int k = 0; k < (i == 0 ? 2 : 1); k++) {
                    vertices.append(QVector3D(float(i)/ grid, 0, float(j) / grid));
                }
                ++j;
                for (int k = 0; k < (i == grid + 1 ? 2 : 1); k++) {
                    vertices.append(QVector3D(float(i)/ grid, 0, float(j) / grid));
                }
                --j;
            }

        heightmap = new QOpenGLTexture(QImage("../heightmaps/test.jpg"));
    }

    virtual ~Terrain ()
    {
        delete heightmap;
    }

    void update() {
        // viewpos.setX(viewpos.x() + 10.0);
        // viewpos.setY(viewpos.y() + 10.0);
    }

    void uniform() {
        heightmap->bind(0);
        program.setUniformValue("uHeightmap", 0);
    }

    void render() {
        float sxz = 10.0;
        program.setUniformValue("uMapPosition", QVector4D(
                    int(-viewpos.x() / float(2 * 512 * grid)),
                    0,
                    int(-viewpos.y() / float(2 * 512 * grid)),
                    0
        ));

        for (int i = 0; i < levels; i++) {
            float ox = (int(viewpos.x() * (1 << i)) & 511) / float(512 * grid);
            float oy = (int(viewpos.y() * (1 << i)) & 511) / float(512 * grid);
            // qDebug() << ox << "\t" << oy;

            QVector4D scale(sxz / 4.0, 1.0, sxz / 4.0, 1.0);
            program.setUniformValue("uScale", scale);
            program.setUniformValue("level", float(i + 1) / levels);
            // qDebug() << float(i + 1) / levels;

            for (int k = -2; k < 2; k++) 
                for (int j = -2; j < 2; j++) {
                    if (i != levels - 1) if (k == -1 || k == 0) if (j == -1 || j == 0) continue;

                    QVector4D offset(ox + float(j), 0, oy + float(k), 0);
                    if (k >= 0) offset.setZ(offset.z() - 1.0 / float(grid));
                    if (j >= 0) offset.setX(offset.x() - 1.0 / float(grid));

                    // not doing frustum culling now

                    program.setUniformValue("uOffset", offset);
                    // glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices.size());
                    glDrawArrays(GL_LINES, 0, vertices.size());
                }
            
            sxz *= 0.5;
        }
    }

private:
    int grid;
    int levels;
    QVector2D viewpos;
    QOpenGLTexture *heightmap;
};


#endif /* end of include guard: TERRAIN_H */
