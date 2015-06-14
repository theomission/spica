#define SPICA_COLOR_EXPORT
#include "color.h"

namespace spica {

    Color::Color()
        : Vector3()
    {
    }

    Color::Color(double red, double green, double blue)
        : Vector3(red, green, blue)
    {
    }

    Color::Color(const Color& color)
        : Vector3()
    {
        operator=(color);
    }

    Color::Color(const Vector3& v)
        : Vector3(v)
    {
    }

    Color::~Color() 
    {
    }

    Color& Color::operator=(const Color& color) {
        Vector3::operator=(color);
        return *this;
    }

    Color& Color::operator=(const Vector3& v) {
        Vector3::operator=(v);
        return *this;
    }

    double Color::luminance() const {
        return 0.2126 * red() + 0.7152 * green() + 0.0722 * blue();
    }

}
