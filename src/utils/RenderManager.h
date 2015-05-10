#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <QTime>
#include <QQmlContext>
#include <QOpenGLFramebufferObject>

#include <Camera.h>

class RenderManager : public QObject
{
	Q_OBJECT;
public:
	RenderManager() { lastTime = 0; _fpsVal = 0; }
	virtual ~RenderManager() {}

	/**
	 * Do not override this function because the method
	 * is doing book-keeping for fps.
	 * */
	void beforeRender();

	virtual void prepare() = 0;
	virtual void render(QOpenGLFramebufferObject *) = 0;
	virtual void shutdown()  = 0;

	/**
	 * Please implement these methods to enable key controls
	 * */
	virtual void keyPressed(int count, int key, int modifiers, QString text) = 0;
	virtual void keyReleased(int count, int key, int modifiers, QString text) = 0;

	/**
	 * Please implement these methods to enable mouse controls
	 * */
	virtual void mouseClicked(int buttons, int modifiers, bool wasHeld, int x, int y) {}
	virtual void mouseDragStarted(int x, int y) {}
	virtual void mouseDragging(int x, int y) {}
	virtual void mouseDragFinished(int x, int y) {}

	int fps() { return _fpsVal; }

	void setContext(QQmlContext *c) { context = c; }
	void setContextProperty(QString key, QString val) {
		Q_ASSERT(context != nullptr);
		context->setContextProperty(key, val);
	}

	/**
	 * Time Utils
	 * Scene time control with start, pause, resume, and restart.
	 * As there is no pause and stop in the Qt, we fake it through
	 * subtracting the elapsed time right before paused.
	 * */
	void start();
	void pause();
	void resume();
	void restart();
	double timeElapsed() const;
	double getInterval() const;
private:
	int _fpsVal;
	int _fps;
	int _fpsCounter;

	QQmlContext *context;
	QTime time;
	int elapsedTime;
	int lastTime;
	int timeStep;
};


#endif /* end of include guard: RENDERMANAGER_H */
