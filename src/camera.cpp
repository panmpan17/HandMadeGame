#include "camera.h"
// #include <GLFW/glfw3.h>
#include "window.h"
#include "node.h"

#include "debug_macro.h"

#include <glad/gl.h>

Camera* Camera::main = nullptr;

Camera::Camera()
{
    glGenBuffers(1, &m_nCameraUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_nCameraUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4x4) * 2 + sizeof(vec4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Camera::~Camera()
{
}

void Camera::onAddToNode()
{
    if (!m_pNode)
    {
        return;
    }

    m_pNode->registerOnPositionChangedListener(std::bind(&Camera::markViewMatrixDirty, this));
    m_pNode->registerOnRotationChangedListener(std::bind(&Camera::markViewMatrixDirty, this));
}

void Camera::markViewMatrixDirty()
{
    m_bViewMatrixDirty = true;
    m_bViewProjectionMatrixDirty = true;
    m_bCameraUBODirty = true;
}

const mat4x4& Camera::getViewMatrix()
{
    if (!m_bViewMatrixDirty)
    {
        return m_matViewCache;
    }

    const Vector3& camPos = m_pNode->getPosition();
    vec3 camForward, camUp;
    m_pNode->getRotationQuaternion().getForwardVector(camForward);
    m_pNode->getRotationQuaternion().getUpVector(camUp);
    camForward[0] += camPos.x;
    camForward[1] += camPos.y;
    camForward[2] += camPos.z;
    mat4x4_look_at(m_matViewCache, vec3 { camPos.x, camPos.y, camPos.z }, camForward, camUp);

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
    if (m_bCameraUBODirty)
    {
        m_bCameraUBODirty = false;

        glBindBuffer(GL_UNIFORM_BUFFER, m_nCameraUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4x4), getViewMatrix());
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4x4), sizeof(mat4x4), getProjectionMatrix());
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4x4) * 2, sizeof(vec3), &m_pNode->getPosition());
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}
