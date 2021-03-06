#define SPICA_API_EXPORT
#include "pathtrace.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include "../core/common.h"
#include "../core/sampler.h"
#include "../core/image.h"
#include "../math/vector3d.h"

#include "../random/halton.h"
#include "../random/random_sampler.h"

#include "../scenes/scene.h"
#include "../light/lighting.h"

#include "renderer_helper.h"
#include "render_parameters.h"
#include "subsurface_integrator.h"

namespace spica {

    PathRenderer::PathRenderer()
        : IRenderer{} {
    }

    PathRenderer::~PathRenderer() {
    }

    void PathRenderer::render(const Scene& scene, const Camera& camera,
                              const RenderParameters& params) {
        const int width  = camera.imageW();
        const int height = camera.imageH();

        // Preparation for accouting for BSSRDF
        _integrator->initialize(scene);

        // Prepare random number generators
        auto samplers = std::vector<RandomSampler>(kNumThreads);
        for (int i = 0; i < kNumThreads; i++) {
            switch (params.randomType()) {
            case PSEUDO_RANDOM_TWISTER:
                samplers[i] = RandomSampler::useMersenne((unsigned int)time(0) + i);
                break;

            case QUASI_MONTE_CARLO:
                samplers[i] = RandomSampler::useHalton(200, true, (unsigned int)time(0) + i);
                break;

            default:
                std::cerr << "[ERROR] Unknown random number generator type!!"
                          << std::endl;
                std::abort();
            }
        }

        // Vectors spanning screen
        Image buffer = Image(width, height);

        // Distribute rendering tasks
        const int taskPerThread = (height + kNumThreads - 1) / kNumThreads;
        std::vector<std::vector<int> > tasks(kNumThreads);
        for (int y = 0; y < height; y++) {
            tasks[y % kNumThreads].push_back(y);
        }

        // Trace rays
        int processed = 0;
        _result.resize(width, height);
        buffer.fill(Color::BLACK);
        for (int i = 0; i < params.samplePerPixel(); i++) {
            if (i % kNumThreads == 0) {
                _integrator->construct(scene, params);
            }

            for (int t = 0; t < taskPerThread; t++) {
                ompfor (int threadID = 0; threadID < kNumThreads; threadID++) {
                    if (t < tasks[threadID].size()) {
                        Stack<double> rstk;
                        const int y = tasks[threadID][t];
                        for (int x = 0; x < width; x++) {
                            samplers[threadID].request(&rstk, 300);
                            buffer.pixel(width - x - 1, y) +=
                                tracePath(scene, camera, params, x, y, rstk);
                        }
                    }
                }
            }

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    _result.pixel(x, y) = buffer(x, y) / (i + 1);
                }
            }

            char filename[256];
            sprintf(filename, params.saveFilenameFormat().c_str(), i + 1);
            _result.gammaCorrect(1.0 / 2.2);
            _result.save(filename);

