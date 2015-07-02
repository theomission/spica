#include "gtest/gtest.h"

#include "../include/spica.h"
using namespace spica;

#include "test_macros.h"

namespace {
    
    bool trimeshIsectGT(const Trimesh& trimesh, const Ray& ray, Hitpoint* hitpoint) {
        bool ret = false;
        for (int i = 0; i < trimesh.numFaces(); i++) {
            Triangle tri = trimesh.getTriangle(i);
            Hitpoint hpTemp;
            if (tri.intersect(ray, &hpTemp)) {
                if (hitpoint->distance() > hpTemp.distance() && Vector3::dot(ray.direction(), tri.normal()) < 0.0) {
                    *hitpoint = hpTemp;
                    ret = true;
                }
            }
        }
        return ret;
    }

}

// ------------------------------
// Trimesh class test
// ------------------------------
TEST(TrimeshTest, InvalidLoad) {
    Trimesh trimesh;
    ASSERT_DEATH(trimesh.load(DATA_DIR + "box.obj"), "");
    ASSERT_DEATH(trimesh.load(DATA_DIR + "bax.ply"), "");
}

TEST(TrimeshTest, BoxIntersection) {
    Trimesh trimesh(DATA_DIR + "box.ply");
    trimesh.setAccelType(KD_TREE_ACCEL, true);

    Ray ray(Vector3(0.0, 0.0, 100.0), Vector3(0.0, 0.0, -1.0));
    Hitpoint hitpoint;
    EXPECT_TRUE(trimesh.intersect(ray, &hitpoint));
    EXPECT_EQ(99.5, hitpoint.distance());
    EXPECT_EQ_VEC(Vector3(0.0, 0.0, 0.5), hitpoint.position());
    EXPECT_EQ_VEC(Vector3(0.0, 0.0, 1.0), hitpoint.normal());
}

TEST(TrimeshTest, BunnyIntersection) {
    Trimesh trimesh(DATA_DIR + "bunny.ply");
    trimesh.setAccelType(KD_TREE_ACCEL, true);

    Ray ray(Vector3(0.0, 0.0, 100.0), Vector3(0.0, 0.0, -1.0));

    Hitpoint hpGT;
    bool isHit = trimeshIsectGT(trimesh, ray, &hpGT);

    Hitpoint hitpoint;
    EXPECT_EQ(isHit, trimesh.intersect(ray, &hitpoint));
    EXPECT_EQ(hpGT.distance(), hitpoint.distance());

    // Test copied data
    Trimesh cp(trimesh);
    EXPECT_EQ(trimesh.numVerts(), cp.numVerts());
    EXPECT_EQ(trimesh.numFaces(), cp.numFaces());
    for (int i = 0; i < cp.numFaces(); i++) {
        Triangle t1 = trimesh.getTriangle(i);
        Triangle t2 = cp.getTriangle(i);
        for (int k = 0; k < 3; k++) {
            EXPECT_EQ_VEC(t1.p(k), t2.p(k));
        }
        EXPECT_EQ_VEC(trimesh.getNormal(i), cp.getNormal(i));
    }

    EXPECT_EQ(isHit, cp.intersect(ray, &hitpoint));
    EXPECT_EQ(hpGT.distance(), hitpoint.distance());
}

TEST(TrimeshTest, RandomKdTreeIntersection) {
    const int nTrial = 100;
    Random rng = Random();

    Trimesh trimesh;
    trimesh.load(DATA_DIR + "bunny.ply");
    trimesh.setAccelType(KD_TREE_ACCEL, true);

    for (int i = 0; i < nTrial; i++) {
        Vector3 from  = Vector3(rng.nextReal(), rng.nextReal(), rng.nextReal()) * 20.0 - Vector3(10.0, 10.0, 0.0);
        Vector3 to    = Vector3(rng.nextReal(), rng.nextReal(), rng.nextReal()) * 20.0 - Vector3(10.0, 10.0, 10.0);
        Vector3 dir = (to - from).normalized();
        Ray ray(from, dir);

        Hitpoint ans;
        bool isHit = trimeshIsectGT(trimesh, ray, &ans);

        Hitpoint hitpoint;
        EXPECT_EQ(isHit, trimesh.intersect(ray, &hitpoint))
            << "  from: " << from.toString() << std::endl
            << "    to: " << to.toString()   << std::endl
            << "   pos: " << ans.position().toString() << std::endl
            << "normal: " << ans.normal().toString()   << std::endl;
        EXPECT_EQ(ans.distance(), hitpoint.distance());
    }
}

TEST(TrimeshTest, RandomQVBHIntersection) {
    const int nTrial = 100;
    Random rng = Random();

    Trimesh trimesh;
    trimesh.load(DATA_DIR + "bunny.ply");
    trimesh.setAccelType(QBVH_ACCEL, true);

    for (int i = 0; i < nTrial; i++) {
        Vector3 from  = Vector3(rng.nextReal(), rng.nextReal(), rng.nextReal()) * 20.0 - Vector3(10.0, 10.0, 0.0);
        Vector3 to    = Vector3(rng.nextReal(), rng.nextReal(), rng.nextReal()) * 20.0 - Vector3(10.0, 10.0, 10.0);
        Vector3 dir = (to - from).normalized();
        Ray ray(from, dir);

        Hitpoint ans;
        bool isHit = trimeshIsectGT(trimesh, ray, &ans);

        Hitpoint hitpoint;
        EXPECT_EQ(isHit, trimesh.intersect(ray, &hitpoint))
            << "  from: " << from.toString() << std::endl
            << "    to: " << to.toString()   << std::endl
            << "   pos: " << ans.position().toString() << std::endl
            << "normal: " << ans.normal().toString()   << std::endl;
        EXPECT_EQ(ans.distance(), hitpoint.distance());
    }
}