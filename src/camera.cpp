#include "camera.h"
// #include <GLFW/glfw3.h>
#include "window.h"

#include <glad/gl.h>

Camera* Camera::main = nullptr;

Camera::Camera()
{
    glGenBuffers(1, &m_nCameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_nCameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4x4) * 2, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

    mat4x4_look_at(m_matViewCache, m_vecPosition, m_vecPointAt, m_vecUp);

    m_bViewMatrixDirty = false;

    return m_matViewCache;
}

const mat4x4& Camera::getProjectionMatrix()
{
    if (!m_bProjectionMatrixDirty)
    {
        return m_matProjectionCache;
    }

    if (m_bUseOrthoProjection)
    {
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
    }
    else
    {
        mat4x4_perspective(m_matProjectionCache, 1.57f, m_fRatio, 0.1f, 1000.0f);
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

void Camera::onStart()
{
    setRatio(Window::ins->getWindowRatio());
}

void Camera::updateCameraDataBuffer()
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_nCameraUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4x4), getViewMatrix());
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4x4), sizeof(mat4x4), getProjectionMatrix());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
