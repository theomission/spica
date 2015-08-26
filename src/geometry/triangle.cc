#define SPICA_TRIANGLE_EXPORT
#include "triangle.h"

#include "../utils/common.h"
#include "../renderer/ray.h"

namespace spica {

    Triangle::Triangle()
        : _points()
    {
    }

    Triangle::Triangle(const Vector3D& p0, const Vector3D& p1, const Vector3D& p2)
        : _points()
    {
        _points[0] = p0;
        _points[1] = p1;
        _points[2] = p2;
    }

    Triangle::Triangle(const Triangle& tri)
        : _points(tri._points)
    {
    }

    Triangle::~Triangle()
    {
    }

    Triangle& Triangle::operator=(const Triangle& tri) {
        this->_points = tri._points;
        return *this;
    }

    Vector3D Triangle::get(int id) const {
        Assertion(0 <= id && id <= 2, "ID must be between 0 and 2");
        return _points[id];
    }

    Vector3D Triangle::operator[](int id) const {
        Assertion(0 <= id && id <= 2, "ID must be between 0 and 2");
        return _points[id];
    }

    Vector3D Triangle::normal() const {
        return Vector3D::cross(_points[1] - _points[0], _points[2] - _points[0]).normalized();
    }

    Vector3D Triangle::gravity() const {
        return (_points[0] + _points[1] + _points[2]) / 3.0;
    }

    bool Triangle::intersect(const Ray& ray, Hitpoint* hitpoint) const {
        Vector3D e1 = _points[1] - _points[0];
        Vector3D e2 = _points[2] - _points[0];
        Vector3D pVec = Vector3D::cross(ray.direction(), e2);
        double det = Vector3D::dot(e1, pVec);
        if (det > -EPS && det <EPS) return false;
        double invdet = 1.0 / det;
        
        Vector3D tVec = ray.origin() - _points[0];
        double u = Vector3D::dot(tVec, pVec) * invdet;
        if (u < 0.0 || u > 1.0) return false;

        Vector3D qVec = Vector3D::cross(tVec, e1);
        double v = Vector3D::dot(ray.direction(), qVec) * invdet;
        if (v < 0.0 || u + v > 1.0) return false;

        double t = Vector3D::dot(e2, qVec) * invdet;
        if (t > EPS) {
            hitpoint->setDistance(t);
            hitpoint->setPosition(ray.origin() + t * ray.direction());
            hitpoint->setNormal(this->normal());
            return true;
        }

        return false;
    }

    double Triangle::area() const {
        const Vector3D e1 = _points[1] - _points[0];
        const Vector3D e2 = _points[2] - _points[0];
        return 0.5 * Vector3D::cross(e1, e2).norm();
    }

    std::vector<Triangle> Triangle::triangulate() const {
        Assertion(false, "[WARNING] triangle do not have to be triangulated !!");
        return std::move(std::vector<Triangle>(1, *this));
    }

}  // namespace spica
