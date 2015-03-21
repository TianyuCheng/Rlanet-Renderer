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
            gridSize = pow(2, ceil(log(gridSize)/log(2))) - 1;
        }

        // Initialize Terrain Ring
        {
            int n = gridSize;
            int m = (n + 1) / 4;
            int k = n - 4 * m;

            // Generate 12 mxm footprints
            generateFootprints(0, 0, m, m);
            generateFootprints(m - 1, 0, m, m);
            generateFootprints(n - 2 * m + 1, 0, m, m);
            generateFootprints(n - m, 0, m, m);

            generateFootprints(0, m - 1, m, m);
            generateFootprints(n - m, m - 1, m, m);
            generateFootprints(0, n - 2 * m + 1, m, m);
            generateFootprints(n - m, n - 2 * m + 1, m, m);

            generateFootprints(0, n - m, m, m);
            generateFootprints(m - 1, n - m, m, m);
            generateFootprints(n - 2 * m + 1, n - m, m, m);
            generateFootprints(n - m, n - m, m, m);

            // Generate trims fix-ups
            generateFootprints(0, 2 * m - 2, m, 3);
            generateFootprints(n - m, 2 * m - 2, m, 3);

            generateFootprints(2 * m - 2, 0, 3, m);
            generateFootprints(2 * m - 2, n - m, 3, m);

            // Generate Interior Trim
            generateFootprints(m - 1, m - 1, 2 * m + 1, 2);
            generateFootprints(m - 1, m, 2, 2 * m + 1);
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
        program.setUniformValue(heightScaleLocation, 3.0f);
    }

    void render() {
        // Draw all levels with different details 
        // using the same set of vertices, but different
        // scales and offsets
        float denom = 1 << levels;
        float scaleFactor = 1.0;
        for (int i = 0; i <= levels; i++) {

            QVector4D fineBlockOrig = QVector4D(
                    scaleFactor / denom,
                    scaleFactor / denom,
                    0.5, 0.5
            );
            program.setUniformValue("uFineBlockOrig", fineBlockOrig);

            QVector2D scale(scaleFactor, scaleFactor);
            QVector2D offset(-scaleFactor / 2.0 - scaleFactor / (2 * gridSize), -scaleFactor / 2.0 - scaleFactor / (2 * gridSize));
            scaleFactor *= 2.0;

            program.setUniformValue("uScale", scale) ;
            program.setUniformValue("uOffset", offset) ;

            glDrawElements(
                    GL_TRIANGLES,      // mode
                    // GL_LINES,
                    indices.size(),    // count
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
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                vertices << QVector3D((x + i) * size, (y + j) * size, 0);
            }
        }

        // Insert the indices
        for (int j = 0; j < h - 1; j++) {
            for (int i = 0; i < w - 1; i++) {
                // Upper triangle
                indices << (indexOffset + j * (w + 0) + i) 
                        << (indexOffset + (j + 1) * (w + 0) + i + 1) 
                        << (indexOffset + j * (w + 0) + i + 1);

                // Bottom triangle
                indices << (indexOffset + j * (w + 0) + i) 
                        << (indexOffset + (j + 1) * (w + 0) + i) 
                        << (indexOffset + (j + 1) * (w + 0) + i + 1);
            }
        }
    }

private:
    int levels;
    int gridSize;

    QVector2D viewpos;

    QOpenGLTexture *decalmap;
    QOpenGLTexture *heightmap;
};


#endif /* end of include guard: TERRAIN_H */
