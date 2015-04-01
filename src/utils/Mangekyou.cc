#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>
#include <QQuickWindow>

#include "Mangekyou.h"
#include "RenderThread.h"

Mangekyou::Mangekyou()
{
	setFlag(ItemHasContents, true);
	thread_.reset(new RenderThread());
}

Mangekyou::~Mangekyou()
{
}

void Mangekyou::get_thread_ready()
{
	qDebug("GL context %p\n", thread_->context());
	QOffscreenSurface* surface = new QOffscreenSurface();
	surface->setFormat(thread_->context()->format());
	surface->create();
	surface->moveToThread(thread_.get());
	thread_->install_surface(surface);
	qDebug("Finishing %s\n", __func__);

	thread_->moveToThread(thread_.get());
	connect(window(), SIGNAL(sceneGraphInvalidated()),
			thread_.get(), SLOT(shutdown()), Qt::QueuedConnection);
	thread_->start();
	update();
}

#include "TextureNode.h"

QSGNode *Mangekyou::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
	TextureNode *node = static_cast<TextureNode *>(oldNode);

	if (!thread_->ctx_ready()) {
		QOpenGLContext *current = window()->openglContext();

		current->doneCurrent();
		thread_->init_context(current);
		current->makeCurrent(window());

		QMetaObject::invokeMethod(this, "get_thread_ready");
		return nullptr;
	}

	if (!node) {
		node = new TextureNode(window());

		connect(thread_.get(), SIGNAL(textureReady(int,QSize)),
			node, SLOT(newTexture(int,QSize)), Qt::DirectConnection);
		connect(node, SIGNAL(pendingNewTexture()),
			window(), SLOT(update()), Qt::QueuedConnection);
		connect(window(), SIGNAL(beforeRendering()),
			node, SLOT(prepareNode()), Qt::DirectConnection);
		connect(node, SIGNAL(textureInUse()),
			thread_.get(), SLOT(render_next()), Qt::QueuedConnection);

		// Get the production of FBO textures started..
		QMetaObject::invokeMethod(thread_.get(), "render_next", Qt::QueuedConnection);
	}
	node->setRect(boundingRect());
	return node;
}
