#include "skybox.h"

#include <glad/gl.h>
#include "camera.h"
#include "draw/image.h"
#include "draw/shader.h"
#include "draw/shader_loader.h"
#include "window.h"


Skybox::Skybox()
{
    m_pSkyboxShader = ShaderLoader::getInstance()->getShader("skybox");
    initShader(m_pSkyboxShader);
}

Skybox::~Skybox()
{
}

void Skybox::initShader(Shader* const pShader)
{
    glUseProgram(pShader->getProgram());

    const GLuint CAMERA_BINDING_POINT = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_BINDING_POINT, Camera::main->getCameraUBO());
    GLuint viewProjIndex = glGetUniformBlockIndex(pShader->getProgram(), SHADER_GLOBAL_UNIFORM_CAMERA_MATRICES.data());
    glUniformBlockBinding(pShader->getProgram(), viewProjIndex, CAMERA_BINDING_POINT);

    bindVertexArray();

    glUseProgram(0);
}

void Skybox::bindVertexArray()
{
    float arrSkyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &m_nSkyboxVAO);
    glGenBuffers(1, &m_nSkyboxVBO);

    glBindVertexArray(m_nSkyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_nSkyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrSkyboxVertices), &arrSkyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Skybox::loadSkyboxCubmaps(const std::string_view& strRight, const std::string_view& strLeft,
                               const std::string_view& strTop, const std::string_view& strBottom,
                               const std::string_view& strFront, const std::string_view& strBack)
{
    glGenTextures(1, &m_nSkyboxTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_nSkyboxTextureID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    {
        Image oRightImage(strRight, false);
        if (oRightImage.isCPULoaded())
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, oRightImage.getWidth(), oRightImage.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, oRightImage.getData());
        }
    }

    {
        Image oLeftImage(strLeft, false);
        if (oLeftImage.isCPULoaded())
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, oLeftImage.getWidth(), oLeftImage.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, oLeftImage.getData());
        }
    }

    {
        Image oTopImage(strTop, false);
        if (oTopImage.isCPULoaded())
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, oTopImage.getWidth(), oTopImage.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, oTopImage.getData());
        }
    }

    {
        Image oBottomImage(strBottom, false);
        if (oBottomImage.isCPULoaded())
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, oBottomImage.getWidth(), oBottomImage.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, oBottomImage.getData());
        }
    }

    {
        Image oFrontImage(strFront, false);
        if (oFrontImage.isCPULoaded())
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, oFrontImage.getWidth(), oFrontImage.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, oFrontImage.getData());
        }
    }

    {
        Image oBackImage(strBack, false);
        if (oBackImage.isCPULoaded())
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, oBackImage.getWidth(), oBackImage.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, oBackImage.getData());
        }
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::draw()
{
    glDepthMask(GL_FALSE);

    glUseProgram(m_pSkyboxShader->getProgram());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_nSkyboxTextureID);

    glBindVertexArray(m_nSkyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    INCREASE_DRAW_CALL_COUNT();
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    glDepthMask(GL_TRUE);
}
