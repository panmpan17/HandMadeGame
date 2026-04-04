#include "camera.h"

#include <glad/gl.h>
#include "window.h"
#include "debug_macro.h"
#include "scene/node.h"
#include "../render/shader_loader.h"
#include "../render/core/renderer.h"
#include "../../editor/inspector_helper.h"


Camera* Camera::main = nullptr;

Camera::Camera()
{
    Window::ins->registerResizeListener([this](const Vector2i& newSize)
    {
        setRatio(newSize.x / (float)newSize.y);
        m_bProjectionMatrixDirty = true;
        m_bViewProjectionMatrixDirty = true;
        m_bCameraUBODirty = true;
    });

    if (Renderer::isUsingOpenGL())
    {
        glGenBuffers(1, &m_nCameraUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, m_nCameraUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4x4) * 2 + sizeof(vec4), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        m_pCameraMetalUBO = Window::ins->getMetalDevice()->newBuffer(sizeof(mat4x4) * 2 + sizeof(vec4), MTL::ResourceStorageModeShared);
    }
#endif // __APPLE__
}

Camera::~Camera()
{
}

void Camera::useAsMain()
{
    main = this;

    ShaderLoader* pShaderLoader = ShaderLoader::getInstance();
    if (pShaderLoader)
    {
        pShaderLoader->onMainCameraChanged();
    }
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

    const Vector3& camPos = m_pNode->getPositionInWorld();
    vec3 camForward, camUp;
    Quaternion rotation = m_pNode->getWorldRotationQuaternion();
    rotation.getForwardVector(camForward);
    rotation.getUpVector(camUp);
    camForward[0] += camPos.x;
    camForward[1] += camPos.y;
    camForward[2] += camPos.z;
    mat4x4_look_at(m_matViewCache, reinterpret_cast<const vec3&>(camPos), camForward, camUp);

    // m_bViewMatrixDirty = false;

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

    // m_bProjectionMatrixDirty = false;

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

    // m_bViewProjectionMatrixDirty = false;

    return m_matViewProjectionCache;
}

void Camera::onStart()
{
    setRatio(Window::ins->getWindowRatio());

    if (!m_pNode)
    {
        return;
    }

    m_pNode->registerOnPositionChangedListener(std::bind(&Camera::markViewMatrixDirty, this));
    m_pNode->registerOnRotationChangedListener(std::bind(&Camera::markViewMatrixDirty, this));
}

void Camera::updateCameraDataBuffer()
{
    if (m_bCameraUBODirty)
    {
        // m_bCameraUBODirty = false;

        Vector3 camPos = m_pNode->getPositionInWorld();

        if (Renderer::isUsingOpenGL())
        {
            glBindBuffer(GL_UNIFORM_BUFFER, m_nCameraUBO);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(mat4x4), getViewMatrix());
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4x4), sizeof(mat4x4), getProjectionMatrix());
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof(mat4x4) * 2, sizeof(vec3), &camPos);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
#if __APPLE__
        else if (Renderer::isUsingMetal())
        {
            float* pBuffer = reinterpret_cast<float*>(m_pCameraMetalUBO->contents());
            memcpy(pBuffer, getViewMatrix(), sizeof(mat4x4));
            memcpy(pBuffer + sizeof(mat4x4) / sizeof(float), getProjectionMatrix(), sizeof(mat4x4));
            memcpy(pBuffer + (sizeof(mat4x4) * 2) / sizeof(float), &camPos, sizeof(vec3));
            m_pCameraMetalUBO->didModifyRange(NS::Range{0, m_pCameraMetalUBO->length()});
        }
#endif // __APPLE__
    }
}

Vector3 Camera::worldPositionToScreenPosition(const Vector3& worldPos)
{
    const mat4x4& viewProj = getViewProjectionMatrix();

    vec4 inPos = { worldPos.x, worldPos.y, worldPos.z, 1.0f };
    vec4 clipPos;
    mat4x4_mul_vec4(clipPos, viewProj, inPos);

    if (clipPos[3] != 0.0f)
    {
        clipPos[0] /= clipPos[3];
        clipPos[1] /= clipPos[3];
        clipPos[2] /= clipPos[3];
    }

    // Convert from NDC to screen space
    const Vector2i& oWindowSize = Window::ins->getWindowSize();

    float screenX = (clipPos[0] * 0.5f + 0.5f) * oWindowSize.x;
    float screenY = (1.0f - (clipPos[1] * 0.5f + 0.5f)) * oWindowSize.y; // Invert Y for screen space

    return Vector3(screenX, screenY, clipPos[3]);
}

void Camera::onInspectorUI(int nComponentIndex)
{
    bool bIsMainCamera = (this == Camera::main);
    if (ImGui::Checkbox("Is Main", &bIsMainCamera))
    {
        if (bIsMainCamera)
        {
            useAsMain();
        }
    }

    inspectorBoolField(nComponentIndex, "Ortho", m_bUseOrthoProjection);
}
