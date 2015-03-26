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

    Terrain terrain("Test Terrain", 5, 128);
    scene.addObject(&terrain);

    w.setScene(&scene);

    return a.exec();
}
