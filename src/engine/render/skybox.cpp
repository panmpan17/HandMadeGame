#include "skybox.h"

#include <glad/gl.h>
#include "image.h"
#include "shader.h"
#include "shader_loader.h"
#include "../core/camera.h"
#include "../core/window.h"


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
    bindVertexArray();
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

void Skybox::loadSkyboxCubmaps(std::initializer_list<std::string_view> strImages)
{
    if (static_cast<int>(strImages.size()) != 6)
    {
        LOGERR("Skybox requires 6 images for cubemap");
        return;
    }

    glGenTextures(1, &m_nSkyboxTextureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_nSkyboxTextureID);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (int i = 0; i < 6; ++i)
    {
        Image oImage(*(strImages.begin() + i), false);
        if (oImage.isCPULoaded())
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, oImage.getWidth(), oImage.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, oImage.getData());
        }
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Skybox::draw()
{
    glDepthFunc(GL_LEQUAL);

    glUseProgram(m_pSkyboxShader->getProgram());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_nSkyboxTextureID);

    glBindVertexArray(m_nSkyboxVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    INCREASE_DRAW_CALL_COUNT(12);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    glDepthFunc(GL_LESS);
}
