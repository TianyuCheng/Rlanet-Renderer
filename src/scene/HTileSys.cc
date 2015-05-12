#include <QOpenGLTexture>
#include "SelectGL.h"
#include "Camera.h"
#include "HTileSys.h"
#include "MacroTile.h"
#include "NoiseGenerator.h"

HTileSys::HTileSys()
{
	earth_.reset(new Zearth(TerrainTileInfo(TileShape<float>({0.0f, 0.0f},
				{4e6,4e6},
				4e6/1024.0)), 9527));
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
	Tile<TerrainComponoentMeta> target_tile(
			TerrainTileInfo(TileShape<float>(
					{pos.x() - 1e5, pos.z() - 1e5},
					{2e5, 2e5},
					2e5/1024.0)),
			0);
#endif
	earth_->blit_to(target_tile, GeoHeightExtractor());
	intptr_t stride;
	float* data = target_tile.access_lod(0, 0, &stride);

#if 1
	for (int i = 0; i < 1024; i++)
		fprintf(stderr, "%f\t", data[i]);
	fprintf(stderr, "\n");
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

	auto shape = target_tile.get_shape_info();
	tex->setSize(shape.nelem_in_line(), shape.nline());
	tex->create();
	tex->bind();
	CHECK_GL_ERROR("After resize and bind tex\n");
#if 0
	tex->setData(QOpenGLTexture::RGBA,
			//QOpenGLTexture::Red,
			QOpenGLTexture::UInt8,
		//QOpenGLTexture::Float32,
			data);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, shape.nelem_in_line(), shape.nline(),
			0, GL_RED, GL_FLOAT, data);
#endif
	CHECK_GL_ERROR("After set tex data\n");
#endif
	init_ = true;
	last_pos_ = pos;
}
