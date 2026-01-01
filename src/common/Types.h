#pragma once
#include <cmath>

// Simple 3D vector struct to keep physics code independent of VTK headers
struct Vec3 {
    double x, y, z;

    Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
    Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }
    double magnitudeSq() const { return x*x + y*y + z*z; }
};
