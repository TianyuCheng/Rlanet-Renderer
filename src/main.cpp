/**
 * main.cpp
 * */

#include <iostream>

#include <CEGUI/CEGUI.h>
#include <CEGUI/RendererModules/OpenGL/GLRenderer.h>

using namespace CEGUI;

int main(int argc, char *argv[])
{
    std::cout << "PlanetRenderer Starts" << std::endl; 

    CEGUI::OpenGLRenderer::bootstrapSystem();
    // CEGUI::OpenGLRenderer& myRenderer = CEGUI::OpenGLRenderer::bootstrapSystem();

    // WindowManager& wmgr = WindowManager::getSingleton();
    // Window* myRoot = wmgr.createWindow( "DefaultWindow", "root" );
    // System::getSingleton().getDefaultGUIContext().setRootWindow( myRoot );

    return 0;
}

