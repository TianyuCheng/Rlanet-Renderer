
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
            int m = (n + 1) / 4;

            generateFootprints(0, 0, n, n);
            
            // // Generate 12 mxm footprints
            // generateFootprints(0, 0, m, m);
            // generateFootprints(m - 1, 0, m, m);
            // generateFootprints(n - 2 * m + 1, 0, m, m);
            // generateFootprints(n - m, 0, m, m);
            //
            // generateFootprints(0, m - 1, m, m);
            // generateFootprints(n - m, m - 1, m, m);
            // generateFootprints(0, n - 2 * m + 1, m, m);
            // generateFootprints(n - m, n - 2 * m + 1, m, m);
            //
            // generateFootprints(0, n - m, m, m);
            // generateFootprints(m - 1, n - m, m, m);
            // generateFootprints(n - 2 * m + 1, n - m, m, m);
            // generateFootprints(n - m, n - m, m, m);
            //
            // // Generate trims fix-ups
            // generateFootprints(0, 2 * m - 2, m, 3);
            // generateFootprints(n - m, 2 * m - 2, m, 3);
            //
            // generateFootprints(2 * m - 2, 0, 3, m);
            // generateFootprints(2 * m - 2, n - m, 3, m);
            //
            // // Generate Interior Trim
            // generateFootprints(m - 1, m - 1, 2 * m + 1, 2);
            // generateFootprints(m - 1, m, 2, 2 * m + 1);
            //
            // // For most levels we render up to this point
            // indexSize = indices.size();
            //
            // // For the finest level, we render the interior as well
            // // Generate Ring Interior
            // generateFootprints(m, m, 2 * m + 1, 2 * m + 1);
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
        // viewpos += QVector2D(1, 1) * pow(2, gridExp);
        viewpos += QVector2D(40, 40);
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
        program.setUniformValue(heightScaleLocation, 20.0f);
    }

    void render() {
        // Draw all levels with different details 
        // using the same set of vertices, but different
        // scales and offsets
        float denom = 1 << levels;
        float scaleFactor = 1.0;
        for (int i = 0; i <= levels; i++) {
            float snappedViewCoeff = pow(2, gridExp - levels);

            QVector2D scaledPos = viewpos / (1 << (levels - i));
            // For calculatinng the uv for each vertex
            QVector2D snappedView = QVector2D(
                    snappedViewCoeff * int(scaledPos.x() / (1 << (i + 1))) * (1 << (i + 1)),
                    snappedViewCoeff * int(scaledPos.y() / (1 << (i + 1))) * (1 << (i + 1))
            );
            // For calculate the offset for the each vertex in this level
            QVector2D snappedPos = QVector2D(
                    -int(scaledPos.x() / (1 << (levels - i + 1))) * (1 << (levels - i + 1)),
                    -int(scaledPos.y() / (1 << (levels - i + 1))) * (1 << (levels - i + 1))
            );

            // For updating vertex offset
            QVector2D scale(scaleFactor, scaleFactor);
            // QVector2D offset(0, 0);
            QVector2D offset = snappedPos / denom;
            float ox = offset.x(), oy = offset.y();
            ox -= (int(ox) / 2) * 2;
            oy -= (int(oy) / 2) * 2;
            offset = QVector2D(ox, oy);

            // For updating texture uv
            QVector4D fineBlockOrig = QVector4D(
                    scaleFactor / (denom),
                    scaleFactor / (denom),
                    (snappedView.x() - denom * gridSize / 2.0) / (denom * gridSize),
                    (snappedView.y() - denom * gridSize / 2.0) / (denom * gridSize)
            );

            // qDebug() << "Level" << i << viewpos << snappedPos << snappedView;

            // Uniform variables
            program.setUniformValue("uFineBlockOrig", fineBlockOrig);
            program.setUniformValue("uScale", scale) ;
            program.setUniformValue("uOffset", offset) ;

            scaleFactor *= 2.0;

            glDrawElements(
                    GL_TRIANGLES,      // mode
                    // GL_LINES,
                    // i == 0 ? indices.size() : indexSize,
                    indices.size(),
                    GL_UNSIGNED_INT,   // type
                    indices.constData());         // element array buffer offset
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
                vertices << QVector3D((x + i) * size - 0.5, (y + j) * size - 0.5, 0);
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
