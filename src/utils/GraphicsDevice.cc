#include <GraphicsDevice.h>


GraphicsDevice::GraphicsDevice() : GLCore() {
    initializeOpenGLFunctions();
}

GraphicsDevice::~GraphicsDevice() {
}


