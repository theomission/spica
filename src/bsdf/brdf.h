#ifdef _MSC_VER
#pragma once
#endif

#ifndef _SPICA_BRDF_H_
#define _SPICA_BRDF_H_

#include "../core/common.h"
#include "../math/vector3d.h"
#include "../core/color.h"

namespace spica {

    class BSDF;

    /** Interface for BSDF object 
     *  @ingroup bsdf_module
     */
    class SPICA_EXPORTS BSDFBase {
    protected:
        BSDFBase() {}
        explicit BSDFBase(const BSDFBase&) {}

    public:
        virtual ~BSDFBase() {}
        virtual const Color& reflectance() const = 0;
        virtual void sample(const Vector3D& in, const Vector3D& normal, double rand1, double rand2, Vector3D* out, double* pdf) const = 0;
        virtual BSDFBase* clone() const = 0;
    };


    /** Lambertian BRDF.
     *  @ingroup bsdf_module
     */
    class SPICA_EXPORTS LambertianBRDF : public BSDFBase {
    private:
        Color _reflectance;

    public:
        static BSDF factory(const Color& reflectance);
        const Color& reflectance() const override;
        void sample(const Vector3D& in, const Vector3D& normal, double rand1, double rand2, Vector3D* out, double* pdf) const override;
        BSDFBase* clone() const override;

    private:
        explicit LambertianBRDF(const Color& reflectance);
    };


    /** Specular BRDF.
     *  @ingroup bsdf_module
     */
    class SPICA_EXPORTS SpecularBRDF : public BSDFBase {
    private:
        Color _reflectance;

    public:
        static BSDF factory(const Color& reflectance);
        const Color& reflectance() const override;
        void sample(const Vector3D& in, const Vector3D& normal, double rand1, double rand2, Vector3D* out, double* pdf) const override;
        BSDFBase* clone() const override;

    private:
        explicit SpecularBRDF(const Color& reflectance);
    };


    /** Phong BRDF.
     *  @ingroup bsdf_module
     */
    class SPICA_EXPORTS PhongBRDF : public BSDFBase {
    private:
        Color _reflectance;
        double _coeff;

    public:
        static BSDF factory(const Color& reflectance, const double n = 32);
        const Color& reflectance() const override;
        void sample(const Vector3D& in, const Vector3D& normal, double rand1, double rand2, Vector3D* out, double* pdf) const override;
        BSDFBase* clone() const override;

    private:
        PhongBRDF(const Color& reflectance, const double n);
    };

    /** Refractive BSDF
     *  @ingroup bsdf_module
     */
    class SPICA_EXPORTS RefractiveBSDF : public BSDFBase {
    private:
        Color _reflectance;

    public:
        static BSDF factory(const Color& reflectance);
        const Color& reflectance() const override;
        void sample(const Vector3D& in, const Vector3D& normal, double rand1, double rand2, Vector3D* out, double* pdf) const override;
        BSDFBase* clone() const override;

    private:
        RefractiveBSDF(const Color& reflectance);
    };

}  // namespace spica

#endif  // _SPICA_BRDF_H_
