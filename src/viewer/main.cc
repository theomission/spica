#include <QtWidgets/qapplication.h>

#include "../scenes/predefined_scenes.h"
#include "scene_viewer.h"
using namespace spica;

int main(int argc, char** argv) {
    glutInit(&argc, argv);

    QApplication app(argc, argv);

    Scene scene;
    Camera camera;
    cornellBoxDragon(&scene, &camera, 640, 480);

    SceneViewer viewer;
    viewer.show();

    viewer.setScene(scene, camera);

    return app.exec();
}