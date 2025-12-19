#include "gizmos.h"

#include <glad/gl.h>
#include <imgui.h>

#include "editor.h"
#include "../engine/core/debug_macro.h"
#include "../engine/core/camera.h"
#include "../engine/core/scene/node.h"
#include "../engine/core/input/input_manager.h"
#include "../engine/render/shader_loader.h"
#include "../engine/render/image_loader.h"
#include "../engine/render/vertex.h"


GizmosManager::GizmosManager()
{
    registerBuffer();

    InputManager::getInstance()->registerMouseButtonCallback(MouseButton::BUTTON_LEFT, std::bind(&GizmosManager::onMouseClickCheck, this, std::placeholders::_1));
}

GizmosManager::~GizmosManager()
{
    glDeleteBuffers(1, &m_nVertexBuffer);
    glDeleteVertexArrays(1, &m_nVertexArray);
}

void GizmosManager::registerBuffer()
{
    m_pImageGizmosShader = ShaderLoader::getInstance()->getShader("simple_gizmos");

    m_pPositionUniform = m_pImageGizmosShader->getUniformHandle("u_WorldPosition");
    m_pColorUniform = m_pImageGizmosShader->getUniformHandle("u_imageColor");
    m_pTextureUniform = m_pImageGizmosShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
    m_pUseTextureUniform = m_pImageGizmosShader->getUniformHandle("u_useTexture");

    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);

    float fStartX = -1 / 2.0f;
    float fStartY = -1 / 2.0f;
    VertexWUV arrVertices[4];
    arrVertices[0] = { { fStartX, fStartY }, { 0, 0}  }; // Bottom left
    arrVertices[2] = { { fStartX, fStartY + 1 }, { 0, 1 } }; // Top right
    arrVertices[1] = { { fStartX + 1, fStartY }, { 1, 0 } }; // Bottom right
    arrVertices[3] = { { fStartX + 1, fStartY + 1 }, { 1, 1 } }; // Top left
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrVertices), arrVertices, GL_STATIC_DRAW);

    GLuint nVPosAttr = m_pImageGizmosShader->getAttributeLocation("a_vPos");
    GLuint nUVAttr = m_pImageGizmosShader->getAttributeLocation("a_vUV");

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(nUVAttr);
    glVertexAttribPointer(nUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GizmosManager::clearAllGizmos()
{
    m_nImageGizmosSize = 0;
}

const Vector3 DEFAULT_IMAGE_GIZMOS_COLOR = Vector3(1, 1, 1);

void GizmosManager::addImageGizmos(Component* const pComponent, const Vector3& vecPosition, const std::string_view& m_strImagePath)
{
    addImageGizmos(pComponent, vecPosition, m_strImagePath, DEFAULT_IMAGE_GIZMOS_COLOR);
}

void GizmosManager::addImageGizmos(Component* const pComponent, const Vector3& vecPosition, const std::string_view& m_strImagePath, const Vector3& vecColor)
{
    if (m_nImageGizmosSize + 1 < m_vecImageGizmos.size())
    {
        ImageGizmosData& oData = m_vecImageGizmos.at(m_nImageGizmosSize++);
        oData.m_vecPosition = vecPosition;
        oData.m_strImagePath = m_strImagePath;
        oData.m_vecColor = vecColor;
        oData.m_pAttachedComponent = pComponent;
        return;
    }

    m_vecImageGizmos.push_back(ImageGizmosData {
        .m_vecPosition = vecPosition,
        .m_vecColor = vecColor,
        .m_strImagePath = m_strImagePath,
        .m_pAttachedComponent = pComponent,
    });
}

const Color DEFAULT_GIZMOS_COLOR = Color(1, 1, 1, .5f);

void GizmosManager::addSphereGizmos(const Vector3& vecPosition, float fRadius)
{
    addSphereGizmos(vecPosition, fRadius, DEFAULT_GIZMOS_COLOR);
}
void GizmosManager::addSphereGizmos(const Vector3& vecPosition, float fRadius, const Color& vecColor)
{
    if (m_nSphereGizmosSize + 1 < m_vecSphereGizmos.size())
    {
        SphereGizmosData& oData = m_vecSphereGizmos.at(m_nSphereGizmosSize++);
        oData.m_vecPosition = vecPosition;
        oData.m_fRadius = fRadius;
        oData.m_color = vecColor;
        return;
    }

    m_vecSphereGizmos.push_back(SphereGizmosData {
        .m_vecPosition = vecPosition,
        .m_fRadius = fRadius,
        .m_color = vecColor,
    });
}

void GizmosManager::addCubeGizmos(const Vector3& vecPosition, const Vector3& vecSize)
{
    addCubeGizmos(vecPosition, vecSize, DEFAULT_GIZMOS_COLOR);
}
void GizmosManager::addCubeGizmos(const Vector3& vecPosition, const Vector3& vecSize, const Color& vecColor)
{
    if (m_nCubeGizmosSize + 1 < m_vecCubeGizmos.size())
    {
        CubeGizmosData& oData = m_vecCubeGizmos.at(m_nCubeGizmosSize++);
        oData.m_vecPosition = vecPosition;
        oData.m_vecSize = vecSize;
        oData.m_color = vecColor;
        return;
    }

    m_vecCubeGizmos.push_back(CubeGizmosData {
        .m_vecPosition = vecPosition,
        .m_vecSize = vecSize,
        .m_color = vecColor,
    });
}


void GizmosManager::drawAllGizmos()
{
    drawSphereGizmos();
    drawCubeGizmos();
    drawImageGizmos();
}

void GizmosManager::drawSphereGizmos()
{
    // TODO: Implementation for drawing sphere gizmos goes here
}

void GizmosManager::drawCubeGizmos()
{
    // TODO: Implementation for drawing cube gizmos goes here
}

void GizmosManager::drawImageGizmos()
{
    glUseProgram(m_pImageGizmosShader->getProgram());
    glBindVertexArray(m_nVertexArray);

    for (int i = 0; i < m_nImageGizmosSize; ++i)
    {
        ImageGizmosData& oData = m_vecImageGizmos.at(i);

        if (Image* pImage = ImageLoader::getInstance()->getImageByPath(oData.m_strImagePath); pImage != nullptr)
        {
            glUniform3f(m_pPositionUniform->m_nLocation, oData.m_vecPosition.x, oData.m_vecPosition.y, oData.m_vecPosition.z);

            // LOGLN("Color: {} {} {}", oData.m_vecColor.x, oData.m_vecColor.y, oData.m_vecColor.z);
            glUniform4f(m_pColorUniform->m_nLocation, oData.m_vecColor.x, oData.m_vecColor.y, oData.m_vecColor.z, 1);
            glUniform1i(m_pUseTextureUniform->m_nLocation, 1);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pImage->getTextureID());
            glUniform1i(m_pTextureUniform->m_nLocation, 0);

            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
            glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        }
    }

    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}

void GizmosManager::onMouseClickCheck(bool bPressed)
{
    if (!bPressed) return;

    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;

    InputManager* pInput = InputManager::getInstance();
    Vector2 oScreenPos;
    pInput->getMousePosition(oScreenPos.x, oScreenPos.y);

    Camera* pMainCamera = Camera::main;

    for (int i = 0; i < m_nImageGizmosSize; ++i)
    {
        ImageGizmosData& oData = m_vecImageGizmos.at(i);
        Vector3 oGizmosScreenPos = pMainCamera->worldPositionToScreenPosition(oData.m_vecPosition);

        if (oGizmosScreenPos.z < 0)
            continue; // Behind camera

        const float fXDelta = std::abs(oScreenPos.x - oGizmosScreenPos.x);
        const float fYDelta = std::abs(oScreenPos.y - oGizmosScreenPos.y);
        const float fSize = 30 / oGizmosScreenPos.z;

        if (fXDelta < fSize && fYDelta < fSize)
        {
            if (oData.m_pAttachedComponent && oData.m_pAttachedComponent->getNode())
            {
                Editor::setSelectedNode(oData.m_pAttachedComponent->getNode());
            }
        }
    }
}
