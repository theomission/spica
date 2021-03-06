#include "gtest/gtest.h"

#include "../include/spica.h"
using namespace spica;

#include "test_macros.h"

TEST(RayTest, DefaultInstanceTest) {
    Ray ray;
    EXPECT_EQ_VEC(Vector3D{}, ray.origin());
    EXPECT_EQ_VEC(Vector3D{}, ray.direction());
    EXPECT_EQ_VEC(Vector3D(INFTY, INFTY, INFTY), ray.invdir());
}

TEST(RayTest, InstanceTest) {
    Vector3D orig(1.0, 2.0, 3.0);
    Vector3D dir(1.0, 0.0, 0.0);
    Ray ray(orig, dir);
    
    EXPECT_EQ_VEC(orig, ray.origin());
    EXPECT_EQ_VEC(dir, ray.direction());
    EXPECT_EQ_VEC(Vector3D(1.0, INFTY, INFTY), ray.invdir());

    ASSERT_DEATH(Ray(orig, Vector3D(1.0, 1.0, 1.0)), "");
}

TEST(RayTest, CopyTest) {
    Vector3D orig(1.0, 2.0, 3.0);
    Vector3D dir(1.0, 0.0, 0.0);
    Ray ray(orig, dir);

    Ray cp = ray;
    EXPECT_EQ_VEC(orig, cp.origin());
    EXPECT_EQ_VEC(dir, cp.direction());
    EXPECT_EQ_VEC(Vector3D(1.0, INFTY, INFTY), cp.invdir());
}
