#include <algorithm>

#include <QGuiApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>
#include <QtGui/QOpenGLVertexArrayObject> 
#include <QtGlobal>

#include "RenderThread.h"
#include "nexus.h"

#if 0
#include "Terrain.h"
#include "Ocean.h"
#include "TextureSkyDome.h"
#include "Grass.h"
#include "Scene.h"
#endif

RenderManager *RenderThread::renderMgr = nullptr;

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

#if 0
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
#endif
    if (renderMgr) { 
        renderMgr->start();
        renderMgr->prepare(); 
    }
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

    /* This is older version code that hard coded
     * the scene inside the render thread. We consider
     * a more flexible framework by render manager
     * */
#if 0
	// Actual rendering procedure
	// renderfbo_->bind();
	scene_->renderScene(renderfbo_.get());
    ctx_->functions()->glFlush();
    // renderfbo_->release();
#endif
    
    if (renderMgr) {
        renderMgr->beforeRender();
        renderMgr->render(renderfbo_.get());
    }

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
#if 0
	scene_.reset();
	terrian_.reset();
#endif
    if (renderMgr) { renderMgr->shutdown(); }

    ctx_->doneCurrent();
	ctx_.reset();

	surface_->deleteLater();
	exit();
	moveToThread(QGuiApplication::instance()->thread());
}

/*
 * Step 4: connect signal to slot
 */
void RenderThread::setup_ui_signals(QObject* ui)
{
#if 0
	connect(ui,
		SIGNAL(cameramove(qreal, qreal, qreal)), // Identical name as in main.qml. Note: real->qreal
		this,
		SLOT(camera_move(qreal, qreal, qreal)),
		Qt::QueuedConnection); // Must be QueuedConnection
#endif
    // Connect for key events
	connect(ui,
		SIGNAL(keypressed(int, int, int, QString)), // Identical name as in main.qml. Note: real->qreal
		this,
		SLOT(keyPressed(int, int, int, QString)),
		Qt::QueuedConnection); // Must be QueuedConnection

	connect(ui,
		SIGNAL(keyreleased(int, int, int, QString)), // Identical name as in main.qml. Note: real->qreal
		this,
		SLOT(keyReleased(int, int, int, QString)),
		Qt::QueuedConnection); // Must be QueuedConnection

    // Connect for mouse events
	connect(ui,
		SIGNAL(mouseclicked(int, int, bool, int, int)), // Identical name as in main.qml. Note: real->qreal
		this,
		SLOT(mouseClicked(int, int, bool, int, int)),
		Qt::QueuedConnection); // Must be QueuedConnection

	connect(ui,
		SIGNAL(mousedragstarted(int, int)), // Identical name as in main.qml. Note: real->qreal
		this,
		SLOT(mouseDragStarted(int, int)),
		Qt::QueuedConnection); // Must be QueuedConnection

	connect(ui,
		SIGNAL(mousedragfinished(int, int)), // Identical name as in main.qml. Note: real->qreal
		this,
		SLOT(mouseDragFinished(int, int)),
		Qt::QueuedConnection); // Must be QueuedConnection

	connect(ui,
		SIGNAL(mousedragging(int, int)), // Identical name as in main.qml. Note: real->qreal
		this,
		SLOT(mouseDragging(int, int)),
		Qt::QueuedConnection); // Must be QueuedConnection

	// Note: In theory we don't need to specify Qt::QueuedConnection because
	// Qt::connect will use QueuedConnection for object in different threads,
	// which is just our case. However I don't want to take the risk.
}

/*
 * Replaced with more general key event process functions
 * */
#if 0
void RenderThread::camera_move(qreal dx, qreal dy, qreal dz)
{
	// Simple handler.
	// fprintf(stderr, "%s %f %f %f\n", __func__, dx, dy, dz);
	scene_->getCamera()->move(QVector3D(dx, dy, dz));
}
#endif

void RenderThread::keyPressed(int count, int key, int modifiers, QString text) {
    // qDebug() << "Key Pressed" << text;
    if (renderMgr) renderMgr->keyPressed(count, key, modifiers, text);
}

void RenderThread::keyReleased(int count, int key, int modifiers, QString text) {
    // qDebug() << "Key Released" << text;
    if (renderMgr) renderMgr->keyReleased(count, key, modifiers, text);
}

void RenderThread::mouseClicked(int buttons, int modifiers, bool wasHeld, int x, int y) {
    if (renderMgr) renderMgr->mouseClicked(buttons, modifiers, wasHeld, x, y);
}

void RenderThread::mouseDragStarted(int x, int y) {
    if (renderMgr) renderMgr->mouseDragStarted(x, y);
}

void RenderThread::mouseDragging(int x, int y) {
    if (renderMgr) renderMgr->mouseDragging(x, y);
}

void RenderThread::mouseDragFinished(int x, int y) {
    if (renderMgr) renderMgr->mouseDragFinished(x, y);
}
