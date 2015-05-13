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
				8192.0)), 180489383));
}

HTileSys::~HTileSys()
{
}

void HTileSys::upload_heightmap(QOpenGLTexture* tex, Camera& cam)
{
	QVector3D pos = cam.getPosition();
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
	if (init_ &&
		(std::abs(last_pos_.x() - pos.x()) < 5e4 ||
		 std::abs(last_pos_.y() - pos.y()) < 5e4 ))
		return ;

	intptr_t stride;
	earth_->access_lod(0, 0, &stride);
	std::vector<GeoInfo>* geodata = earth_->access_lod_vec(0);
	//float* data = target_tile.access_lod(0, 0, &stride);
	std::vector<float> hmap;
	hmap.resize(geodata->size());
	for(size_t i = 0; i < hmap.size(); i++) {
		hmap[i] = (*geodata)[i].height * 0.5 + 0.5;
	}
	const float *data = hmap.data();
	auto shape = earth_->get_shape_info();
	int width = shape.nelem_in_line();
	int height = shape.nline();
	tex->setSize(width, height);
	tex->create();
	tex->bind();

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 
			width, height,
			//shape.nelem_in_line(), shape.nline(),
			0, GL_RED, GL_FLOAT,
			//data
			data
			);
	init_ = true;
	last_pos_ = pos;
}
