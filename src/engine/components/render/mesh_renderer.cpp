#include "mesh_renderer.h"

#include <glad/gl.h>
#include "../../core/debug_macro.h"
#include "../../core/window.h"
#include "../../core/camera.h"
#include "../../core/scene/node.h"
#include "../../render/image.h"
#include "../../render/shader.h"
#include "../../render/material.h"
#include "../../render/lighting/light_manager.h"
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

void MeshRenderer::setShader(Shader* pShader)
{
    m_pShader = pShader;
    initShader(pShader);
}

void MeshRenderer::initShader(Shader* const pShader)
{
    glUseProgram(pShader->getProgram());

    const GLuint CAMERA_BINDING_POINT = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BINDING_POINT, Camera::main->getCameraUBO());
    GLuint viewProjIndex = glGetUniformBlockIndex(pShader->getProgram(), SHADER_GLOBAL_UNIFORM_CAMERA_MATRICES.data());
    glUniformBlockBinding(pShader->getProgram(), viewProjIndex, CAMERA_BINDING_POINT);

    const GLuint LIGHTING_BINDING_POINT = 1;
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHTING_BINDING_POINT, LightManager::getInstance()->getLightingUBO());
    GLuint lightIndex = glGetUniformBlockIndex(pShader->getProgram(), SHADER_GLOBAL_UNIFORM_LIGHTING_DATA.data());
    glUniformBlockBinding(pShader->getProgram(), lightIndex, LIGHTING_BINDING_POINT);

    glUseProgram(0);

    m_pModelUniform = pShader->getUniformHandle("u_Model");
    m_pSpecularParamUniform = pShader->getUniformHandle("u_SpecularParams");
    m_pMainTexUniform = pShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
    m_pSpecularMapUniform = pShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_1);
    m_pNormalMapUniform = pShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_2);
    m_pTextureEnabledUniform = pShader->getUniformHandle("u_textureEnabled");

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

    // Enable and set the vertex attributes using the retrieved locations
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormal), (void*)offsetof(VertexWUVNormal, pos));

    glEnableVertexAttribArray(nVUVAttr);
    glVertexAttribPointer(nVUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormal), (void*)offsetof(VertexWUVNormal, uv));

    glEnableVertexAttribArray(nVNormalAttr);
    glVertexAttribPointer(nVNormalAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWUVNormal), (void*)offsetof(VertexWUVNormal, normal));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void MeshRenderer::draw()
{
    ASSERT(m_pMesh, "Mesh must be set before drawing the mesh");


    const mat4x4& local = m_pNode->getWorldMatrix();

    if (m_pShader)
    {
        glUseProgram(m_pShader->getProgram());
    }
    else if (m_pMaterial && m_pMaterial->getShader())
    {
        glUseProgram(m_pMaterial->getShader()->getProgram());
    }
    else
    {
        ASSERT(false, "Shader and material not set for MeshRenderer");
        return;
    }

    glUniformMatrix4fv(m_pModelUniform->m_nLocation, 1, GL_FALSE, (const GLfloat*) local);
    glUniform2f(m_pSpecularParamUniform->m_nLocation, 1.f, 32.f);

    int ntextureBitmask = 0;

    if (m_pMainTexUniform)
    {
        if (m_pMainTexture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_pMainTexture->getTextureID());
            glUniform1i(m_pMainTexUniform->m_nLocation, 0);

            ntextureBitmask |= 1; // Enable main texture
        }
        else if (m_pMaterial && m_pMaterial->getAlbedoMap())
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_pMaterial->getAlbedoMap()->getTextureID());
            glUniform1i(m_pMainTexUniform->m_nLocation, 0);

            ntextureBitmask |= 1; // Enable main texture
        }
    }

    if (m_pSpecularMapUniform)
    {
        if (m_pSpecularMap)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_pSpecularMap->getTextureID());
            glUniform1i(m_pSpecularMapUniform->m_nLocation, 1);

            ntextureBitmask |= 2; // Enable specular map
        }
        else if (m_pMaterial && m_pMaterial->getSpecularMap())
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_pMaterial->getSpecularMap()->getTextureID());
            glUniform1i(m_pSpecularMapUniform->m_nLocation, 1);

            ntextureBitmask |= 2; // Enable specular map
        }
    }

    if (m_pNormalMapUniform)
    {
        if (m_pMaterial && m_pMaterial->getNormalMap())
        {
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, m_pMaterial->getNormalMap()->getTextureID());
            glUniform1i(m_pNormalMapUniform->m_nLocation, 2);

            ntextureBitmask |= 4; // Enable normal map
        }
    }

    glUniform1i(m_pTextureEnabledUniform->m_nLocation, ntextureBitmask);

    glBindVertexArray(m_nVertexArray);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);
    glDrawElements(GL_TRIANGLES, m_nIndiceCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    INCREASE_DRAW_CALL_COUNT();

    glUseProgram(0);
}