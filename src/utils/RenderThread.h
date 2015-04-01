#ifndef RENDER_THREAD_H
#define RENDER_THREAD_H

#include <memory>
#include <QtCore/QThread>

using std::unique_ptr;
class QOpenGLContext;
class QOffscreenSurface;
class Scene;
class Terrain

class RenderThread : public QThread {
	Q_OBJECT;
public slots:
	void renderNext();
	void shutDown();
signals:
	void textureReady(int id, const QSize &size);

public:
	RenderThread();
	bool ctx_ready() const { return ctx_; }
	bool fbo_ready() const { return renderfbo_; }

	bool init_context(QOpenGLContext *root_ctx);
	bool init_fbos();
	bool init_renderer();
private:
	unique_ptr<QOpenGLContext> ctx_;
	QOffscreenSurface surface_; // QOffscreenSurface needs deleteLater
	unique_ptr<QOpenGLFramebufferObject> renderfbo_, displayfbo_;
	QSize size_;

	unique_ptr<Scene> scene_;
	unique_ptr<Terrain> terrian_;
};

#endif
