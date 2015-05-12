#ifndef HIERARCHICAL_TILE_SYSTEM_H
#define HIERARCHICAL_TILE_SYSTEM_H

#include <QVector3D>
#include <memory>

#include "ZearthFwd.h"

using std::unique_ptr;
class QOpenGLTexture;

class HTileSys {
public:
	HTileSys();
	~HTileSys();

	void upload_heightmap(QOpenGLTexture* tex, Camera& cam);
private:
	unique_ptr<Zearth> earth_;
	QVector3D last_pos_;
	bool init_ = false;
};

#endif
