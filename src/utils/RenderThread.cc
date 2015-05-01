#include <algorithm>

#include <QGuiApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>
#include <QtGui/QOpenGLVertexArrayObject> 
#include <QtGlobal>

#include "RenderThread.h"
#include "Terrain.h"
#include "Ocean.h"
#include "TextureSkyDome.h"
#include "Grass.h"
#include "Scene.h"
#include "nexus.h"

RenderThread::RenderThread()
	:size_(1024, 768)
{
	nexus::register_thread(this);
}

RenderThread::~RenderThread()
{
	qDebug("VAO ptr: %p", vao_.get());
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
	vao_.reset(new QOpenGLVertexArrayObject);
	vao_->create();
	vao_->bind();

	scene_.reset(new Scene(nexus::get_scene_name(), size_.width(), size_.height()));
	terrian_.reset(new Terrain(64, 15, scene_.get()));
	// ocean_.reset(new Ocean(64, 10, scene_.get()));
    skydome_.reset(new TextureSkyDome(64, scene_.get()));
    grass_.reset(new Grass(0x12345678));
    scene_->addObject(skydome_.get());
    scene_->addObject(grass_.get());
	scene_->addObject(terrian_.get());
	// scene_->addSecondPassObject(ocean_.get());
	scene_->first_frame();
	return true;
}

void RenderThread::render_next()
{
	if (shutdown_)
		return;
	//qDebug("Try to make current ctx %p on thread %p\n", ctx_.get(), QThread::currentThread());
	ctx_->makeCurrent(surface_);
	//qDebug("Successfully make current ctx %p on surface %p\n", ctx_.get(), surface_);
	
	if (!fbo_ready()) {
		init_fbos();
		init_renderer();
	}

	// Actual rendering procedure
	// renderfbo_->bind();
	scene_->renderScene(renderfbo_.get());
    ctx_->functions()->glFlush();
    // renderfbo_->release();

	// Double buffering support
	std::swap(renderfbo_, displayfbo_);
	emit textureReady(displayfbo_->texture(), size_);
}

void RenderThread::shutdown()
{
	shutdown_ = true;
	qDebug("%s Try to make current ctx %p on thread %p\n", __func__, ctx_.get(), QThread::currentThread());
	/* Release context and FBOs */
        ctx_->makeCurrent(surface_);
	displayfbo_.reset();
	renderfbo_.reset();
	vao_.reset();
	scene_.reset();
	terrian_.reset();
        ctx_->doneCurrent();
	ctx_.reset();

	surface_->deleteLater();
	exit();
	moveToThread(QGuiApplication::instance()->thread());
}
