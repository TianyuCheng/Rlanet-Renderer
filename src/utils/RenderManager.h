#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <QTime>
#include <QOpenGLFramebufferObject>

class RenderManager
{
public:
    RenderManager() { lastTime = 0; }
    virtual ~RenderManager() {}

    void beforeRender();

    virtual void prepare() = 0;
    virtual void render(QOpenGLFramebufferObject *) = 0;
    virtual void shutdown()  = 0;

    int fps() { return _fpsVal; }

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

    QTime time;
    int elapsedTime;
    int lastTime;
    int timeStep;
};


#endif /* end of include guard: RENDERMANAGER_H */
