
#ifndef TERRAIN_H
#define TERRAIN_H

#include <cmath>

#include <QString>
#include <QOpenGLTexture>

#include "SceneObject.h"


class Terrain : public SceneObject
{
public:
        Terrain (QString n, int _levels, int _gridSize) 
            : SceneObject(n, "../glsl/terrain.vert", "../glsl/terrain.frag"), 
            levels(_levels), gridSize(_gridSize)
        {
            // Initialize the starting view position
            viewpos = QVector2D(0, 0);

            // Load the textures
            {
                QImage decal("../textures/decalmap.jpg");
                QImage height("../textures/heightmap.jpg");
                // QImage height("../textures/heightmap2.jpg");
                if (decal.isNull() || height.isNull()) {
                    qDebug() << "Decal/Height map for terrain has not been found!";
                    exit(-1);
                }
                decalmap  = new QOpenGLTexture(decal);
                heightmap = new QOpenGLTexture(height);

                decalmap->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
                decalmap->setMagnificationFilter(QOpenGLTexture::Linear);

                heightmap->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
                heightmap->setMagnificationFilter(QOpenGLTexture::Linear);

                // Adjusting grid size to 2^n - 1
                gridExp = ceil(log(gridSize)/log(2));
                gridSize = pow(2, gridExp) - 1;
            }

            // Initialize Terrain Ring
            {
                int n = gridSize;
                generateFootprints(0, 0, n, n);
            }

            // Print Debug Info
            {
                qDebug() << "Generating Clipmap Terrain with" << levels << "levels";
                qDebug() << "Terrain Grid Size:" << gridSize;
                qDebug() << "Terrain Vertex Numbers:" << vertices.size();
                qDebug() << "Terrain Index Numbers:" << indices.size();
            } 
        }

        virtual ~Terrain ()
        {
            delete decalmap;
            delete heightmap;
        }

        void update() {
            viewpos += QVector2D(0, 2) * gridSize;
        }

        void uniform() {
            /**
             * I do not understand why this works, but it just works.
             * If I put this in the reversed order. Nothing appears.
             * Like this:
             *  heightmap->bind(0);
             *  decalmap->bind(1);
             * */
            decalmap->bind(1);
            heightmap->bind(0);

            int heightLocation = program.uniformLocation("uHeightmap");
            int decalLocation = program.uniformLocation("uDecalmap");
            program.setUniformValue(heightLocation, 0);
            program.setUniformValue(decalLocation, 1);

            int heightScaleLocation = program.uniformLocation("uHeightScale");
            program.setUniformValue(heightScaleLocation, .1f);
        }

        void render() {
            // Draw all levels with different details 
            // using the same set of vertices, but different
            // scales and offsets
            int size = 512;

            QVector2D mapPosition = viewpos / float( 2 * size * gridSize);
            program.setUniformValue("uMapPos", mapPosition);

            // // qDebug() << "viewpos:" << mapPosition;
            float scaleFactor = 8.0;
            float fullSize = scaleFactor;
            for (int i = 0; i < levels; i++) {
                float ox = (int(viewpos.x() * (1 << i)) & (size - 1)) / float(size * gridSize);
                float oy = (int(viewpos.y() * (1 << i)) & (size - 1)) / float(size * gridSize);

                // Calculate scale
                QVector2D scale (scaleFactor * 0.25, scaleFactor * 0.25);
                program.setUniformValue("uScale", scale);

                // draw four patches
                for (int k = -2; k < 2; k++) {
                    for (int j = -2; j < 2; j++) {
                        // Do not draw the inner part, leave it to the next level
                        if (i != levels - 1) if (k == -1 || k == 0) if (j == -1 || j == 0) continue;

                        // Calculate offset
                        float offsetX = ox + float(j);
                        float offsetY = oy + float(k);

                        // Adjust offset for proper overlapping
                        if (k >= 0) offsetY -= 1.0 / float(gridSize);
                        if (j >= 0) offsetX -= 1.0 / float(gridSize);

                        // Calculate the offset
                        QVector2D offset (offsetX, offsetY);
                        // offset += temp;

                        program.setUniformValue("uOffset", offset);
                        program.setUniformValue("uLevel", float(i));

                        // // Cull
                        // int xp = 0, xm = 0, yp = 0, ym = 0, zp = 0;
                        // for (int l = 0; l < 2; l++) 
                        //     for (int m = 0; m < 2; m++)
                        //          for (int n = 0; n < 2; n++) {
                        //              QVector3D v = scale * (offset + QVector3D(l, m, float(-n) * 0.05));
                        //          }

                        // Render the patch for this level
                        glDrawElements(
                                GL_TRIANGLES,      // mode
                                // GL_LINES,
                                indices.size(),    // count
                                GL_UNSIGNED_INT,   // type
                                indices.constData());         // element array buffer offset
                    }
                }
                scaleFactor *= 0.5;
            }

        }

    private:
        /**
         * x: starting horizontal position
         * y: starting vertical position
         * w: width of footprint
         * h: height of footprint
         * */
        void generateFootprints(int x, int y, int w, int h) {
            // Used later for indices generation
            int indexOffset = vertices.size();

            // Calculate the size for each square/triangle

            // Insert the vertices
            float size = 1.0 / gridSize;
            for (int j = 0; j <= h; j++) {
                for (int i = 0; i <= w; i++) {
                    vertices << QVector3D((x + i) * size, (y + j) * size, 0);
                }
            }

            // Insert the indices
            for (int j = 0; j < h; j++) {
                for (int i = 0; i < w; i++) {
                    // Upper triangle
                    indices << (indexOffset + j * (w + 1) + i) 
                        << (indexOffset + (j + 1) * (w + 1) + i + 1) 
                        << (indexOffset + j * (w + 1) + i + 1);

                    // Bottom triangle
                    indices << (indexOffset + j * (w + 1) + i) 
                        << (indexOffset + (j + 1) * (w + 1) + i) 
                        << (indexOffset + (j + 1) * (w + 1) + i + 1);
                }
            }
        }

    private:
        int levels;
        int gridSize;
        int gridExp;

        int indexSize;
        QVector2D viewpos;

        QOpenGLTexture *decalmap;
        QOpenGLTexture *heightmap;
};


#endif /* end of include guard: TERRAIN_H */
