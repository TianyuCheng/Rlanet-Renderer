#include <QOpenGLTexture>
#include "SelectGL.h"
#include "Camera.h"
#include "HTileSys.h"
#include "MacroTile.h"
#include "NoiseGenerator.h"
#include "noiseutils.h"

HTileSys::HTileSys()
{
	earth_.reset(new Zearth(TerrainTileInfo(TileShape<float>({0.0f, 0.0f},
				{1048576,1048576},
				1024.0)), 1804289383));
}

HTileSys::~HTileSys()
{
}

void HTileSys::upload_heightmap(QOpenGLTexture* tex, Camera& cam)
{
	QVector3D pos = cam.getPosition();
#if 1
#if 0
	if (pos != last_pos_) {
		fprintf(stderr, "Camera pos (%f, %f, %f)\n",
				pos.x(),
				pos.y(),
				pos.z());
		last_pos_ = pos;
	}
#endif
	if (init_)
		return;
#endif
#if 0
	if (init_ &&
		(std::abs(last_pos_.x() - pos.x()) < 5e4 ||
		 std::abs(last_pos_.y() - pos.y()) < 5e4 ))
		return ;
	if (init_)
		return ;
#if 0
	Tile<TerrainComponoentMeta> target_tile(
			TerrainTileInfo(TileShape<float>(
					{0.0f, 0.0f}kk
					{512.0f, 512.0f},
					0.5)),
			0);
#else
	fprintf(stderr, "Camera pos (%f, %f, %f)\n",
			pos.x(),
			pos.y(),
			pos.z());
	float vsize = 16384.0*64;
#if 0
	Tile<TerrainComponoentMeta> target_tile(
			TerrainTileInfo(TileShape<float>(
					{pos.x() - vsize, pos.z() - vsize},
					{2*vsize, 2*vsize},
					vsize/1024.0)),
			0);
#else
	Tile<TerrainComponoentMeta> target_tile(
			TerrainTileInfo(TileShape<float>(
					{pos.x(), pos.z()},
					{vsize, vsize},
					4096)),
			0);
#endif
#endif
#if 0
	noise::module::Perlin perlin;
	perlin.SetSeed(1804289383);
	perlin.SetOctaveCount(6);
	perlin.SetFrequency(1.0);

	int width = 1024, height = 1024;

	noise::utils::NoiseMap noiseMap;
	noise::utils::NoiseMapBuilderSphere noiseMapBuilder;
	//noise::utils::NoiseMapBuilderPlane noiseMapBuilder;

	noiseMapBuilder.SetSourceModule(perlin);
	noiseMapBuilder.SetDestNoiseMap(noiseMap);
	noiseMapBuilder.SetDestSize (width, height);
	noiseMapBuilder.SetBounds (-90.0, 90.0, -90.0, 90.0);
	//noiseMapBuilder.SetBounds (0.0, 1024.0, 0.0, 1024.0);
	noiseMapBuilder.Build();

	float* data = noiseMap.GetSlabPtr();
	for (int i = 0; i < width*height; i++) {
		data[i] = 0.5 + 0.5 * data[i];
#if 0
		fprintf(stderr, "%f\t", data[i]);
		if (i % 16 == 15)
			fprintf(stderr, "\n");
#endif
#if 0
		if (data[i] > 1.0)
			data[i] = 1.0;
		else if (data[i] < 0.0)
			data[i] = 0.0;
#endif
	}
	tex->setSize(width, height);
	tex->create();
	tex->bind();
#else
	intptr_t stride;
	earth_->access_lod(0, 0, &stride);
	std::vector<GeoInfo>* geodata = earth_->access_lod_vec(0);
	//float* data = target_tile.access_lod(0, 0, &stride);
	std::vector<float> hmap;
	hmap.resize(geodata->size());
	for(size_t i = 0; i < hmap.size(); i++) {
		hmap[i] = (*geodata)[i].height * 0.5 + 0.5;
#if 0
		fprintf(stderr, "%f\t", hmap[i]);
		if (i % 16 == 15)
			fprintf(stderr, "\n");
#endif
	}
#if 0
	for (int i = 0; i < target_tile.get_shape_info().nelement(); i++) {
		data[i] = 0.51;
	}
	earth_->blit_to(target_tile, GeoHeightExtractor());
	auto shape = target_tile.get_shape_info();
#endif
	//float* data = target_tile.access_lod(0, 0, &stride);

	auto shape = earth_->get_shape_info();
	//for (int i = 0; i < target_tile.get_shape_info().nelement(); i++) {
	//	fprintf(stderr, "\n");
	int width = shape.nelem_in_line();
	int height = shape.nline();
	tex->setSize(width, height);
	tex->create();
	tex->bind();
	CHECK_GL_ERROR("After resize and bind tex\n");
	const float* data = hmap.data();
#if 0
	for (int i = 0; i < width*height; i++)
		data[i] = 0.5 + 0.5 * data[i];
#endif
#endif
#if 0
	int width = 32;
	int height = 32;
	int ow = shape.nelem_in_line();
	int oh = shape.nline();
	int mw = ow / width;
	int mh = oh / height;
	float newdata[width][height];
	for (int i = 0; i < target_tile.get_shape_info().nelement(); i++) {
		if (data[i] > 1.0)
			data[i] = 1.0;
		else if (data[i] < 0.0)
			data[i] = 0.0;
	}
	for(int i = 0; i < height; i++) {
		for(int j = 0; j < width; j++) {
			float sh = 0.0;
			for(int x = mh*i; x < mh*i+mh; x++) {
				for(int y = mw*j; y < mw*j+mw; y++) {
					sh += data[x*oh+y];
				}
			}
			newdata[i][j] = sh/mh/mw;
		}
	}
#endif
#if 1
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 
			width, height,
			//shape.nelem_in_line(), shape.nline(),
			0, GL_RED, GL_FLOAT,
			//data
			data
			);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 
			shape.nelem_in_line(), shape.nline(),
			0, GL_RED, GL_FLOAT,
			data
			);
#endif
#if 0
	for (int i = 0; i < shape.nelement(); i++) {
		fprintf(stderr, "%f\t", data[i]);
		if (i % 16 == 15)
			fprintf(stderr, "\n");
	}
#endif
#endif
#if 1
	// Generate heightmap using seed
	int r = 128;
	int w  = r * 2 * M_PI;
	int h = r * M_PI;

	QImage height(w, h, QImage::Format_RGB32);
	NoiseGenerator::SphericalHeightmap(height, r, clock());
	tex->setData(height);
#else

#if 0
	tex->setData(QOpenGLTexture::RGBA,
			//QOpenGLTexture::Red,
			QOpenGLTexture::UInt8,
		//QOpenGLTexture::Float32,
			data);
#else
#endif
	CHECK_GL_ERROR("After set tex data\n");
#endif
	init_ = true;
	last_pos_ = pos;
}
