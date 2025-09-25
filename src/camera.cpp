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

const mat4x4& Camera::getViewMatrix()
{
    if (!m_bViewMatrixDirty)
    {
        return m_matViewCache;
    }

    // TODO: The order of the calculations might be wrong, could try mat4x4_look_at
    mat4x4_translate(m_matViewCache, m_position[0], m_position[1], m_position[2]);
    mat4x4_rotate_Z(m_matViewCache, m_matViewCache, (float) m_rotation[2]); // Rotate around Z-axis

    m_bViewMatrixDirty = false;

    return m_matViewCache;
}

const mat4x4& Camera::getProjectionMatrix()
{
    if (!m_bProjectionMatrixDirty)
    {
        return m_matProjectionCache;
    }

    if (m_bFitScreenWidth)
    {
        const float LEFT_CLIPPING_PLANE = -m_fWorldSizeScale;
        const float RIGHT_CLIPPING_PLANE = m_fWorldSizeScale;
        const float BOTTOM_CLIPPING_PLANE = -m_fWorldSizeScale / m_fRatio;
        const float TOP_CLIPPING_PLANE = m_fWorldSizeScale / m_fRatio;
        mat4x4_ortho(m_matProjectionCache, LEFT_CLIPPING_PLANE, RIGHT_CLIPPING_PLANE, BOTTOM_CLIPPING_PLANE, TOP_CLIPPING_PLANE, m_fNearPlane, m_fFarPlane);
    }
    else
    {
        const float LEFT_CLIPPING_PLANE = -m_fRatio * m_fWorldSizeScale;
        const float RIGHT_CLIPPING_PLANE = m_fRatio * m_fWorldSizeScale;
        const float BOTTOM_CLIPPING_PLANE = -m_fWorldSizeScale;
        const float TOP_CLIPPING_PLANE = m_fWorldSizeScale;
        mat4x4_ortho(m_matProjectionCache, LEFT_CLIPPING_PLANE, RIGHT_CLIPPING_PLANE, BOTTOM_CLIPPING_PLANE, TOP_CLIPPING_PLANE, m_fNearPlane, m_fFarPlane);
    }

    m_bProjectionMatrixDirty = false;

    return m_matProjectionCache;
}

const mat4x4& Camera::getViewProjectionMatrix()
{
    if (!m_bViewProjectionMatrixDirty && !m_bViewMatrixDirty && !m_bProjectionMatrixDirty)
    {
        return m_matViewProjectionCache;
    }

    const mat4x4& view = getViewMatrix();
    const mat4x4& proj = getProjectionMatrix();

    mat4x4_mul(m_matViewProjectionCache, proj, view);

    return m_matViewProjectionCache;
}
