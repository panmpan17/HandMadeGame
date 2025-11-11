#pragma once

#include <linmath.h>
#include "math/vector.h"
#include "../components/component.h"

typedef unsigned int GLuint;


class Camera : public Component
{
public:
    static Camera* main;

    Camera();
    ~Camera();

    void onAddToNode() override;

    void useAsMain();
    inline bool isMainCamera() const { return main == this; }
    
    inline bool getUsingOrthoProjection() const { return m_bUseOrthoProjection; }
    inline void setUseOrthoProjection(bool bOrtho) 
    { 
        m_bUseOrthoProjection = bOrtho; 
        m_bProjectionMatrixDirty = true; 
        m_bViewProjectionMatrixDirty = true;
        m_bCameraUBODirty = true;
    }

    inline void setRatio(float fRatio)
    {
        m_fRatio = fRatio;
        m_bProjectionMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
        m_bCameraUBODirty = true;
    }

    inline void setWorldSizeScale(float fScale)
    {
        m_fWorldSizeScale = fScale;
        m_bProjectionMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
        m_bCameraUBODirty = true;
    }
    inline float getWorldSizeScale() const { return m_fWorldSizeScale; }

    inline bool isFitScreenWidth() const { return m_bFitScreenWidth; }
    inline void setFitScreenWidth(bool bFit)
    {
        m_bFitScreenWidth = bFit;
        m_bProjectionMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
        m_bCameraUBODirty = true;
    }

    const mat4x4& getViewProjectionMatrix();
    const mat4x4& getViewMatrix();
    const mat4x4& getProjectionMatrix();


    virtual bool isIDrawable() const override { return false; }
    virtual bool isUpdatable() const override { return false; }

    void onStart() override;

    void draw() override {}
    void update(float deltaTime) override {}

    void updateCameraDataBuffer();
    inline GLuint getCameraUBO() const { return m_nCameraUBO; }

    void markViewMatrixDirty();

private:
    bool m_bUseOrthoProjection = false;
    bool m_bFitScreenWidth = true;

    float m_fWorldSizeScale = 5;
    float m_fRatio = 5.0f;

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

    bool m_bCameraUBODirty = true;
    GLuint m_nCameraUBO = 0;
};
