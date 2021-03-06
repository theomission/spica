/** 
 * @defgroup light_module Light
 * @brief light
 */

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _SPICA_LIGHTING_H_
#define _SPICA_LIGHTING_H_

#include <memory>
#include <string>

#include "light_interface.h"

#include "../math/vector3d.h"
#include "../core/stack.h"
#include "../core/image.h"
#include "../shape/triangle.h"
#include "../shape/sphere.h"

namespace spica {

    enum class LightType {
        AreaLight = 0x01,
        Envmap    = 0x02
    };

    /** Lighting class
     * @ingroup light_module
     */
    class SPICA_EXPORTS Lighting {
    private:
        std::unique_ptr<ILight> _ptr;

    public:
        /** The Lighting constructor.
         */
        Lighting();

        /** The Lighting constructor (copy).
         */
        Lighting(const Lighting& l);

        /** The Lighting constructor (move)
         */
        Lighting(Lighting&& l);

        /** The lighting destructor.
         */
        ~Lighting();

        Lighting& operator=(const Lighting& l);
        Lighting& operator=(Lighting&& l);

        /** Initialize lighting as environment map (with image file name)
         */
        static Lighting asEnvmap(const Sphere& boundSphere, const std::string& filename);

        /** Initialize lighting as environment map (with image)
         */
        static Lighting asEnvmap(const Sphere& boundSphere, const Image& image);

        /** Initialize lighting as area light
         */
        static Lighting asAreaLight(const std::vector<Triangle>& triangles, const Color& emittance);

        LightSample sample(Stack<double>& rstack) const;
        Color directLight(const Vector3D& dir) const;
        double area() const;
    };

}  // namespace spica

#endif  // _SPICA_LIGHTING_H_
