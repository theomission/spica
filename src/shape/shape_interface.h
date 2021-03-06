/**
 * @defgroup shape_module Shapes
 * @brief Shape classes.
 */

#ifdef _MSC_VER
#pragma once
#endif

#ifndef SPICA_SHAPE_INTERFACE_H_
#define SPICA_SHAPE_INTERFACE_H_

#include <vector>

#include "../core/common.h"

namespace spica {
    
    class Ray;
    class Hitpoint;
    class Triangle;

    /** Shape types
     *  @ingroup shape_module
     *  @brief Enumerator for shape types.
     */
    enum class ShapeType : int {
        None,      /**< None         */
        BBox,      /**< Bounding Box */
        Disk,      /**< Disk         */
        Plane,     /**< Plane        */
        Quad,      /**< Quad         */
        Sphere,    /**< Sphere       */
        Triangle,  /**< Triangle     */
        Trimesh,   /**< Trimesh      */
    };
    
    /** Interface for shapes
     *  @ingroup shape_module
     */
    class SPICA_EXPORTS IShape {
    protected:
        ShapeType _type;

    public:
        IShape(ShapeType type) : _type{type} {}
        virtual ~IShape() {}
        virtual bool intersect(const Ray& ray, Hitpoint* hitpoint) const = 0;
        virtual double area() const = 0;
        virtual std::vector<Triangle> triangulate() const = 0;
        inline ShapeType type() const { return _type; }
    };

}  // namespace spica

#endif  // SPICA_SHAPE_INTERFACE_H_
