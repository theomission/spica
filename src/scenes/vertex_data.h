#ifdef _MSC_VER
#pragma once
#endif

#ifndef _SPICA_VERTEX_DATA_H_
#define _SPICA_VERTEX_DATA_H_

#include "../math/vector2d.h"
#include "../math/vector3d.h"
#include "../core/color.h"

namespace spica {

    class VertexData {
    private:
        Vector3D _pos;
        Color    _color;
        Vector3D _normal;
        Vector2D _texcoord;
        bool     _isTextured;

    public:
        VertexData()
            : _pos()
            , _color()
            , _normal()
            , _texcoord(INFTY, INFTY)
            , _isTextured(false) {
        }

        explicit VertexData(const Vector3D& pos,
                            const Color& color = Color(0.0, 0.0, 0.0),
                            const Vector3D& normal = Vector3D(0.0, 0.0, 0.0),
                            const Vector2D& texcoord = Vector2D(INFTY, INFTY))
            : _pos(pos)
            , _color(color)
            , _normal(normal)
            , _texcoord(texcoord)
            , _isTextured(false) {
            if (0.0 <= texcoord.x() && 0.0 <= texcoord.y() &&
                texcoord.x() <= 1.0 && texcoord.y() <= 1.0) {
                _isTextured = true;
            }
        }

        VertexData(const VertexData& v) 
            : _pos(v._pos)
            , _color(v._color)
            , _normal(v._normal)
            , _texcoord(v._texcoord)
            , _isTextured(v._isTextured) {
        }

        ~VertexData() {
        }
        
        VertexData& operator=(const VertexData& v) {
            this->_pos      = v._pos;
            this->_color    = v._color;
            this->_normal   = v._normal;
            this->_texcoord = v._texcoord;
            this->_isTextured = v._isTextured;
            return *this;
        }

        inline const Vector3D& pos() const { return _pos; }
        inline const Color& color() const { return _color; }
        inline const Vector3D& normal() const { return _normal; }
        inline const Vector2D& texcoord() const { return _texcoord; }
        inline bool isTextured() const { return _isTextured; }

        inline void setPosition(const Vector3D& pos) { _pos = pos; }
        inline void setColor(const Color& color) { _color = color; }
        inline void setNormal(const Vector3D& nrm) { _normal = nrm; }
        inline void setTexcoord(const Vector2D& texcoord) { _texcoord = texcoord; }
    };

}  // namespace spica

#endif  // _SPICA_VERTEX_DATA_H_
