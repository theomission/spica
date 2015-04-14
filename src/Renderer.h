#ifndef RAINY_RENDERER_H_
#define RAINY_RENDERER_H_

#include <string>

#include "Ray.h"
#include "Random.h"
#include "material.h"

namespace rainy {

    class Renderer {
    private:
        int _width;              // Screen width
        int _height;             // Screen height
        int _samplePerPixel;     // # of samples / pixel
        int _supersamplePerAxis; // # of samples in pixel for an axis

        // --------------------
        // Constant
        // --------------------
        static const Color backgroundColor;
        static const int maxDepth;
        static const int depthLimit;

    public:
        explicit Renderer(int width = 512, int height = 512, int sample = 4, int supsample = 2);
        Renderer(const Renderer& renderer);
        ~Renderer();

        Renderer& operator=(const Renderer& renderer);

        void setSize(int width, int height);
        void setSamples(int sample, int supsample);

        int render();

    private:
        static Color radiance(const Ray& ray, Random& rng, const int depth);
        static void savePPM(std::string filename, Color* image, int width, int height);
    };

}

#endif  // RAINY_RENDERER_H_
