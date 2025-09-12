#pragma once


#include <cmath>

// struct Vector3
// {
//     float x, y, z;
// };
struct Vector3;

struct Vector2
{
public:
    float x, y;

    // Vector2() : x(0), y(0) {}
    Vector2(float value) : x(value), y(value) {}
    Vector2(float x, float y) : x(x), y(y) {}

    void normalize()
    {
        float length = std::sqrt(x * x + y * y);
        if (length != 0)
        {
            x /= length;
            y /= length;
        }
    }

    operator Vector3() const;

    Vector2 operator+(const Vector2& vecOther) const { return Vector2(x + vecOther.x, y + vecOther.y); }
    Vector2 operator+(const Vector3& vecOther) const;

    Vector2& operator+=(const Vector2& vecOther) { x += vecOther.x; y += vecOther.y; return *this; }
    Vector2& operator+=(const Vector3& vecOther);

    Vector2 operator-(const Vector2& vecOther) const { return Vector2(x - vecOther.x, y - vecOther.y); }
    Vector2 operator-(const Vector3& vecOther) const;

    Vector2 operator*(float fMultiplier) const { return Vector2(x * fMultiplier, y * fMultiplier); }
    Vector2& operator*=(float fMultiplier) { x *= fMultiplier; y *= fMultiplier; return *this; }

    Vector2 operator/(float fMultiplier) const { return Vector2(x / fMultiplier, y / fMultiplier); }
    Vector2& operator/=(float fMultiplier) { x /= fMultiplier; y /= fMultiplier; return *this; }
};

struct Vector3
{
public:
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float value) : x(value), y(value), z(value) {}
    Vector3(float x, float y) : x(x), y(y), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

    void normalize()
    {
        float length = std::sqrt(x * x + y * y + z * z);
        if (length != 0)
        {
            x /= length;
            y /= length;
            z /= length;
        }
    }

    operator Vector2() const
    {
        return Vector2(x, y);
    }

    Vector3 operator+(const Vector2& vecOther) const { return Vector3(x + vecOther.x, y + vecOther.y, z); }
    Vector3 operator+(const Vector3& vecOther) const { return Vector3(x + vecOther.x, y + vecOther.y, z + vecOther.z); }

    Vector3& operator+=(const Vector2& vecOther) { x += vecOther.x; y += vecOther.y; return *this; }
    Vector3& operator+=(const Vector3& vecOther) { x += vecOther.x; y += vecOther.y; z += vecOther.z; return *this; }

    Vector3 operator-(const Vector2& vecOther) const { return Vector3(x - vecOther.x, y - vecOther.y, z); }
    Vector3 operator-(const Vector3& vecOther) const { return Vector3(x - vecOther.x, y - vecOther.y, z - vecOther.z); }

    Vector3& operator-=(const Vector2& vecOther) { x -= vecOther.x; y -= vecOther.y; return *this; }
    Vector3& operator-=(const Vector3& vecOther) { x -= vecOther.x; y -= vecOther.y; z -= vecOther.z; return *this; }

    Vector3 operator*(float fMultiplier) const { return Vector3(x * fMultiplier, y * fMultiplier, z * fMultiplier); }
    Vector3& operator*=(float fMultiplier) { x *= fMultiplier; y *= fMultiplier; z *= fMultiplier; return *this; }

    Vector3 operator/(float fMultiplier) const { return Vector3(x / fMultiplier, y / fMultiplier, z / fMultiplier); }
    Vector3& operator/=(float fMultiplier) { x /= fMultiplier; y /= fMultiplier; z /= fMultiplier; return *this; }
};
