#ifndef TEXTURESKYDOME_H_
#define TEXTURESKYDOME_H_

#include <QString>
#include <QImage>
#include <QOpenGLTexture>

#include <SceneObject.h>
#include <Scene.h>

using std::unique_ptr;

class TextureSkyDome : public SceneObject {
public:
    TextureSkyDome(GraphicsDevice *device, int grid, Scene *scene);

    virtual ~TextureSkyDome ();

    void update();

    void uniform();

    void render();

private:
    unique_ptr<QOpenGLTexture> decalmap;
};

#endif /* end of include guard: TEXTURESKYDOME_H_ */
