#ifndef SOLAR_H
#define SOLAR_H

#include "SceneObject.h"

class Light {
public:
	virtual void apply_light(SceneObject* ) = 0;
};

class Solar : public Light, public SceneObject {
public:
	Solar(const QString& name,
		const QVector3D& pos,
		float radius,
		const QVector3D& amb,
		const QVector3D& diff,
		const QVector3D& spec
		);
	virtual void apply_light(SceneObject* );
	virtual void render();

private:
	QVector3D pos_;
	float radius_;
	QVector3D amb_;
	QVector3D diff_;
	QVector3D spec_;
};

#endif
