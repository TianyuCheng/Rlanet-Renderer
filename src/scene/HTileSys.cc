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
	if (done_)
		return;
	QVector3D pos = cam.getPosition();
	Tile<TerrainTileInfo> target_tile(
			TerrainTileInfo(TileShape<float>(
					{0.0f, 0.0f},
					{512.0f, 512.0f},
					0.5)),
			0);
	earth_->blit_to(target_tile);
	intptr_t stride;
	float* data = target_tile.access_lod(0, 0, &stride);

#if 0
	for (int i = 0; i < 1024; i++)
		fprintf(stderr, "%f\t", data[i]);
	fprintf(stderr, "\n");
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

	done_ = true;
	last_pos_ = pos;
}
