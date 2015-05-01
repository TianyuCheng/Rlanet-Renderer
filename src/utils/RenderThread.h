#ifndef RENDER_THREAD_H
#define RENDER_THREAD_H

#include <memory>
#include <QtCore/QThread>

using std::unique_ptr;
class QOpenGLContext;
class QOffscreenSurface;
class QOpenGLFramebufferObject;
class QOpenGLVertexArrayObject;
class Scene;
class Terrain;
class Ocean;
class TextureSkyDome;
class Grass;

class RenderThread : public QThread {
	Q_OBJECT;
public slots:
	void render_next();
	void shutdown();
signals:
	void textureReady(int id, const QSize &size);

public:
	RenderThread();
	~RenderThread();
	bool ctx_ready() const { return !!ctx_; }
	bool fbo_ready() const { return !!renderfbo_; }
	void install_surface(QOffscreenSurface* surface);

	bool init_context(QOpenGLContext *root_ctx);
	bool init_fbos();
	bool init_renderer();
	QOpenGLContext* context() const { return ctx_.get(); }
private:
	unique_ptr<QOpenGLContext> ctx_;
	QOffscreenSurface *surface_; // QOffscreenSurface needs deleteLater
	unique_ptr<QOpenGLFramebufferObject> renderfbo_, displayfbo_;
	QSize size_;

	unique_ptr<Scene> scene_;
	unique_ptr<Terrain> terrian_;
	unique_ptr<Ocean> ocean_;
    unique_ptr<TextureSkyDome> skydome_;
    unique_ptr<Grass> grass_;
	unique_ptr<QOpenGLVertexArrayObject> vao_;
	bool shutdown_ = false;
};

#endif
