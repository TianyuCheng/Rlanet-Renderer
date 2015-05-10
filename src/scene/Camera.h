#ifndef CAMERA_H
#define CAMERA_H

#include <QPair>
#include <QtMath>
#include <QVector>
#include <QString>
#include <QVector3D>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QOpenGLShaderProgram>

#include <BoundingBox.h>

class Camera {
public:
	Camera(QString n);
	virtual ~Camera ();

	void initialize();

	/**
	 * ModelView matrix manipulatiuon
	 * */
	void setCenter(QVector3D center);
	void setEye(QVector3D eye);
	void setUp(QVector3D up);
	void lookAt(QVector3D eye, QVector3D center, QVector3D up);
	void moveForward(double distance);      // move towards the center
	void moveBackward(double distance);     // move away from center
	void move(QVector3D deye);
	void turnLeft(double angle, QVector3D axis = QVector3D(0, 1, 0));            // angle is in degree
	void turnRight(double angle, QVector3D axis = QVector3D(0, 1, 0));           // angle is in degree
	void lookUp(double angle);
	void lookDown(double angle);

	/**
	 * Projection matrix manipulation
	 * */
	void setLeft(double left);
	void setRight(double right);
	void setbottom(double bottom);
	void setTop(double top);
	void setNear(double near);
	void setFar(double far);
	void setFrustum(double left,    double right,
			double bottom,  double top,
			double near,    double far);

	void setFOV(double fov);        // this fov is only for vertical angle of view
	void setAspect(double aspect);  // this aspect is for horizontal
	void setPerspective(double fovy, double aspect, double zNear, double zFar);

	// reflect about plane
	void reflectCamera(QVector4D plane, Camera *cam);

	// GLSL related functions
	void uniformMatrices(QOpenGLShaderProgram &program);

	// Spatial optimization with cull
	enum Cullable { 
		TOTALLY_CULLABLE = 0,
		PARTIALLY_CULLABLE,
		NOT_CULLABLE
	};
	Cullable isCullable(BoundingBox box);

	// Getters
	QVector3D getPosition() const { return eye; }

	// Getting the frustum
	double getLeft() const { return left; }
	double getRight() const { return right; }
	double getTop() const { return top; }
	double getBottom() const { return bottom; }
	double getFar() const { return far; }
	double getNear() const { return near; }

private:
	/**
	 * Debugging functions
	 * */
	void modelviewInfo() {
		qDebug() << "Eye:" << eye;
		qDebug() << "Center:" << center;
		qDebug() << "Up:" << up;
		qDebug() << "ModelView:" << uMVMatrix;
	}

	void projectionInfo() {
		qDebug() << "Left:" << left;
		qDebug() << "Right:" << right;
		qDebug() << "Bottom:" << bottom;
		qDebug() << "Top:" << top;
		qDebug() << "Near:" << near;
		qDebug() << "Far:" << far;
		qDebug() << "Projection:" << uPMatrix;
	}

	void updateFrustum();

private:
	QString name;       // For debugging purpose

	bool dirty;         // for lazily generating frustum planes
	QVector< QPair<QVector3D, QVector3D> > frustum;

	// Camera position
	QVector3D eye;
	QVector3D center;
	QVector3D up;

	// look is computed using (center - eye)
	// It is not normalized because the length of 
	// look can be used for turn left/right
	QVector3D look;  

	// Viewing frustum
	double near, far;
	double left, right;
	double top, bottom;

	/**
	 * Replace the OpenGL's matrices.
	 * This requires all shaders for this engine
	 * to provide uniform variables for these three
	 * matrices.
	 */
	QMatrix4x4 uMVMatrix;   // model-view matrix (model -> view space)
	QMatrix4x4 uPMatrix;    // projection matrix (view -> projection space)
	QMatrix4x4 uNMatrix;    // normal matrix (inverse of transpose of model-view matrix)
};

#endif /* end of include guard: CAMERA_H */
