#pragma once

#include <cmath>
#include <linmath.h>
#include "vector.h"


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

    static Quaternion fromEulerAngles(const Vector3& eulerAngles)
    {
        float cy = cos(eulerAngles.z / (180.0f / M_PI) * 0.5f);
        float sy = sin(eulerAngles.z / (180.0f / M_PI) * 0.5f);
        float cp = cos(eulerAngles.y / (180.0f / M_PI) * 0.5f);
        float sp = sin(eulerAngles.y / (180.0f / M_PI) * 0.5f);
        float cr = cos(eulerAngles.x / (180.0f / M_PI) * 0.5f);
        float sr = sin(eulerAngles.x / (180.0f / M_PI) * 0.5f);

        Quaternion q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;

        q.normalize();
        return q;
    }
    static Quaternion fromEulerAngles(float x, float y, float z)
    {
        float cy = cos(z * 0.5f);
        float sy = sin(z * 0.5f);
        float cp = cos(y * 0.5f);
        float sp = sin(y * 0.5f);
        float cr = cos(x * 0.5f);
        float sr = sin(x * 0.5f);

        Quaternion q;
        q.w = cr * cp * cy + sr * sp * sy;
        q.x = sr * cp * cy - cr * sp * sy;
        q.y = cr * sp * cy + sr * cp * sy;
        q.z = cr * cp * sy - sr * sp * cy;

        q.normalize();
        return q;
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

    Quaternion& operator*=(const Quaternion& q)
    {
        *this = *this * q;
        return *this;
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

    inline void getForwardVector(vec3 out) const
    {
        out[0] = -2.0f * (x * z + y * w);
        out[1] = -2.0f * (y * z - x * w);
        out[2] = -(1.0f - 2.0f * (x * x + y * y));
    }

    inline void getUpVector(vec3 out) const
    {
        out[0] = 2.0f * (x * y - z * w);
        out[1] = 1.0f - 2.0f * (x * x + z * z);
        out[2] = 2.0f * (y * z + x * w);
    }

    inline void getRightVector(vec3 out) const
    {
        out[0] = 1.0f - 2.0f * (y * y + z * z);
        out[1] = 2.0f * (x * y + z * w);
        out[2] = 2.0f * (x * z - y * w);
    }

    inline void toEulerAngles(Vector3& outEuler) const
    {
        // roll (x-axis rotation)
        float sinr_cosp = 2.0f * (w * x + y * z);
        float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
        outEuler.x = atan2f(sinr_cosp, cosr_cosp) * (180.0f / M_PI);

        // pitch (y-axis rotation)
        float sinp = 2.0f * (w * y - z * x);
        if (fabs(sinp) >= 1)
            outEuler.y = copysignf(M_PI / 2, sinp) * (180.0f / M_PI); // use 90 degrees if out of range
        else
            outEuler.y = asinf(sinp) * (180.0f / M_PI);

        // yaw (z-axis rotation)
        float siny_cosp = 2.0f * (w * z + x * y);
        float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
        outEuler.z = atan2f(siny_cosp, cosy_cosp) * (180.0f / M_PI);
    }
};


// Formatter specialization
template <>
struct std::formatter<Quaternion> : std::formatter<std::string> {
    auto format(const Quaternion& q, std::format_context& ctx) const {
        // Produce formatted text; use ctx.out() to write to output iterator
        return std::format_to(ctx.out(), "({}, {}, {}, {})", q.w, q.x, q.y, q.z);
    }
};

#ifndef QUATERNION_H_IMPLEMENTATION
#define QUATERNION_H_IMPLEMENTATION

// const Quaternion Quaternion::Identity = Quaternion(1, 0, 0, 0);

#endif // QUATERNION_H_IMPLEMENTATION
