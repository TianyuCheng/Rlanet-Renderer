/**
 * main.cpp
 * */
#include <QApplication>  

#include "mainwindow.h"
#include "Scene.h"
#include "Planet.h"

int main(int argc, char *argv[])  
{  
    QApplication a(argc, argv);
    MainWindow w;

    w.show();

    // OpenGL Context has been created
    Scene scene;
    Planet planet("test object");
    scene.addObject(&planet);

    return a.exec();
}
