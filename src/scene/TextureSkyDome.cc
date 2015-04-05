#include "TextureSkyDome.h"

TextureSkyDome::TextureSkyDome(int grid, Scene *parent) 
    : SceneObject("Textued SkyDome", "../glsl/skydome.vert", "../glsl/skydome.frag", parent) 
{
    // Load texture
    QImage decal("../textures/decal_sky.jpg");
    decalmap.reset(new QOpenGLTexture(decal));
    decalmap->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    decalmap->setMagnificationFilter(QOpenGLTexture::Linear);

    // Generate vertex and index data
    {
        float size = 1.0 / grid;
        for (int j = 0; j <= grid; j++) {
            for (int i = 0; i <= grid; i++) {
                vertices << QVector3D(i * size, 0, j * size);
            }
        }
        for (int j = 0; j < grid; j++) {
            for (int i = 0; i < grid; i++) {
                // Upper triangle
                indices << (j * (grid + 1) + i + 1)
                    << (j * (grid + 1) + i) 
                    << ((j + 1) * (grid + 1) + i);

                // Bottom triangle
                indices << ((j + 1) * (grid + 1) + i) 
                    << ((j + 1) * (grid + 1) + i + 1)
                    << (j * (grid + 1) + i + 1); 
            }
        }
    }

}

TextureSkyDome::~TextureSkyDome() {

}

void TextureSkyDome::uniform() {
    // decalmap->bind(0);
    // int decalLocation = program.uniformLocation("uDecalmap");
    // program.setUniformValue(decalLocation, 0);
    // CHECK_GL_ERROR("after sets uniforms");
}
