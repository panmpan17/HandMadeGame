#pragma once

#include <linmath.h>
#include "vector.h"


class Camera
{
public:
    static Camera* main;

    Camera();
    ~Camera();

    inline void useAsMain() { main = this; }
    inline bool isMainCamera() const { return main == this; }
    
    inline bool isUsingOrthoProjection() const { return m_bUseOrthoProjection; }
    inline void setUseOrthoProjection(bool bOrtho) 
    { 
        m_bUseOrthoProjection = bOrtho; 
        m_bProjectionMatrixDirty = true; 
        m_bViewProjectionMatrixDirty = true;
    }

    inline void setRatio(float fRatio)
    {
        m_fRatio = fRatio;
        m_bViewMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
    }
    inline void getPosition(vec3& outPosition) const { vec3_dup(outPosition, m_vecPosition); }
    inline void setPosition(float fX, float fY)
    {
        m_vecPosition[0] = fX;
        m_vecPosition[1] = fY;
        m_bViewMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
    }
    inline void setPosition(float fX, float fY, float fZ)
    {
        m_vecPosition[0] = fX;
        m_vecPosition[1] = fY;
        m_vecPosition[2] = fZ;
        m_bViewMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
    }
    inline void setPosition(const Vector3& vec3)
    {
        m_vecPosition[0] = vec3.x;
        m_vecPosition[1] = vec3.y;
        m_vecPosition[2] = vec3.z;
        m_bViewMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
    }
    inline void move(float fX, float fY, float fZ)
    {
        m_vecPosition[0] += fX;
        m_vecPosition[1] += fY;
        m_vecPosition[2] += fZ;
        m_bViewMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
    }

    inline void getPointAt(vec3& outPointAt) const { vec3_dup(outPointAt, m_vecPointAt); }
    inline void setPointAt(float fX, float fY, float fZ)
    {
        m_vecPointAt[0] = fX;
        m_vecPointAt[1] = fY;
        m_vecPointAt[2] = fZ;
        m_bViewMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
    }

    inline void getVecUp(vec3& outVecUp) const { vec3_dup(outVecUp, m_vecUp); }
    inline void setVecUp(float fX, float fY, float fZ)
    {
        m_vecUp[0] = fX;
        m_vecUp[1] = fY;
        m_vecUp[2] = fZ;
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

    vec3 m_vecPosition = {0.f, 0.f, 2.5f};
    vec3 m_vecPointAt = {0.f, 0.f, 0.f};
    vec3 m_vecUp = {0.f, 1.f, 0.f};

    float m_fNearPlane = -3.f;
    float m_fFarPlane = 10.f;

    bool m_bViewMatrixDirty = true;
    mat4x4 m_matViewCache;
    bool m_bProjectionMatrixDirty = true;
    mat4x4 m_matProjectionCache;

    bool m_bViewProjectionMatrixDirty = true;
    mat4x4 m_matViewProjectionCache;
};
