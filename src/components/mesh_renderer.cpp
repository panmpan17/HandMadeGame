#include "mesh_renderer.h"

#include <glad/gl.h>
#include "../models/simple_obj_reader.h"
#include "../debug_macro.h"
#include "../window.h"
#include "../node.h"
#include "../camera.h"
#include "../lighting/light_manager.h"
#include "../draw/image.h"


inline constexpr std::string_view SHADER_GLOBAL_UNIFORM_CAMERA_MATRICES = "CameraMatrices";
inline constexpr std::string_view SHADER_GLOBAL_UNIFORM_LIGHTING_DATA = "LightData";


MeshRenderer::MeshRenderer()
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::setShader(Shader* pShader)
{
    m_pShader = pShader;

    glUseProgram(m_pShader->getProgram());

    const GLuint CAMERA_BINDING_POINT = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BINDING_POINT, Camera::main->getCameraUBO());
    GLuint viewProjIndex = glGetUniformBlockIndex(m_pShader->getProgram(), SHADER_GLOBAL_UNIFORM_CAMERA_MATRICES.data());
    glUniformBlockBinding(m_pShader->getProgram(), viewProjIndex, CAMERA_BINDING_POINT);

    const GLuint LIGHTING_BINDING_POINT = 1;
    glBindBufferBase(GL_UNIFORM_BUFFER, LIGHTING_BINDING_POINT, LightManager::getInstance()->getLightingUBO());
    GLuint lightIndex = glGetUniformBlockIndex(m_pShader->getProgram(), SHADER_GLOBAL_UNIFORM_LIGHTING_DATA.data());
    glUniformBlockBinding(m_pShader->getProgram(), lightIndex, LIGHTING_BINDING_POINT);

    glUseProgram(0);

    m_pModelUniform = m_pShader->getUniformHandle("u_Model");
    m_pSpecularParamUniform = m_pShader->getUniformHandle("u_SpecularParams");
    m_pMainTexUniform = m_pShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
    m_pSpecularMapUniform = m_pShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_1);
    m_pTextureEnabledUniform = m_pShader->getUniformHandle("u_textureEnabled");

    bindVertexArray();

}

void MeshRenderer::bindVertexArray()
{
    m_nIndiceCount = m_pMesh->m_nIndiceCount;
    
    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_pMesh->m_nVertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pMesh->m_nIndexBuffer);

    // Get the attribute locations from the shader
    GLuint nVPosAttr = m_pShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr = m_pShader->getAttributeLocation("a_vUV");
    GLuint nVNormalAttr = m_pShader->getAttributeLocation("a_vNormal");

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
    ASSERT(m_pShader, "Shader must be set before drawing the mesh");
    ASSERT(m_pMesh, "Mesh must be set before drawing the mesh");


    const mat4x4& local = m_pNode->getWorldMatrix();

    glUseProgram(m_pShader->getProgram());

    glUniformMatrix4fv(m_pModelUniform->m_nLocation, 1, GL_FALSE, (const GLfloat*) local);
    glUniform2f(m_pSpecularParamUniform->m_nLocation, 1.f, 32.f);

    int ntextureBitmask = 0;

    if (m_pMainTexture)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_pMainTexture->getTextureID());
        glUniform1i(m_pMainTexUniform->m_nLocation, 0);

        ntextureBitmask |= 1; // Enable main texture
    }

    if (m_pSpecularMap && m_pSpecularMapUniform)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_pSpecularMap->getTextureID());
        glUniform1i(m_pSpecularMapUniform->m_nLocation, 1);

        ntextureBitmask |= 2; // Enable specular map
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