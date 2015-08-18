#include <cstdio>
#include <iostream>

#include "../include/spica.h"
using namespace spica;

int main(int argc, char **argv) {
    std::cout << "*** spica: Bidirectional path tracing ***" << std::endl;

    const int width = argc >= 2 ? atoi(argv[1]) : 400;
    const int height = argc >= 3 ? atoi(argv[2]) : 300;
    const int samplePerPixel = argc >= 4 ? atoi(argv[3]) : 32;

    std::cout << "      width: " << width << std::endl;
    std::cout << "     height: " << height << std::endl;
    std::cout << "  sample/px: " << samplePerPixel << std::endl << std::endl;

    Scene scene;
    Camera camera;
    cornellBox(&scene, &camera, width, height);
    scene.computeLightPdfs();

    Timer timer;
    timer.start();
    BDPTRenderer renderer;
    renderer.render(scene, camera, samplePerPixel, PSEUDO_RANDOM_TWISTER);
    printf("Timer: %f sec\n", timer.stop());

    return 0;
}
