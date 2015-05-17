#ifndef OCEAN_H
#define OCEAN_H

#include <QTime>
#include <Terrain.h>

class Ocean : public Terrain {
public:
    Ocean (GraphicsDevice *device, int grid, int levels, Scene *parent);
    virtual ~Ocean ();

    void initialize();
    void uniform();

protected:
	virtual void init_gl_resource();
};

#endif /* end of include guard: OCEAN_H */
