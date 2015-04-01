#include <algorithm>

#include <QGuiApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>
#include <QtGlobal>

#include "RenderThread.h"
#include "Terrain.h"
#include "Scene.h"
#include "nexus.h"

RenderThread::RenderThread()
	:size_(600,400)
{
}

RenderThread::~RenderThread()
{
}

void RenderThread::install_surface(QOffscreenSurface* surface)
{
	surface_ = surface;
}

bool RenderThread::init_context(QOpenGLContext* parent_ctx)
{
	qDebug("Calling %s, parent %p\n", __func__, parent_ctx);
	ctx_.reset(new QOpenGLContext());
	ctx_->setFormat(parent_ctx->format()); // Do NOT call select_gl, RenderThread's ctx should be the same version as the parent one.
	ctx_->setShareContext(parent_ctx);
	if (!ctx_->create()) {
		qDebug("Failed to create context\n");
		return false;
	}
	ctx_->moveToThread(this);
	qDebug("Move ctx %p to thread %p\n", ctx_.get(), this);

	return true;
}

bool RenderThread::init_fbos()
{
	QOpenGLFramebufferObjectFormat fmt;
	fmt.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
	renderfbo_.reset(new QOpenGLFramebufferObject(size_, fmt));
	displayfbo_.reset(new QOpenGLFramebufferObject(size_, fmt));
	return true;
}

bool RenderThread::init_renderer()
{
	scene_.reset(new Scene(nexus::get_scene_name(), size_.width(), size_.height()));
	terrian_.reset(new Terrain(16, 10, scene_.get()));
	scene_->addObject(terrian_.get());
	return true;
}

void RenderThread::render_next()
{
	qDebug("Try to make current ctx %p on surface %p\n",
			ctx_.get(), surface_);
	ctx_->makeCurrent(surface_);
	qDebug("Successfully make current ctx %p on surface %p\n",
			ctx_.get(), surface_);
	
	if (!fbo_ready()) {
		init_fbos();
		init_renderer();
	}

	// Actual rendering procedure
	renderfbo_->bind();
	// scene_->start(); TODO
	scene_->renderScene();
        ctx_->functions()->glFlush();
        renderfbo_->release();

	// Double buffering support
	std::swap(renderfbo_, displayfbo_);
	emit textureReady(displayfbo_->texture(), size_);
}

void RenderThread::shutdown()
{
	/* Release context and FBOs */
        ctx_->makeCurrent(surface_);
	displayfbo_.reset();
	renderfbo_.reset();
        ctx_->doneCurrent();
	ctx_.reset();

	surface_->deleteLater();
	exit();
	moveToThread(QGuiApplication::instance()->thread());
}
