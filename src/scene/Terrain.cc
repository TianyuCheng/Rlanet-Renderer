#include "Terrain.h"

// TerrainPatch class

TerrainPatch::TerrainPatch(QVector2D p, int l, QVector< QPair<double, double> > *r) : pos(p), level(l), ranges(*r) {
    for (int i = 0; i < 4; i++)
        children[i] = nullptr;

    // set up the bounding box
    // Note: this bounding box does not contain height information
    // We just insert a dummy value of 1000 for height. However, we can
    // change it when we generate the heightmap by itself
    int size = ranges[level].first;
    QVector3D min = QVector3D(p.x(), 0.0, p.y());
    QVector3D max = QVector3D(p.x() + size, 10000.0, p.y() + size);
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
    
    // // Test it first with camera for frustum culling
    // if (camera.isCullable(bounds) == Camera::Cullable::TOTALLY_CULLABLE) return;

    int size = ranges[level].first;

    if (level == 0 || !bounds.intersectSphere(cameraPos, ranges[level - 1].first)) {
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
        // Load terrain texture from file
        QImage decal_dirt("../textures/decal_dirt.jpg");
        QImage decal_grass("../textures/decal_grass.jpg");
        QImage decal_snow("../textures/decal_snow.jpg");

        // Generate heightmap using seed
        int r = 128;
        int w  = r * 2 * M_PI;
        int h = r * M_PI;
        QImage height(w, h, QImage::Format_RGB32);
        NoiseGenerator::SphericalHeightmap(height, r, 100);

        size = 2048;        // put it here temporarily

        // Check whether texture are loaded
        if (decal_dirt.isNull() || decal_grass.isNull() || decal_snow.isNull() || height.isNull()) {
            qDebug() << "Decal/Height map for terrain has not been found!";
            exit(-1);
        }
        decalmap[0].reset(new QOpenGLTexture(decal_grass));
        decalmap[1].reset(new QOpenGLTexture(decal_dirt));
        decalmap[2].reset(new QOpenGLTexture(decal_snow));
        heightmap.reset(new QOpenGLTexture(height));

        decalmap[0]->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        decalmap[0]->setMagnificationFilter(QOpenGLTexture::Linear);

        decalmap[1]->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        decalmap[1]->setMagnificationFilter(QOpenGLTexture::Linear);

        decalmap[2]->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        decalmap[2]->setMagnificationFilter(QOpenGLTexture::Linear);

        heightmap->setWrapMode(QOpenGLTexture::Repeat);
        heightmap->setMinificationFilter(QOpenGLTexture::Linear);
        heightmap->setMagnificationFilter(QOpenGLTexture::Linear);
        heightmap->setWrapMode(QOpenGLTexture::Repeat);
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
    // cameraPos.setY(0.0);     // To test level of detail, uncomment this

    // int far = qNextPowerOfTwo(int(camera->getFar()));
    int xfar = qCeil(size * M_PI);
    int zfar = qCeil(size * M_PI / 2);
    int size = int(ranges[levels].first);
    int xnum = qCeil(xfar / size);
    int znum = qCeil(zfar / size);
    
    // int x = qRound(cameraPos.x() / size) * size;
    // int y = qRound(cameraPos.z() / size) * size;
    int x = 0;
    int y = 0;

    // qDebug() << xnum * (2 * size + 1) << znum * (2 * size + 1);

    for (int i = (-znum - 1) * size; i <= znum * size; i += size) {
        for (int j = (-xnum - 1) * size; j <= xnum * size; j += size) {
            QPair<int, int> key(x + i, y + j);
            if (!children.contains(key)) {
                children.insert(key, new TerrainPatch(QVector2D(x + i, y + j), levels, &ranges));
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
    heightmap->bind(0);
    decalmap[0]->bind(1);
    decalmap[1]->bind(2);
    decalmap[2]->bind(3);

    int heightLocation = program.uniformLocation("uHeightmap");
    int decalLocation0 = program.uniformLocation("uDecalmap0");
    int decalLocation1 = program.uniformLocation("uDecalmap1");
    int decalLocation2 = program.uniformLocation("uDecalmap2");
    program.setUniformValue(heightLocation, 0);
    program.setUniformValue(decalLocation0, 1);
    program.setUniformValue(decalLocation1, 2);
    program.setUniformValue(decalLocation2, 3);
    program.setUniformValue("uGrid", float(grid));
    CHECK_GL_ERROR("after sets uniforms");
}

void Terrain::update() {
    updatePatches();
}

void Terrain::render()
{
	program.bind();
	int loc = program.uniformLocation("uOffset");
	//qDebug("uOffset location: %d", loc);
	// Drawing only using line mode for LOD visualization
	// drawMode = GL_LINE;
	glPolygonMode( GL_FRONT_AND_BACK, drawMode );
	CHECK_GL_ERROR("set polygon mode");

	vbo_.bind();
	CHECK_GL_ERROR("After vbo bind");
	program.setAttributeBuffer(vertexLocation_, GL_FLOAT, 0, 3);
	program.enableAttributeArray(vertexLocation_);
	CHECK_GL_ERROR("After set vertex location");
	ibo_.bind();
	CHECK_GL_ERROR("After ibo bind");

#if 0
	int vbname, ibname;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vbname);
	glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &ibname);
	qDebug("VB binded to %d, IB binded to %d\n", vbname, ibname);
#endif

	// qDebug() << "Children:" << children.size() << "Selected:" << selectedPatches.size();
	for (int i = 0; i < selectedPatches.size(); i++) {
		TerrainPatch *patch = selectedPatches[i];
		double scaleFactor = ranges[patch->level].first;
		QVector2D scale(scaleFactor, scaleFactor);
		program.setUniformValue("uOffset", patch->pos);
		//glUniform2f(loc, 1.0f, 1.0f);
		CHECK_GL_ERROR("After Terrian bind uOffset");
		program.setUniformValue("uScale", scale);
		CHECK_GL_ERROR("After Terrian bind uScale");
		program.setUniformValue("uLevel", patch->level);
		CHECK_GL_ERROR("After Terrian bind uLevel");

		// draw all the triangles
		glDrawElements(GL_TRIANGLES, 
				indices.size(),		// count
				GL_UNSIGNED_INT,	// type
				0);			// element array buffer offset
		CHECK_GL_ERROR("After Terrian draws");
	} // end of for loop
	program.release();
}
