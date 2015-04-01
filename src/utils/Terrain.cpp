#include "Terrain.h"

// TerrainPatch class

TerrainPatch::TerrainPatch(QVector2D p, int l, QVector< QPair<double, double> > *r) : pos(p), level(l), ranges(*r) {
    for (int i = 0; i < 4; i++)
        children[i] = nullptr;

    // set up the bounding box
    // Note: this bounding box does not contain height information
    // We just insert a dummy value of 1 for height. However, we can
    // change it when we generate the heightmap by itself
    int size = ranges[level].first;
    QVector3D min = QVector3D(p.x(), 0.0, p.y());
    QVector3D max = QVector3D(p.x() + size, 1.0, p.y() + size);
    bounds.setMin(min);
    bounds.setMax(max);
    bounds.updateCorners();
}

TerrainPatch::~TerrainPatch() {
    // Clean up all children
    for (int i = 0; i < 4; i++) {
        if (children[i]) {
            delete children[i];
            children[i] = nullptr;
        }
    }
}

void TerrainPatch::selectPatches(Camera &camera, QVector3D &cameraPos, QVector<TerrainPatch*> &selectedPatches) {
    
    // Test it first with camera for frustum culling
    if (camera.isCullable(bounds) == Camera::Cullable::TOTALLY_CULLABLE) return;

    int size = ranges[level].first;

    if (level == 0 || !bounds.intersectSphere(cameraPos, ranges[level - 1].first)) {
        // qDebug() << "Add node" << pos << "from level" << level;
        // Add this node to scene
        selectedPatches << this;
    }
    else {
        for (int i = 0; i < 4; i++) {
            // If the child is never instantiated, create it
            if (!children[i]) {
                QVector2D childPos;
                switch (i) {
                    case 0:
                        childPos = QVector2D(pos.x() + size / 2.0, pos.y() + size / 2.0);
                        break;
                    case 1:
                        childPos = QVector2D(pos.x(), pos.y() + size / 2.0);
                        break;
                    case 2:
                        childPos = QVector2D(pos.x(), pos.y());
                        break;
                    default:
                        childPos = QVector2D(pos.x() + size / 2.0, pos.y());
                }
                children[i] = new TerrainPatch(childPos, level - 1, &ranges);
                // qDebug() << "Created node at " << childPos << "level:" << level - 1;
            }
            // Recursively add children patches
            children[i]->selectPatches(camera, cameraPos, selectedPatches);
        }
    }
}


// Terrain class

Terrain::Terrain(int g, int l, Scene *parent) :
    SceneObject(QString("CDLOD Terrain"), "../glsl/terrain.vert", "../glsl/terrain.frag", parent), 
    grid(g), levels(l) {

    {
        QImage decal("../textures/decal_dirt.jpg");
        QImage height("../textures/heightmap4.jpg");
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
    }

    // Initialize ranges
    double range = 1.0, morph = range;
    for (int i = 0; i <= levels; i++) {
        ranges << qMakePair(range, morph);
        range *= 2;
        morph = 0.85 * range;       // let morph area takes 15% less
    }
    
    // rangesInfo();
    
    // Generate vertex and index data
    // All levels of terrain patch will be using exactly the 
    // same set of vertices, but with different scales
    {
        float size = 1.0 / grid;
        for (int j = 0; j <= grid; j++) {
            for (int i = 0; i <= grid; i++) {
                vertices << QVector3D(i * size, 0, j * size);
            }
        }
        for (int j = 0; j < grid; j++) {
            for (int i = 0; i < grid; i++) {
                // Upper triangle
                indices << (j * (grid + 1) + i + 1)
                        << (j * (grid + 1) + i) 
                        << ((j + 1) * (grid + 1) + i);

                // Bottom triangle
                indices << ((j + 1) * (grid + 1) + i) 
                        << ((j + 1) * (grid + 1) + i + 1)
                        << (j * (grid + 1) + i + 1); 
            }
        }
    }
}

Terrain::~Terrain() {
    qDeleteAll( children );  //  deletes all the values stored in "children"
    children.clear();        //  removes all items from children
}

void Terrain::updatePatches() {
    Camera* camera = dynamic_cast<Scene*>(parent)->getCamera();
    QVector3D cameraPos = camera->getPosition();
    cameraPos.setY(0.0);     // To test level of detail, uncomment this

    int far = qNextPowerOfTwo(int(camera->getFar()));
    int size = int(ranges[levels].first);
    int num = qCeil(far / size);
    
    int x = qRound(cameraPos.x() / size) * size;
    int y = qRound(cameraPos.z() / size) * size;

    for (int i = (-num - 1) * size; i <= num * size; i += size) {
        for (int j = (-num - 1) * size; j <= num * size; j += size) {
            QPair<int, int> key(x + i, y + j);
            if (!children.contains(key)) {
                children.insert(key, new TerrainPatch(QVector2D(x + i, y + j), levels, &ranges));
                // qDebug() << "Insert: " << key;
            }
        }
    }

    // allPatchesInfo();

    selectPatches(*camera, cameraPos);
}

void Terrain::selectPatches(Camera &camera, QVector3D &cameraPos) {
    selectedPatches.clear();
    QMapIterator< QPair<int, int>, TerrainPatch*> iter(children);
    while (iter.hasNext()) {
        iter.next();
        iter.value()->selectPatches(camera, cameraPos, selectedPatches);
    }
}

void Terrain::rangesInfo() {
    // Print ranges out for debugging
    for (int i = 0; i < levels; i++)
        qDebug() << ranges[i];
}

void Terrain::allPatchesInfo() {
    qDebug() << "Terrain contains the following patches:";
    QMapIterator< QPair<int, int>, TerrainPatch*> iter(children);
    while (iter.hasNext()) {
        iter.next();
        qDebug() << iter.key();
    }
}

void Terrain::uniform() {
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
    program.setUniformValue("uGrid", float(grid));
}

void Terrain::update() {
    updatePatches();
}

void Terrain::render() {
    // Drawing only using line mode for LOD visualization
    // drawMode = GL_LINE;
    // glPolygonMode( GL_FRONT_AND_BACK, drawMode );
    glPolygonMode( GL_FRONT, drawMode );

    qDebug() << "Children:" << children.size() << "Selected:" << selectedPatches.size();
    for (int i = 0; i < selectedPatches.size(); i++) {
        TerrainPatch *patch = selectedPatches[i];
        double scaleFactor = ranges[patch->level].first;
        // qDebug() << "level:" << patch->level << "pos:" << patch->pos << "scale:" << scaleFactor;
        QVector2D scale(scaleFactor, scaleFactor);
        program.setUniformValue("uOffset", patch->pos);
        program.setUniformValue("uScale", scale);
        program.setUniformValue("uLevel", patch->level);

        // draw all the triangles
        glDrawElements(
                GL_TRIANGLES, 
                indices.size(),    // count
                GL_UNSIGNED_INT,   // type
                indices.constData());         // element array buffer offset
    } // end of for loop
}
