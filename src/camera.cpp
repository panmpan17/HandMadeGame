#include "camera.h"
// #include <GLFW/glfw3.h>

Camera* Camera::main = nullptr;

Camera::Camera()
{
    main = this;
}

Camera::~Camera()
{
}

const mat4x4& Camera::getViewProjectionMatrix()
{
    if (!m_bViewMatrixDirty)
    {
        return m_matViewMatrixCache;
    }

    mat4x4 matViewMatrix, matProjectionMatrix;

    mat4x4_translate(matViewMatrix, m_position[0], m_position[1], m_position[2]);
    mat4x4_rotate_Z(matViewMatrix, matViewMatrix, (float) m_rotation[2]); // Rotate around Z-axis

    if (m_bFitScreenWidth)
    {
        const float LEFT_CLIPPING_PLANE = -m_fWorldSizeScale;
        const float RIGHT_CLIPPING_PLANE = m_fWorldSizeScale;
        const float BOTTOM_CLIPPING_PLANE = -m_fWorldSizeScale / m_fRatio;
        const float TOP_CLIPPING_PLANE = m_fWorldSizeScale / m_fRatio;
        mat4x4_ortho(matProjectionMatrix, LEFT_CLIPPING_PLANE, RIGHT_CLIPPING_PLANE, BOTTOM_CLIPPING_PLANE, TOP_CLIPPING_PLANE, m_fNearPlane, m_fFarPlane);
    }
    else
    {
        const float LEFT_CLIPPING_PLANE = -m_fRatio * m_fWorldSizeScale;
        const float RIGHT_CLIPPING_PLANE = m_fRatio * m_fWorldSizeScale;
        const float BOTTOM_CLIPPING_PLANE = -m_fWorldSizeScale;
        const float TOP_CLIPPING_PLANE = m_fWorldSizeScale;
        mat4x4_ortho(matProjectionMatrix, LEFT_CLIPPING_PLANE, RIGHT_CLIPPING_PLANE, BOTTOM_CLIPPING_PLANE, TOP_CLIPPING_PLANE, m_fNearPlane, m_fFarPlane);
    }

    // constexpr float FOV_IN_RANGLES = 1.57f; // 90 degrees in radians
    // constexpr float NEAR_PLANE = 1.f;
    // constexpr float FAR_PLANE = 10.f;
    // mat4x4_perspective(p, FOV_IN_RANGLES, m_fRatio, NEAR_PLANE, FAR_PLANE);

    mat4x4_mul(m_matViewMatrixCache, matProjectionMatrix, matViewMatrix);

    m_bViewMatrixDirty = false;

    return m_matViewMatrixCache;
}
