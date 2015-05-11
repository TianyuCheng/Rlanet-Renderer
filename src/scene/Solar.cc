#include "Solar.h"

Solar::Solar(const QString& name,
	const QVector3D& pos, float radius,
	const QVector3D& amb,
	const QVector3D& diff,
	const QVector3D& spec)
	:SceneObject(name),
	pos_(pos),
	radius_(radius),
	amb_(amb),
	diff_(diff),
	spec_(spec)
{
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
	SET_UNIFORM(prog, "lightAmbient", amb_);
	SET_UNIFORM(prog, "lightDiffuse", diff_);
	SET_UNIFORM(prog, "lightSpecular", spec_);
}

void Solar::render()
{
}
