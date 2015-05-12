#include <boost/range/irange.hpp>
#include "Terrain.h"
#include "HTileSys.h"

// TerrainPatch class

TerrainPatch::TerrainPatch(QVector2D p,
		int l,
		QVector< QPair<double, double> > *r)
	: pos(p), level(l), ranges(*r)
{
	for (int i = 0; i < 4; i++)
		children[i] = nullptr;

	// set up the bounding box
	// Note: this bounding box does not contain height information
	// We just insert a dummy value of 1000 for height. However, we can
	// change it when we generate the heightmap by itself
	int size = ranges[level].first;
	QVector3D min = QVector3D(p.x(), -2000.0, p.y());
	QVector3D max = QVector3D(p.x() + size, 2000.0, p.y() + size);
	bounds.setMin(min);
	bounds.setMax(max);
	bounds.updateCorners();
}

TerrainPatch::~TerrainPatch()
{
	// Clean up all children
	for (int i = 0; i < 4; i++) {
		if (children[i]) {
			delete children[i];
			children[i] = nullptr;
		}
	}
}

void TerrainPatch::selectPatches(Camera &camera,
		QVector3D &cameraPos,
		QVector<TerrainPatch*> &selectedPatches)
{

	// Test using distance
	if (level == ranges.size()) {
		float far = camera.getFar();
		if ((bounds.getMin() - cameraPos).length() > far ||
			(bounds.getMax() - cameraPos).length()) return;
	}

	// Test it first with camera for frustum culling
	if (camera.isCullable(bounds) == Camera::Cullable::TOTALLY_CULLABLE) return;

	int size = ranges[level].first;

	if (level == 0 || !bounds.intersectSphere(cameraPos, ranges[level].first)) {
		selectedPatches << this;
	} else if (!bounds.intersectSphere(cameraPos, ranges[level - 1].first)) {
		// Add this node to scene
		// selectedPatches << this;
		initializeChildren();
		for (int i = 0; i < 4; i++) 
			selectedPatches << children[i];
	} else {
        initializeChildren();
        // this patch has some patches that lying in the high detailed range
        // Partially add this patch
        // this->partiallySelectPatches(camera, cameraPos, selectedPatches);

        // try recursively adding children
        for (int i = 0; i < 4; i++) {
            TerrainPatch *child = children[i];
            child->selectPatches(camera, cameraPos, selectedPatches);
        }
	}
}

void TerrainPatch::initializeChildren() {
	// initialize all children if not initialized
	int nextSize = ranges[level - 1].first;
	children[0] = new TerrainPatch(QVector2D(pos.x() + nextSize, pos.y() + nextSize), level - 1, &ranges);
	children[1] = new TerrainPatch(QVector2D(pos.x(), pos.y() + nextSize), level - 1, &ranges);
	children[2] = new TerrainPatch(QVector2D(pos.x(), pos.y()), level - 1, &ranges);
	children[3] = new TerrainPatch(QVector2D(pos.x() + nextSize, pos.y()), level - 1, &ranges);
}

// Terrain class

Terrain::Terrain(int g, int l, Scene *parent) :
	SceneObject(QString("CDLOD Terrain"), parent), 
	grid(g), levels(l)
{
	init_gl_resource();

	// Initialize ranges
	double range = 64.0;
	for (int i = 0; i <= levels; i++) {
		ranges << qMakePair(range, range * 0.85);
		range *= 2.0;
	}

	// Generate vertex and index data
	// All levels of terrain patch will be using exactly the 
	// same set of vertices, but with different scales
	init_vert();

	/*
 	 * Create the earth
	 */
	hsys_.reset(new HTileSys);
}

Terrain::~Terrain()
{
	qDeleteAll( children );  //  deletes all the values stored in "children"
	children.clear();        //  removes all items from children

	for (int i = 0; i < 3; i++)
		decalmap[i].reset();
	heightmap.reset();
	noisemap.reset();
	waterCaustics.reset();
}

void Terrain::init_gl_resource()
{
	// Load terrain texture from file
	QImage decal_dirt("../textures/terrain/decal_dirt.jpg");
	QImage decal_grass("../textures/terrain/decal_grass.jpg");
	QImage decal_snow("../textures/terrain/decal_snow.jpg");
	QImage decal_noise("../textures/terrain/noisy_terrain.jpg");
	QImage alpha_caustics("../textures/ocean/caustics.jpg");

	size = 2048;        // put it here temporarily

	// Check whether texture are loaded
	if (decal_dirt.isNull() ||
	    decal_grass.isNull() ||
	    decal_snow.isNull() ||
	    decal_noise.isNull() ||
	    alpha_caustics.isNull()) {
		qDebug() << "Decal/Height map for terrain has not been found!";
		exit(-1);
	}
	decalmap[0].reset(new QOpenGLTexture(decal_grass));
	decalmap[1].reset(new QOpenGLTexture(decal_dirt));
	decalmap[2].reset(new QOpenGLTexture(decal_snow));
	noisemap.reset(new QOpenGLTexture(decal_noise));
	waterCaustics.reset(new QOpenGLTexture(alpha_caustics));

	for (auto i : boost::irange(0, 3)) {
		decalmap[i]->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
		decalmap[i]->setMagnificationFilter(QOpenGLTexture::Linear);
	}

	heightmap.reset(new QOpenGLTexture(QOpenGLTexture::Target2D));
	heightmap->setMinificationFilter(QOpenGLTexture::Linear);
	heightmap->setMagnificationFilter(QOpenGLTexture::Linear);
	heightmap->setWrapMode(QOpenGLTexture::Repeat);

	noisemap->setMinificationFilter(QOpenGLTexture::Linear);
	noisemap->setMagnificationFilter(QOpenGLTexture::Linear);
	noisemap->setWrapMode(QOpenGLTexture::Repeat);

	waterCaustics->setMinificationFilter(QOpenGLTexture::Linear);
	waterCaustics->setMagnificationFilter(QOpenGLTexture::Linear);
	waterCaustics->setWrapMode(QOpenGLTexture::Repeat);
}

