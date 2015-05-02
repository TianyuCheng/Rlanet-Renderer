#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <QOpenGLFramebufferObject>

class RenderManager
{
public:
    RenderManager() {}
    virtual ~RenderManager() {}

    virtual void prepare() = 0;
    virtual void render(QOpenGLFramebufferObject *) = 0;
    virtual void shutdown()  = 0;

private:
    /* data */
};


#endif /* end of include guard: RENDERMANAGER_H */
