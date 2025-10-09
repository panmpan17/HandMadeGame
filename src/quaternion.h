#pragma once

#include <cmath>
#include <linmath.h>


struct Quaternion
{
public:
    // static const Quaternion Identity;

    static Quaternion fromAxisAngle(const vec3& axis, float angleDegrees)
    {
        float angleRadians = angleDegrees * (M_PI / 180.0f);
        float halfAngle = angleRadians / 2.0f;
        float s = sin(halfAngle);
        return Quaternion(cos(halfAngle), axis[0] * s, axis[1] * s, axis[2] * s);
    }

    float w, x, y, z;

    Quaternion() : w(1), x(0), y(0), z(0) {}
    Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

    Quaternion operator*(const Quaternion& q) const
    {
        // Right-hand rule multiplication
        Quaternion oNew = Quaternion(
            (w * q.w) - (x * q.x) - (y * q.y) - (z * q.z),
            (w * q.x) + (x * q.w) + (y * q.z) - (z * q.y),
            (w * q.y) - (x * q.z) + (y * q.w) + (z * q.x),
            (w * q.z) + (x * q.y) - (y * q.x) + (z * q.w)
        );
        oNew.normalize();

        return oNew;
    }

    void normalize()
    {
        float mag = sqrt(w * w + x * x + y * y + z * z);
        if (mag > 0.0f)
        {
            w /= mag;
            x /= mag;
            y /= mag;
            z /= mag;
        }
    }

    void toMat4x4(mat4x4 M) const
    {
        // Fill rotation matrix (column-major, as in linmath.h)
        M[0][0] = 1.0f - 2.0f * (y * y + z * z);
        M[0][1] = 2.0f * (x * y + z * w);
        M[0][2] = 2.0f * (x * z - y * w);
        M[0][3] = 0.0f;

        M[1][0] = 2.0f * (x * y - z * w);
        M[1][1] = 1.0f - 2.0f * (x * x + z * z);
        M[1][2] = 2.0f * (y * z + x * w);
        M[1][3] = 0.0f;

        M[2][0] = 2.0f * (x * z + y * w);
        M[2][1] = 2.0f * (y * z - x * w);
        M[2][2] = 1.0f - 2.0f * (x * x + y * y);
        M[2][3] = 0.0f;

        // last row for affine transform
        M[3][0] = 0.0f;
        M[3][1] = 0.0f;
        M[3][2] = 0.0f;
        M[3][3] = 1.0f;
    }

    void getForwardVector(vec3 out) const
    {
        out[0] = -2.0f * (x * z + y * w);
        out[1] = -2.0f * (y * z - x * w);
        out[2] = -(1.0f - 2.0f * (x * x + y * y));
    }

    void getUpVector(vec3 out) const
    {
        out[0] = 2.0f * (x * y - z * w);
        out[1] = 1.0f - 2.0f * (x * x + z * z);
        out[2] = 2.0f * (y * z + x * w);
    }
};


#ifndef QUATERNION_H_IMPLEMENTATION
#define QUATERNION_H_IMPLEMENTATION

// const Quaternion Quaternion::Identity = Quaternion(1, 0, 0, 0);

#endif // QUATERNION_H_IMPLEMENTATION
