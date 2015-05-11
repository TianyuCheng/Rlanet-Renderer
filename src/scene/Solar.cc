#include "Solar.h"

Solar::Solar(const QString& name,
	float theta,
	float phi,
	float radius,
	const QVector3D& amb,
	const QVector3D& diff,
	const QVector3D& spec)
	:SceneObject(name),
	theta_(theta),
	phi_(phi),
	radius_(radius),
	amb_(amb),
	diff_(diff),
	spec_(spec)
{
	update_gloc();
}

#define SET_UNIFORM(prog, name, value) 			do {	\
	int uloc = prog->uniformLocation(name);			\
	if (uloc >= 0)						\
		prog->setUniformValue(uloc, value);		\
} while (0)

void Solar::apply_light(SceneObject* sobj)
{
	QOpenGLShaderProgram *prog = sobj->get_program();
	SET_UNIFORM(prog, "lightPos", pos_);
	SET_UNIFORM(prog, "lightPolar", QVector2D(theta_, phi_));
	SET_UNIFORM(prog, "lightAmbient", amb_);
	SET_UNIFORM(prog, "lightDiffuse", diff_);
	SET_UNIFORM(prog, "lightSpecular", spec_);
	SET_UNIFORM(prog, "solarRad", radius_);
}

void Solar::render()
{
}

void Solar::update_gloc()
{
	double rho = 9000.0;
	pos_ = QVector3D(rho * cos(theta_) * cos(phi_),
			rho * sin(phi_),
			rho * sin(theta_) * cos(phi_));
}
