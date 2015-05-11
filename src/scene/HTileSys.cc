#include <QOpenGLTexture>
#include "Camera.h"
#include "HTileSys.h"
#include "MacroTile.h"

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
	void* data = target_tile.access_lod(0, 0, &stride);

	auto shape = target_tile.get_shape_info();
	tex->setSize(shape.nelem_in_line(), shape.nline());
	tex->setData(QOpenGLTexture::Red,
		QOpenGLTexture::Float32,
			data);

	done_ = true;
	last_pos_ = pos;
}
