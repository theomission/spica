#ifdef _MSC_VER
#pragma once
#endif

#ifndef _SPICA_VBO_H_
#define _SPICA_VBO_H_

#include <vector>
#include <type_traits>

#include "../shape/shape.h"

extern void* enabler;

namespace spica {

    /** Vertex buffer object.
     */
    class SPICA_EXPORTS VBO {
    private:
        std::vector<float> _vertices;
        std::vector<float> _normals;
        std::vector<float> _colors;
        std::vector<unsigned int> _indices;

    public:
        /** The VBO constructor. */
        VBO();
        /** The VBO constructor (copy). */
        VBO(const VBO& vbo);
        /** The VBO destructor. */
        ~VBO();

        /** Assignment operator. */
        VBO& operator=(const VBO& vbo);

        void add(const Vector3D& v, const Vector3D& normal, const Color& color);

        template <class T, typename std::enable_if<std::is_base_of<IShape, T>::value>::type *& = enabler>
        void add(const T& shape, const Color& color);

        inline int numIndices() const { return (int)_indices.size(); }

        inline const float* vertices() const { return &_vertices[0]; }
        inline const float* normals() const { return &_normals[0]; }
        inline const float* colors() const { return &_colors[0]; }
        inline const unsigned int* indices() const { return &_indices[0]; }
    };

    template <class T, typename std::enable_if<std::is_base_of<IShape, T>::value>::type *&>
    void VBO::add(const T& shape, const Color& color) {        
        const std::vector<Triangle> tris = shape.triangulate();
        for (int i = 0; i < tris.size(); i++) {
            const Vector3D& normal = tris[i].normal();
            for (int j = 0; j < 3; j++) {
                add(tris[i][j], normal, color);
            }
        }
    }
}

#endif  // _SPICA_VBO_H_
