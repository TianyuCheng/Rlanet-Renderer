#include "Billboard.h"

Billboard::Billboard(BillboardFactory *f)
	: SceneObject(f->device, QString("Billboard"), f)
{
}

Billboard::~Billboard() {

}

void Billboard::placeBillboard(QVector2D coordinate, double height)
{
	vertices << QVector3D(coordinate.x(), coordinate.y(), height);
}

void Billboard::render()
{
	device->glEnable(GL_BLEND);
	device->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	vbo_.bind();
	program.setAttributeBuffer(vertexLocation_, GL_FLOAT, 0, 3);
	program.enableAttributeArray(vertexLocation_);
	CHECK_GL_ERROR(device, "After buffer");

	device->glDrawArrays(GL_POINTS, 0, vertices.size());
	CHECK_GL_ERROR(device, "After render");

	vbo_.release();

	device->glDisable(GL_BLEND);
}

BillboardFactory::BillboardFactory(GraphicsDevice *device, QString name) 
	: SceneObject(device, name) {
	}

BillboardFactory::~BillboardFactory() {
}

static double random_factor() // -1 to 1
{
	return -1.0 + 2.0 * drand48();
}

void BillboardFactory::createBillboard(Billboard *billboard,
		QVector2D center,
		double radius,
		double spacing,
		double size,
		double height,
		int seed,
		double chance
		)
{
	if (!seed)
		seed = clock();
	srand(seed);

	billboard->setSize(size);
	double cx = center.x();
	double cy = center.y();
	for (double x = -radius; x <= radius; x += spacing) {
		for (double y = -radius; y <= radius; y += spacing) {
			QVector2D point(x, y);
			// Check if the point is in the circle
			if (point.length() <= radius) {
				if (drand48() >= chance)
					continue;
				double rnd = ((rand() / (float)RAND_MAX) * 0.25 - 0.125) * height;
				billboard->placeBillboard(
					QVector2D(cx + x + spacing * 0.5 * random_factor(),
						cy + y + spacing * 0.5 * random_factor()),
					height + rnd);
			}
		}
	}
}
