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
        m_bViewProjectionMatrixDirty = true;
    }
    inline void getPosition(vec3& outPosition) const { vec3_dup(outPosition, m_position); }
    inline void setPosition(float fX, float fY)
    {
        m_position[0] = fX;
        m_position[1] = fY;
        m_bViewMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
    }
    inline void setPosition(float fX, float fY, float fZ)
    {
        m_position[0] = fX;
        m_position[1] = fY;
        m_position[2] = fZ;
        m_bViewMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
    }
    inline void move(float fX, float fY, float fZ)
    {
        m_position[0] += fX;
        m_position[1] += fY;
        m_position[2] += fZ;
        m_bViewMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
    }

    inline void setRotation(float fX, float fY, float fZ)
    {
        m_rotation[0] = fX;
        m_rotation[1] = fY;
        m_rotation[2] = fZ;
        m_bViewMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
    }
    inline void rotate(float fX, float fY, float fZ)
    {
        m_rotation[0] += fX;
        m_rotation[1] += fY;
        m_rotation[2] += fZ;
        m_bViewMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
    }
    inline void setWorldSizeScale(float fScale)
    {
        m_fWorldSizeScale = fScale;
        m_bProjectionMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
    }
    inline float getWorldSizeScale() const { return m_fWorldSizeScale; }

    inline bool isFitScreenWidth() const { return m_bFitScreenWidth; }
    inline void setFitScreenWidth(bool bFit)
    {
        m_bFitScreenWidth = bFit;
        m_bProjectionMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
    }

    const mat4x4& getViewProjectionMatrix();
    const mat4x4& getViewMatrix();
    const mat4x4& getProjectionMatrix();

private:
    bool m_bUseOrthoProjection = false;
    bool m_bFitScreenWidth = true;

    float m_fWorldSizeScale = 5;
    float m_fRatio = 5.0f;
    vec3 m_position = {0.f, 0.f, 0.f};
    vec3 m_rotation = {0.f, 0.f, 0.f};
    float m_fNearPlane = 3.f;
    float m_fFarPlane = -3.f;

    bool m_bViewMatrixDirty = true;
    mat4x4 m_matViewCache;
    bool m_bProjectionMatrixDirty = true;
    mat4x4 m_matProjectionCache;

    bool m_bViewProjectionMatrixDirty = true;
    mat4x4 m_matViewProjectionCache;
};
