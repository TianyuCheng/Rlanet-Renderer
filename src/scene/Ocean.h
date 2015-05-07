#ifndef OCEAN_H
#define OCEAN_H

#include <QTime>
#include <Terrain.h>

class Ocean : public Terrain
{
public:
    Ocean (int grid, int levels, Scene *parent);
    virtual ~Ocean ();

    void initialize();
    void uniform();
};

#endif /* end of include guard: OCEAN_H */
