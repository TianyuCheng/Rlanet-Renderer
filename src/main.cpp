/**
 * main.cpp
 * */
#include <QApplication>  

#include "mainwindow.h"
#include "Scene.h"
#include "Planet.h"
#include "Terrain.h"

int main(int argc, char *argv[])  
{  
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    // OpenGL Context has been created
    Scene scene("Test Scene", 600, 400);

    // Planet planet("Test Object");
    // scene.addObject(&planet);

    Terrain terrain("Test Terrain");
    scene.addObject(&terrain);

    w.setScene(&scene);

    return a.exec();
}
