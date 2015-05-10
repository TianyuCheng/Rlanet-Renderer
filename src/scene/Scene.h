#ifndef SCENE_H
#define SCENE_H

#include <memory>
#include <QSize>
#include <QVector>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QMap>
#include <QTime>
#include <QImage>
#include <QOpenGLTexture>

#include <SceneObject.h>
#include <Camera.h>

class Scene : public SceneObject
{
public:
	Scene(QString name, QSize resolution);
	virtual ~Scene();

	/**
	 * Add object into the scene
	 * */
	void addObject(SceneObject* object);

	/**
	 * Render all visible objects to framebuffer.
	 *
	 * Output: current Framebuffer object
	 *
	 * Note: Currently it just draws all the objects in the 
	 * scene, but possible to optimize. 
	 *
	 * if fbo == nullptr, then we render it into scene's own
	 * fbo.
	 * */
	void renderScene(QOpenGLFramebufferObject *fbo = nullptr);

	/**
	 * Scene Update
	 * */
	void update() {}
	void uniform() {}

	/**
	 * Camera
	 * */
	void setCamera(Camera *cam) { camera = cam; }
	Camera* getCamera() { return camera; }

	void resize(QSize res);

	void discardTexture() { lastPass = -1; }
	void useTexture(GLuint texture) { lastPass = texture; }

	GLuint texture() const;
	GLuint takeTexture() const;

private:
	void init_fbo();

private:
	// Name for debugging and displaying
	QString name;
	QSize resolution;

	// Camera object encapsulation
	Camera *camera;

	// Framebuffer object
	unique_ptr<QOpenGLFramebufferObject> fbo_;

	// List of objects inside the scene
	QVector<SceneObject*> objects;

	bool fbo_ready;
	int lastPass;
};

#endif /* end of include guard: SCENE_H */
