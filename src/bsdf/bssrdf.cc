#define SPICA_API_EXPORT
#include "bssrdf.h"

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include "../math/vector3d.h"
#include "../renderer/renderer_constants.h"

namespace spica {

    // ------------------------------------------------------------
    // BSSRDF base class
    // ------------------------------------------------------------

    double BSSRDFBase::Ft(const Vector3D& normal, const Vector3D& in) const {
        const double nnt = kIorObject / kIorVaccum;
        const double ddn = in.dot(normal);
        const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);

        if (cos2t < 0.0) return 0.0;

        Vector3D refractDir = (in * nnt + normal * (ddn * nnt + sqrt(cos2t))).normalized();

        const double a = kIorObject - kIorVaccum;
        const double b = kIorObject + kIorVaccum;
        const double R0 = (a * a) / (b * b);

        const double c  = 1.0 - Vector3D::dot(refractDir, -normal);
        const double Re = R0 + (1.0 - R0) * pow(c, 5.0);
        return 1.0 - Re;
    }

    double BSSRDFBase::Fdr() const {
        if (_eta >= 1.0) {
            return -1.4399 / (_eta * _eta) + 0.7099 / _eta + 0.6681 + 0.0636 * _eta;
        } else {
            return -0.4399 + 0.7099 / _eta - 0.3319 / (_eta * _eta) + 0.0636 / (_eta * _eta * _eta);
        }
    }

    // ------------------------------------------------------------
    // BSSRDF with dipole approximation
    // ------------------------------------------------------------

    DipoleBSSRDF::DipoleBSSRDF(const Color& sigma_a,
                               const Color& sigmap_s, double eta)
        : BSSRDFBase(eta)
        , _A(0.0)
        , _sigmap_t()
        , _sigma_tr()
        , _alphap()
        , _zpos()
        , _zneg()
    {
        _A = (1.0 + Fdr()) / (1.0 - Fdr());
        _sigmap_t = sigma_a + sigmap_s;
        _sigma_tr = Color::sqrt(3.0 * sigma_a * _sigmap_t);
        _alphap = sigmap_s / _sigmap_t;
        _zpos = Color(1.0, 1.0, 1.0) / _sigmap_t;
        _zneg = _zpos * (1.0 + (4.0 / 3.0) * _A);
    }

    DipoleBSSRDF::DipoleBSSRDF(const DipoleBSSRDF& bssrdf)
        : BSSRDFBase()
        , _A(0.0)
        , _sigmap_t()
        , _sigma_tr()
        , _alphap()
        , _zpos()
        , _zneg()
    {
        this->operator=(bssrdf);
    }

    DipoleBSSRDF& DipoleBSSRDF::operator=(const DipoleBSSRDF& bssrdf) {
        BSSRDFBase::operator=(bssrdf);
        this->_A = bssrdf._A;
        this->_sigmap_t = bssrdf._sigmap_t;
        this->_sigma_tr = bssrdf._sigma_tr;
        this->_alphap = bssrdf._alphap;
        this->_zpos = bssrdf._zpos;
        this->_zneg = bssrdf._zneg;
        return *this;
    }

    BSSRDF DipoleBSSRDF::factory(const Color& sigma_a, const Color& sigmap_s, double eta) {
        return BSSRDF(new DipoleBSSRDF(sigma_a, sigmap_s, eta));
    }

    Color DipoleBSSRDF::operator()(const Vector3D& v1, const Vector3D& v2) const {
        const double d2 = (v1 - v2).squaredNorm();
        const Color dpos = Color::sqrt(d2 + _zpos * _zpos);
        const Color dneg = Color::sqrt(d2 + _zneg * _zneg);
        const Color dpos3 = dpos * dpos * dpos;
        const Color dneg3 = dneg * dneg * dneg;
        const Color posTerm = _zpos * (dpos * _sigma_tr + 1.0) * Color::exp(-_sigma_tr * dpos);
        const Color negTerm = _zneg * (dneg * _sigma_tr + 1.0) * Color::exp(-_sigma_tr * dneg);
        const Color Rd = ((_alphap * posTerm * dneg3 - negTerm * dpos3) / (4.0 * PI * _sigma_tr * dpos3 * dneg3));
        return Rd.clamp();
    }

    BSSRDFBase* DipoleBSSRDF::clone() const {
        return new DipoleBSSRDF(*this);
    }

    // ------------------------------------------------------------
    // BSSRDF with diffuse reflectance function
    // ------------------------------------------------------------

    DiffuseBSSRDF::DiffuseBSSRDF()
        : BSSRDFBase()
        , _distances()
        , _colors()
    {
    }

    DiffuseBSSRDF::DiffuseBSSRDF(const double eta, const std::vector<double>& distances, const std::vector<Color>& colors)
        : BSSRDFBase(eta)
        , _distances(distances)
        , _colors(colors) {
        Assertion(distances.size() == colors.size(), "Arrays for distances and colors must have the same length!!");
    }

    DiffuseBSSRDF::DiffuseBSSRDF(const DiffuseBSSRDF& bssrdf)
        : BSSRDFBase()
        , _distances()
        , _colors() {
        this->operator=(bssrdf);
    }

    DiffuseBSSRDF::DiffuseBSSRDF(DiffuseBSSRDF&& bssrdf)
        : BSSRDFBase()
        , _distances()
        , _colors() {
        this->operator=(std::move(bssrdf));
    }

    DiffuseBSSRDF& DiffuseBSSRDF::operator=(const DiffuseBSSRDF& bssrdf) {
        BSSRDFBase::operator=(bssrdf);
        this->_distances = bssrdf._distances;
        this->_colors = bssrdf._colors;
        return *this;
    }

    DiffuseBSSRDF& DiffuseBSSRDF::operator=(DiffuseBSSRDF&& bssrdf) {
        BSSRDFBase::operator=(std::move(bssrdf));
        this->_distances = std::move(bssrdf._distances);
        this->_colors = std::move(bssrdf._colors);
        return *this;
    }

    BSSRDF DiffuseBSSRDF::factory(const double eta, const std::vector<double>& distances, const std::vector<Color>& colors) {
        return std::move(BSSRDF(new DiffuseBSSRDF(eta, distances, colors)));
    }

    BSSRDF DiffuseBSSRDF::factory() const {
        return std::move(BSSRDF(new DiffuseBSSRDF(*this)));
    }

    Color DiffuseBSSRDF::operator()(const Vector3D& v1, const Vector3D& v2) const {
        const double d = (v1 - v2).norm();
        if (d < 0.0 || d > _distances[_distances.size() - 1]) return Color(0.0, 0.0, 0.0);
        const int idx = std::lower_bound(_distances.begin(), _distances.end(), d) - _distances.begin();
        return _colors[idx];
    }

    BSSRDFBase* DiffuseBSSRDF::clone() const {
        return new DiffuseBSSRDF(*this);
    }

    int DiffuseBSSRDF::numIntervals() const {
        return static_cast<int>(_distances.size());
    }

    const std::vector<double>& DiffuseBSSRDF::distances() const {
        return _distances;
    }

    const std::vector<Color>& DiffuseBSSRDF::colors() const {
        return _colors;
    }

    DiffuseBSSRDF DiffuseBSSRDF::scaled(double sc) const {
        DiffuseBSSRDF ret(*this);
        for (size_t i = 0; i < ret._distances.size(); i++) {
            ret._distances[i] *= sc;
            ret._colors[i] /= (sc * sc);
        }
        return std::move(ret);
    }

    void DiffuseBSSRDF::save(const std::string& filename) const {
        std::ofstream ofs(filename.c_str(), std::ios::out | std::ios::binary);
        const int intervals = static_cast<int>(this->_distances.size());
        ofs.write((const char*)&intervals, sizeof(int));

        float ff[4];
        for (int i = 0; i < intervals; i++) {
            ff[0] = static_cast<float>(_distances[i]);        
            ff[1] = static_cast<float>(_colors[i].red());        
            ff[2] = static_cast<float>(_colors[i].green());        
            ff[3] = static_cast<float>(_colors[i].blue());        
            ofs.write((const char*)ff, sizeof(float) * 4);
        }

        ofs.close();
    }

    void DiffuseBSSRDF::load(const std::string& filename) {
        std::ifstream ifs(filename.c_str(), std::ios::in | std::ios::binary);
        Assertion(ifs.is_open(), "Faied to open file!!");

        int intervals;
        ifs.read((char*)&intervals, sizeof(int));

        _distances.resize(intervals);
        _colors.resize(intervals);

        float ff[4];
        for (int i = 0; i < intervals; i++) {
            ifs.read((char*)ff, sizeof(float) * 4);
            _distances[i] = ff[0];
            _colors[i] = spica::Color(ff[1], ff[2], ff[3]);
        }

        ifs.close();
    }

    // ------------------------------------------------------------
    // Custom BSSRDF interface
    // ------------------------------------------------------------

    CustomBSSRDF::CustomBSSRDF()
        : _ptr(nullptr) {
    }

    CustomBSSRDF::~CustomBSSRDF() {
        delete _ptr;
    }

    BSSRDF CustomBSSRDF::factory() {
        Assertion(_ptr != nullptr, "BSSRDF pointer is null!!");
        return BSSRDF(_ptr->clone());                      
    }

    // ------------------------------------------------------------
    // Abstract BSSRDF class
    // ------------------------------------------------------------

    BSSRDF::BSSRDF()
        : _ptr(nullptr) {
    }

    BSSRDF::BSSRDF(const BSSRDF& bssrdf)
        : _ptr(nullptr) {
        this->operator=(bssrdf);
    }

    BSSRDF::BSSRDF(BSSRDF&& bssrdf)
        : _ptr(nullptr) {
        this->operator=(std::move(bssrdf));
    }

    BSSRDF::BSSRDF(const BSSRDFBase* ptr)
        : _ptr(ptr) {
        Assertion(_ptr != nullptr, "Pointer is null!!");
    }

    BSSRDF::~BSSRDF() {
        delete _ptr;
    }

    BSSRDF& BSSRDF::operator=(const BSSRDF& bssrdf) {
        if (this == &bssrdf) {
            return *this;
        }

        delete _ptr;

        if (bssrdf._ptr == nullptr) {
            _ptr = nullptr;
        } else {
            _ptr = bssrdf._ptr->clone();
        }
        return *this;
    }

    BSSRDF& BSSRDF::operator=(BSSRDF&& bssrdf) {
        delete _ptr;

        this->_ptr = bssrdf._ptr;
        bssrdf._ptr = nullptr;
        return *this;
    }

    double BSSRDF::Ft(const Vector3D& normal, const Vector3D& in) const {
        nullCheck();
        return _ptr->Ft(normal, in);
    }

    double BSSRDF::Fdr() const {
        nullCheck();
        return _ptr->Fdr();
    }

    Color BSSRDF::operator()(const Vector3D& v1, const Vector3D& v2) const {
        nullCheck();
        return _ptr->operator()(v1, v2);
    }

    void BSSRDF::nullCheck() const {
        Assertion(_ptr != NULL, "BSSRDF does not have instance!!");
    }

}  // namespace spica
