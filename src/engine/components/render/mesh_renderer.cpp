#include "mesh_renderer.h"

#include <glad/gl.h>
#include "../../core/debug_macro.h"
#include "../../core/window.h"
#include "../../core/camera.h"
#include "../../core/scene/node.h"
#include "../../render/image.h"
#include "../../render/shader.h"
#include "../../render/shader_loader.h"
#include "../../render/material.h"
#include "../../render/lighting/light_manager.h"
#include "../../render/lighting/direction_light.h"
#include "../../render/models/simple_obj_reader.h"


MeshRenderer::MeshRenderer()
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::setMaterial(std::shared_ptr<Material>& pMaterial)
{
    m_pMaterial = pMaterial;
    initShader(m_pMaterial->getShader());
}

void MeshRenderer::initShader(Shader* const pShader)
{
    // Main 3d shader uniforms
    m_pModelUniform = pShader->getUniformHandle("u_Model");
    m_pSpecularParamUniform = pShader->getUniformHandle("u_SpecularParams");
    m_pTextureEnabledUniform = pShader->getUniformHandle("u_textureEnabled");

    // Depth shader uniforms
    m_pDepthTextureUniform = pShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_3);
    m_pLightMatrixUniform1 = pShader->getUniformHandle("u_LightMatrix");
    m_pShadowColorUniform = pShader->getUniformHandle("u_shadowColor");

    bindDepthVertexArray();
    bindVertexArray(pShader);
}

void MeshRenderer::bindVertexArray(Shader* const pShader)
{
    m_nIndiceCount = m_pMesh->m_nIndiceCount;
    
    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->m_nVertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pMesh->m_nIndexBuffer);

    // Get the attribute locations from the shader
    GLuint nVPosAttr = pShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr = pShader->getAttributeLocation("a_vUV");
    GLuint nVNormalAttr = pShader->getAttributeLocation("a_vNormal");
    GLuint nVTangentAttr = pShader->getAttributeLocation("a_vTangent");
    GLuint nVBitangentAttr = pShader->getAttributeLocation("a_vBitangent");

    // Enable and set the vertex attributes using the retrieved locations
    if (nVPosAttr != GL_INVALID_VALUE)
    {
        glEnableVertexAttribArray(nVPosAttr);
        glVertexAttribPointer(nVPosAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormalTangent), (void*)offsetof(VertexWUVNormalTangent, pos));
    }

    if (nVUVAttr != GL_INVALID_VALUE)
    {
        glEnableVertexAttribArray(nVUVAttr);
        glVertexAttribPointer(nVUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormalTangent), (void*)offsetof(VertexWUVNormalTangent, uv));
    }

    if (nVNormalAttr != GL_INVALID_VALUE)
    {
        glEnableVertexAttribArray(nVNormalAttr);
        glVertexAttribPointer(nVNormalAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormalTangent), (void*)offsetof(VertexWUVNormalTangent, normal));
    }

    if (nVTangentAttr != GL_INVALID_VALUE)
    {
        glEnableVertexAttribArray(nVTangentAttr);
        glVertexAttribPointer(nVTangentAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormalTangent), (void*)offsetof(VertexWUVNormalTangent, tangent));
    }

    if (nVBitangentAttr != GL_INVALID_VALUE)
    {
        glEnableVertexAttribArray(nVBitangentAttr);
        glVertexAttribPointer(nVBitangentAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormalTangent), (void*)offsetof(VertexWUVNormalTangent, bitangent));
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshRenderer::bindDepthVertexArray()
{
    m_nIndiceCount = m_pMesh->m_nIndiceCount;
    
    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->m_nVertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pMesh->m_nIndexBuffer);

    m_pDepthShader = ShaderLoader::getInstance()->getShader("3d_depth");
    m_pDepthModelUniform = m_pDepthShader->getUniformHandle("u_Model");
    m_pLightMatrixUniform2 = m_pDepthShader->getUniformHandle("u_LightMatrix");

    GLuint nVPosAttr = m_pDepthShader->getAttributeLocation("a_vPos");

    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormal), (void*)offsetof(VertexWUVNormal, pos));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}


bool MeshRenderer::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    if (Component::deserializeField(deserializer, strFieldName, strFieldValue)) return true;

    // TODO: Deserialize m_pMesh, m_pMaterial

    return false;
}

void MeshRenderer::serializeToWrapper(DataSerializer& serializer) const
{
    // TODO: Serialize m_pMesh, m_pMaterial
}

void MeshRenderer::onNodeFinishedDeserialization()
{}


void MeshRenderer::draw()
{
    ASSERT(m_pMesh, "Mesh must be set before drawing the mesh");

    if (!m_pMaterial || !m_pMaterial->getShader())
    {
        ASSERT(false, "Shader and material not set for MeshRenderer");
        return;
    }

    m_pMaterial->useShader();

    ShaderUniformHandle::sendData(m_pModelUniform, m_pNode->getWorldMatrix());

    int ntextureBitmask = m_pMaterial->sendTexturesData();

    DirectionLightComponent* pMainDirLight = LightManager::getInstance()->getMainDirectionLightComponent();
    if (pMainDirLight && pMainDirLight->getShadowsEnabled())
    {
        if (ShaderUniformHandle::sendTexture(m_pDepthTextureUniform, LightManager::getInstance()->getShadowDepthMapTexture(), 3))
        {
            ntextureBitmask |= (1 << 3); // Enable shadow map texture
        }

        if (m_pLightMatrixUniform1)
        {
            glUniformMatrix4fv(m_pLightMatrixUniform1->m_nLocation, 1, GL_FALSE, (const GLfloat*) pMainDirLight->getLightCastingMatrix());
        }

        if (m_pShadowColorUniform)
        {
            const Vector3& vecShadowColor = pMainDirLight->getShadowColor();
            glUniform4f(m_pShadowColorUniform->m_nLocation, vecShadowColor.x, vecShadowColor.y, vecShadowColor.z, pMainDirLight->getShadowIntensity());
        }
    }

    if (m_pTextureEnabledUniform)
    {
        glUniform1i(m_pTextureEnabledUniform->m_nLocation, ntextureBitmask);
    }

    glBindVertexArray(m_nVertexArray);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glDrawElements(GL_TRIANGLES, m_nIndiceCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    INCREASE_DRAW_CALL_COUNT();

    glUseProgram(0);
}

void MeshRenderer::drawDepth()
{
    ASSERT(m_pMesh, "Mesh must be set before drawing the mesh");

    const mat4x4& local = m_pNode->getWorldMatrix();

    glUseProgram(m_pDepthShader->getProgram());

    glUniformMatrix4fv(m_pDepthModelUniform->m_nLocation, 1, GL_FALSE, (const GLfloat*) local);

    DirectionLightComponent* pMainDirLight = LightManager::getInstance()->getMainDirectionLightComponent();
    if (pMainDirLight)
    {
        glUniformMatrix4fv(m_pLightMatrixUniform2->m_nLocation, 1, GL_FALSE, (const GLfloat*) pMainDirLight->getLightCastingMatrix());
    }

    glBindVertexArray(m_nVertexArray);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glDrawElements(GL_TRIANGLES, m_nIndiceCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    INCREASE_DRAW_CALL_COUNT();

    glUseProgram(0);
}