#pragma once

#include <linmath.h>


class Camera
{
public:
    static Camera* main;

    Camera();
    ~Camera();

    inline void setRatio(float fRatio)
    {
        m_fRatio = fRatio;
        m_bViewMatrixDirty = true;
    }
    inline void setPosition(float fX, float fY)
    {
        m_position[0] = fX;
        m_position[1] = fY;
        m_bViewMatrixDirty = true;
    }
    inline void setPosition(float fX, float fY, float fZ)
    {
        m_position[0] = fX;
        m_position[1] = fY;
        m_position[2] = fZ;
        m_bViewMatrixDirty = true;
    }
    inline void move(float fX, float fY, float fZ)
    {
        m_position[0] += fX;
        m_position[1] += fY;
        m_position[2] += fZ;
        m_bViewMatrixDirty = true;
    }

    inline void setRotation(float fX, float fY, float fZ)
    {
        m_rotation[0] = fX;
        m_rotation[1] = fY;
        m_rotation[2] = fZ;
        m_bViewMatrixDirty = true;
    }
    inline void rotate(float fX, float fY, float fZ)
    {
        m_rotation[0] += fX;
        m_rotation[1] += fY;
        m_rotation[2] += fZ;
        m_bViewMatrixDirty = true;
    }
    inline void setWorldSizeScale(float fScale)
    {
        m_fWorldSizeScale = fScale;
        m_bViewMatrixDirty = true;
    }
    inline float getWorldSizeScale() const { return m_fWorldSizeScale; }

    const mat4x4& getViewProjectionMatrix();

private:
    float m_fWorldSizeScale = 5;
    float m_fRatio = 5.0f;
    vec3 m_position = {0.f, 0.f, 0.f};
    vec3 m_rotation = {0.f, 0.f, 0.f};
    float m_fNearPlane = 1.f;
    float m_fFarPlane = -1.f;

    bool m_bViewMatrixDirty = true;
    mat4x4 m_matViewMatrixCache;
};
