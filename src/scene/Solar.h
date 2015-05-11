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
		float theta,
		float phi,
		float radius,
		const QVector3D& amb,
		const QVector3D& diff,
		const QVector3D& spec
		);
	virtual void apply_light(SceneObject* );
	virtual void render();

	void update_polar(float theta, float phi) {
		theta_ = theta;
		phi_ = phi;
		update_gloc();
	}


private:
	QVector3D pos_;
	float theta_;
	float phi_;
	float radius_;
	QVector3D amb_;
	QVector3D diff_;
	QVector3D spec_;

	void update_gloc();
};

#endif
