#include "gizmos.h"

#include <glad/gl.h>
#include <imgui.h>

#include "editor.h"
#include "../engine/core/window.h"
#include "../engine/core/debug_macro.h"
#include "../engine/core/camera.h"
#include "../engine/core/scene/node.h"
#include "../engine/core/input/input_manager.h"
#include "../engine/render/shader_loader.h"
#include "../engine/render/image_loader.h"
#include "../engine/render/vertex.h"


GizmosManager::GizmosManager()
{
    initImageGizmosShaderAndBuffer();
    initSphereGizmosShaderAndBuffer();
    initCubeGizmosShaderAndBuffer();

    InputManager::getInstance()->registerMouseButtonCallback(MouseButton::BUTTON_LEFT, std::bind(&GizmosManager::onMouseClickCheck, this, std::placeholders::_1));
}

GizmosManager::~GizmosManager()
{
    glDeleteBuffers(1, &m_nImageGizmosVertexBuffer);
    glDeleteVertexArrays(1, &m_nImageGizmosVertexArray);
}

void GizmosManager::initImageGizmosShaderAndBuffer()
{
    m_pImageGizmosShader = ShaderLoader::getInstance()->getShader("simple_gizmos");

    m_pImageGizmosPositionUniform = m_pImageGizmosShader->getUniformHandle("u_WorldPosition");
    m_pImageGizmosColorUniform = m_pImageGizmosShader->getUniformHandle("u_imageColor");
    m_pImageGizmosTextureUniform = m_pImageGizmosShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
    m_pImageGizmosUseTextureUniform = m_pImageGizmosShader->getUniformHandle("u_useTexture");

    glGenBuffers(1, &m_nImageGizmosVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nImageGizmosVertexBuffer);

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

    glGenVertexArrays(1, &m_nImageGizmosVertexArray);
    glBindVertexArray(m_nImageGizmosVertexArray);
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(nUVAttr);
    glVertexAttribPointer(nUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GizmosManager::initSphereGizmosShaderAndBuffer()
{
}

void GizmosManager::initCubeGizmosShaderAndBuffer()
{
    m_pCubeGizmosShader = ShaderLoader::getInstance()->getShader("mesh_gizmos");

    m_pCubeGizmosPositionUniform = m_pCubeGizmosShader->getUniformHandle("u_WorldPosition");
    m_pCubeGizmosSizeUniform = m_pCubeGizmosShader->getUniformHandle("u_Scale");
    m_pCubeGizmosColorUniform = m_pCubeGizmosShader->getUniformHandle("u_GizmosColor");

    glGenBuffers(1, &m_nCubeGizmosVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nCubeGizmosVertexBuffer);

    float fStartX = -1 / 2.0f;
    float fStartY = -1 / 2.0f;
    float fStartZ = -1 / 2.0f;
    
    // 36 vertices for a unit cube (6 faces * 2 triangles * 3 vertices)
    vec3 arrVertices[36] = {
        // Front face (Z+)
        { fStartX, fStartY, fStartZ + 1 }, { fStartX + 1, fStartY, fStartZ + 1 }, { fStartX + 1, fStartY + 1, fStartZ + 1 },
        { fStartX, fStartY, fStartZ + 1 }, { fStartX + 1, fStartY + 1, fStartZ + 1 }, { fStartX, fStartY + 1, fStartZ + 1 },
        
        // Back face (Z-)
        { fStartX + 1, fStartY, fStartZ }, { fStartX, fStartY, fStartZ }, { fStartX, fStartY + 1, fStartZ },
        { fStartX + 1, fStartY, fStartZ }, { fStartX, fStartY + 1, fStartZ }, { fStartX + 1, fStartY + 1, fStartZ },
        
        // Left face (X-)
        { fStartX, fStartY, fStartZ }, { fStartX, fStartY, fStartZ + 1 }, { fStartX, fStartY + 1, fStartZ + 1 },
        { fStartX, fStartY, fStartZ }, { fStartX, fStartY + 1, fStartZ + 1 }, { fStartX, fStartY + 1, fStartZ },
        
        // Right face (X+)
        { fStartX + 1, fStartY, fStartZ + 1 }, { fStartX + 1, fStartY, fStartZ }, { fStartX + 1, fStartY + 1, fStartZ },
        { fStartX + 1, fStartY, fStartZ + 1 }, { fStartX + 1, fStartY + 1, fStartZ }, { fStartX + 1, fStartY + 1, fStartZ + 1 },
        
        // Top face (Y+)
        { fStartX, fStartY + 1, fStartZ + 1 }, { fStartX + 1, fStartY + 1, fStartZ + 1 }, { fStartX + 1, fStartY + 1, fStartZ },
        { fStartX, fStartY + 1, fStartZ + 1 }, { fStartX + 1, fStartY + 1, fStartZ }, { fStartX, fStartY + 1, fStartZ },
        
        // Bottom face (Y-)
        { fStartX, fStartY, fStartZ }, { fStartX + 1, fStartY, fStartZ }, { fStartX + 1, fStartY, fStartZ + 1 },
        { fStartX, fStartY, fStartZ }, { fStartX + 1, fStartY, fStartZ + 1 }, { fStartX, fStartY, fStartZ + 1 }
    };
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrVertices), arrVertices, GL_STATIC_DRAW);

    GLuint nVPosAttr = m_pCubeGizmosShader->getAttributeLocation("a_vPos");

    glGenVertexArrays(1, &m_nCubeGizmosVertexArray);
    glBindVertexArray(m_nCubeGizmosVertexArray);
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GizmosManager::clearAllGizmos()
{
    m_nImageGizmosSize = 0;
    m_nSphereGizmosSize = 0;
    m_nCubeGizmosSize = 0;
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

void GizmosManager::addSphereGizmos(const Vector3& vecPosition, Quaternion rotation, float fRadius)
{
    addSphereGizmos(vecPosition, rotation, fRadius, DEFAULT_GIZMOS_COLOR);
}
void GizmosManager::addSphereGizmos(const Vector3& vecPosition, Quaternion rotation, float fRadius, const Color& vecColor)
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

void GizmosManager::addCubeGizmos(const Vector3& vecPosition, Quaternion rotation, const Vector3& vecSize)
{
    addCubeGizmos(vecPosition, rotation, vecSize, DEFAULT_GIZMOS_COLOR);
}
void GizmosManager::addCubeGizmos(const Vector3& vecPosition, Quaternion rotation, const Vector3& vecSize, const Color& vecColor)
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
    glEnable(GL_BLEND);
    glDepthMask(GL_FALSE);

    drawSphereGizmos();
    drawCubeGizmos();
    drawImageGizmos();

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void GizmosManager::drawSphereGizmos()
{
    // TODO: Implementation for drawing sphere gizmos goes here
}

void GizmosManager::drawCubeGizmos()
{
    glUseProgram(m_pCubeGizmosShader->getProgram());
    glBindVertexArray(m_nCubeGizmosVertexArray);

    for (int i = 0; i < m_nCubeGizmosSize; ++i)
    {
        CubeGizmosData& oData = m_vecCubeGizmos.at(i);

        glUniform3f(m_pCubeGizmosPositionUniform->m_nLocation, oData.m_vecPosition.x, oData.m_vecPosition.y, oData.m_vecPosition.z);
        glUniform3f(m_pCubeGizmosSizeUniform->m_nLocation, oData.m_vecSize.x, oData.m_vecSize.y, oData.m_vecSize.z);
        // TODO: Impletement rotation uniform
        glUniform4f(m_pCubeGizmosColorUniform->m_nLocation, oData.m_color.r, oData.m_color.g, oData.m_color.b, oData.m_color.a);

        glDrawArrays(GL_TRIANGLES, 0, 36); // Assuming the cube is made of 36 vertices (12 triangles)
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void GizmosManager::drawImageGizmos()
{
    glUseProgram(m_pImageGizmosShader->getProgram());
    glBindVertexArray(m_nImageGizmosVertexArray);

    for (int i = 0; i < m_nImageGizmosSize; ++i)
    {
        ImageGizmosData& oData = m_vecImageGizmos.at(i);

        if (Image* pImage = ImageLoader::getInstance()->getImageByPath(oData.m_strImagePath); pImage != nullptr)
        {
            glUniform3f(m_pImageGizmosPositionUniform->m_nLocation, oData.m_vecPosition.x, oData.m_vecPosition.y, oData.m_vecPosition.z);

            // LOGLN("Color: {} {} {}", oData.m_vecColor.x, oData.m_vecColor.y, oData.m_vecColor.z);
            glUniform4f(m_pImageGizmosColorUniform->m_nLocation, oData.m_vecColor.x, oData.m_vecColor.y, oData.m_vecColor.z, 1);
            glUniform1i(m_pImageGizmosUseTextureUniform->m_nLocation, 1);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pImage->getTextureID());
            glUniform1i(m_pImageGizmosTextureUniform->m_nLocation, 0);

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

    // LOGLN("Mouse Click at: {} {}", oScreenPos.x, oScreenPos.y);

    Camera* pMainCamera = Camera::main;

    const float GizmosWidth = Window::ins->GetActualWidth() / 10;
    const float GizmosHeight = Window::ins->GetActualHeight() / 10;

    for (int i = 0; i < m_nImageGizmosSize; ++i)
    {
        ImageGizmosData& oData = m_vecImageGizmos.at(i);
        Vector3 oGizmosScreenPos = pMainCamera->worldPositionToScreenPosition(oData.m_vecPosition);

        // LOGLN("Gizmos Screen Pos: {} {} {}", oGizmosScreenPos.x, oGizmosScreenPos.y, oGizmosScreenPos.z);

        if (oGizmosScreenPos.z < 0)
            continue; // Behind camera

        const float fXDelta = std::abs(oScreenPos.x - oGizmosScreenPos.x);
        const float fYDelta = std::abs(oScreenPos.y - oGizmosScreenPos.y);

        if (fXDelta < (GizmosWidth / oGizmosScreenPos.z) && fYDelta < (GizmosHeight / oGizmosScreenPos.z))
        {
            if (oData.m_pAttachedComponent && oData.m_pAttachedComponent->getNode())
            {
                // LOGLN("Gizmos Clicked! Select Node: {}", oData.m_pAttachedComponent->getNode()->getName());
                Editor::setSelectedNode(oData.m_pAttachedComponent->getNode());
            }
        }
    }
}