void Terrain::init_vert()
{
	float size = 1.0 / grid;
	for (int j = -1; j <= grid + 1; j++) {
		for (int i = -1; i <= grid + 1; i++) {
			vertices << QVector3D(i * size, 0, j * size);
		}
	}
	for (int j = 0; j < grid + 1; j++) {
		for (int i = 0; i < grid + 1; i++) {
			// Upper triangle
			indices << (j * (grid + 3) + i + 1)
				<< (j * (grid + 3) + i) 
				<< ((j + 1) * (grid + 3) + i);

			// Bottom triangle
			indices << ((j + 1) * (grid + 3) + i) 
				<< ((j + 1) * (grid + 3) + i + 1)
				<< (j * (grid + 3) + i + 1); 
		}
	}
}

void Terrain::updatePatches() {
	Camera* camera = dynamic_cast<Scene*>(parent)->getCamera();
	QVector3D cameraPos = camera->getPosition();

	int far = qNextPowerOfTwo(int(camera->getFar()));
	int xfar = qCeil(size * M_PI);
	int zfar = qCeil(size * M_PI / 2);
	int size = int(ranges[levels].first);
	// int xnum = qCeil(xfar / size);
	// int znum = qCeil(zfar / size);
	int xnum = qCeil(far / size);
	int znum = qCeil(far / size);

	int x = qRound(cameraPos.x() / size) * size;
	int y = qRound(cameraPos.z() / size) * size;
	// int x = 0;
	// int y = 0;

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

void Terrain::selectPatches(Camera &camera, QVector3D &cameraPos)
{
	hsys_->upload_heightmap(heightmap.get(), camera);

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
	heightmap->bind(1);
	decalmap[0]->bind(2);
	decalmap[1]->bind(3);
	decalmap[2]->bind(4);
	noisemap->bind(5);
	waterCaustics->bind(6);

	program.setUniformValue("uUnderWaterCull", underWaterCull);
	if (underWaterCull) program.setUniformValue("uCullPlane", waterPlane);

	int heightLocation = program.uniformLocation("uHeightmap");
	int decalLocation0 = program.uniformLocation("uDecalmap0");
	int decalLocation1 = program.uniformLocation("uDecalmap1");
	int decalLocation2 = program.uniformLocation("uDecalmap2");
	int noiseLocation = program.uniformLocation("uNoisemap");
	int causticsLocation = program.uniformLocation("uWaterCaustics");
	program.setUniformValue(heightLocation, 1);
	program.setUniformValue(decalLocation0, 2);
	program.setUniformValue(decalLocation1, 3);
	program.setUniformValue(decalLocation2, 4);
	program.setUniformValue(noiseLocation, 5);
	program.setUniformValue(causticsLocation, 6);
	program.setUniformValue("uGrid", float(grid));

	Camera* camera = dynamic_cast<Scene*>(parent)->getCamera();
	QVector3D cameraPos = camera->getPosition();
	program.setUniformValue("uCamera", cameraPos);

	float elapsedTime = float(time.elapsed()) / 1e3;
	program.setUniformValue("uTime", elapsedTime);

	CHECK_GL_ERROR("after sets uniforms");
}

void Terrain::update() {
	updatePatches();
}

void Terrain::initialize() {
	this->setShader(QOpenGLShader::Vertex, "../glsl/terrain.vert");
	this->setShader(QOpenGLShader::Geometry, "../glsl/terrain.geom");
	this->setShader(QOpenGLShader::Fragment, "../glsl/terrain.frag");
	SceneObject::initialize();
	time.start();
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
		CHECK_GL_ERROR("After Terrian bind uOffset");
		program.setUniformValue("uScale", scale);
		CHECK_GL_ERROR("After Terrian bind uScale");
		program.setUniformValue("uLevel", patch->level);
		CHECK_GL_ERROR("After Terrian bind uLevel");
		QPair<double, double> range = ranges[patch->level];
		QVector2D ranges = QVector2D(range.first, range.second);
		program.setUniformValue("uRange", ranges);

		// draw all the triangles
		glDrawElements(GL_TRIANGLES, 
				indices.size(),		// count
				GL_UNSIGNED_INT,	// type
				0);			// element array buffer offset
		CHECK_GL_ERROR("After Terrian draws");
	} // end of for loop
	program.release();
}