            printf("%6.2f %%  processed -> %s\r",
                   100.0 * (i + 1) / params.samplePerPixel(), filename);
        }
        printf("\nFinish!!\n");
    }

    Color PathRenderer::tracePath(const Scene& scene, const Camera& camera, 
                                  const RenderParameters& params,
                                  const double pixelX, const double pixelY,
                                  Stack<double>& rands) {
        CameraSample camSample = camera.sample(pixelX, pixelY, rands);
        const Ray ray = camSample.ray();

        return radiance(scene, params, ray, rands, 0) * (camera.sensitivity() / camSample.pdf());
    }

    Color PathRenderer::radiance(const Scene& scene,
                                const RenderParameters& params,
                                const Ray& ray, Stack<double>& rstack,
                                int bounces) const {
        if (bounces >= params.bounceLimit()) {
            return Color::BLACK;
        }

        Intersection isect;
        if (!scene.intersect(ray, &isect)) {
            return Color::BLACK;
        }

        // Require random numbers
        const double randnums[3] = { rstack.pop(), rstack.pop(), rstack.pop() };

        // Get intersecting material
        const int objectID     = isect.objectID();
        const BSDF& bsdf       = scene.getBsdf(objectID);
        const Color& refl      = isect.color();

        // Russian roulette
        double roulette = max3(refl.red(), refl.green(), refl.blue());
        if (bounces < params.bounceStartRoulette()) {
            roulette = 1.0;
        } else {
            if (roulette <= randnums[0]) {
                return Color::BLACK;
            }
        }

        // Variables for next bounce
        Color bssrdfRad(0.0, 0.0, 0.0);
        Vector3D nextdir;
        double pdf = 1.0;

        // Account for BSSRDF
        if (bsdf.type() & BsdfType::Bssrdf) {
            Assertion(_integrator != nullptr,
                      "Subsurface intergrator is NULL !!");
            bssrdfRad = bsdf.sampleBssrdf(ray.direction(),
                                          isect.position(),
                                          isect.normal(),
                                          randnums[1], randnums[2],
                                          *_integrator,
                                          &nextdir, &pdf);
        } else {
            // Sample next direction
            bsdf.sample(ray.direction(), isect.normal(), 
                        randnums[1], randnums[2], &nextdir, &pdf);
        }

        // Compute next bounce
        const Ray nextray(isect.position(), nextdir);
        const Color nextrad = radiance(scene, params, nextray,
                                       rstack, bounces + 1);            

        Color directrad = directSample(scene, objectID, ray.direction(),
                                       isect.position(), isect.normal(),
                                       refl, bounces, rstack);

        return (bssrdfRad + directrad + refl * nextrad / pdf) / roulette;
    }

    Color PathRenderer::directSample(const Scene& scene, const int triID,
                                     const Vector3D& in, const Vector3D& v,
                                     const Vector3D& n, const Color& refl, 
                                     int bounces, Stack<double>& rstk) const {
        // Acquire random numbers
        const double rands[2] = { rstk.pop(), rstk.pop() };

        const BSDF&  bsdf = scene.getBsdf(triID);

        if (bsdf.type() & BsdfType::Scatter) {
            // Scattering surface
            if (!scene.isLightCheck(triID)) {
                const LightSample ls = scene.sampleLight(rstk);
                double pdf;
                Vector3D nextdir;
                bsdf.sample(in, n, rands[0], rands[1], &nextdir, &pdf);
        
                const Vector3D lightDir = (ls.position() - v).normalized();
                const double dist2 = (ls.position() - v).squaredNorm();
                const double dot0  = Vector3D::dot(n, lightDir);
                const double dot1  = Vector3D::dot(ls.normal(), -lightDir);

                if (dot0 > EPS && dot1 > EPS) {
                    const double G = dot0 * dot1 / dist2;
                    Intersection isect;
                    if (scene.intersect(Ray(v, lightDir), &isect)) {
                        if (scene.isLightCheck(isect.objectID())) {
                            return (refl * ls.Le()) * (INV_PI * G * scene.lightArea()); 
                        }
                    }
                }
            } else if (bounces == 0) {
                return scene.directLight(in);
            }
        } else if (bsdf.type() & BsdfType::Dielectric) {
            // Dielectric surface
            double pdf;
            Vector3D nextdir;
            bsdf.sample(in, n, rands[0], rands[1], &nextdir, &pdf);
            
            Vector3D on = in.dot(n) < 0.0 ? n : -n;

            Intersection isect;
            if (scene.intersect(Ray(v, nextdir), &isect)) {
                if (scene.isLightCheck(isect.objectID())) {
                    if (bsdf.type() == BsdfType::Refractive) {
                        if(on.dot(nextdir) < 0.0) {
                            // printf("pdf = %f\n", pdf);   
                            // return {};
                        } else {
                            // printf("pdf = %f\n", pdf);                            
                        }
                    }
                    return (refl * scene.directLight(nextdir)) / pdf;
                }
            }
        } else {
            SpicaError("Invalid BSDF detected: this is "
                       "neigher scattering nor dielectric!!");
        }
        return Color(0.0, 0.0, 0.0);
    }

}  // namespace spica
