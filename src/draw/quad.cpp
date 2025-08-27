// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "quad.h"
#include "../image.h"
#include "../camera.h"
#include "../debug_macro.h"
#include "../node.h"
#include "../window.h"

Quad::Quad(float fWidth, float fHeight, vec4 color) : m_fWidth(fWidth), m_fHeight(fHeight)
{
    vec4_dup(m_color, color);
}

Quad::~Quad()
{
    glDeleteBuffers(1, &m_nVertexBuffer);
    glDeleteVertexArrays(1, &m_nVertexArray);
}

void Quad::registerBuffer()
{
    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);

    float fStartX = -m_fWidth / 2.0f;
    float fStartY = -m_fHeight / 2.0f;
    VertexWUV arrVertices[4];
    arrVertices[0] = { { fStartX, fStartY }, { 0.0f, 0.0f } }; // Bottom left
    arrVertices[1] = { { fStartX + m_fWidth, fStartY }, { 1.0f, 0.0f } }; // Bottom right
    arrVertices[2] = { { fStartX, fStartY + m_fHeight }, { 0.0f, 1.0f } }; // Top right
    arrVertices[3] = { { fStartX + m_fWidth, fStartY + m_fHeight }, { 1.0f, 1.0f } }; // Top left
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrVertices), arrVertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    glEnableVertexAttribArray(m_pShader->getVPosLocation());
    glVertexAttribPointer(m_pShader->getVPosLocation(), 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(m_pShader->getVUVLocation());
    glVertexAttribPointer(m_pShader->getVUVLocation(), 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Quad::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    mat4x4 mvp, local, cameraViewMatrix;

    mat4x4_identity(local);

    const vec3& position = m_pNode->getPosition();
    mat4x4_translate(local, position[0], position[1], position[2]);

    mat4x4_rotate_Z(local, local, m_pNode->getRotation());

    Camera::main->getViewMatrix(cameraViewMatrix);
    mat4x4_mul(mvp, cameraViewMatrix, local);

    glUseProgram(m_pShader->getProgram());
    glUniformMatrix4fv(m_pShader->getMvpLocation(), 1, GL_FALSE, (const GLfloat*) mvp);
    glUniform4f(m_pShader->getColorLocation(), m_color[0], m_color[1], m_color[2], 1);
    predrawSetShaderUniforms();

    glUniform1i(m_pShader->getTextureLocation(), 0); // Texture unit 0

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_pImage ? m_pImage->getTextureID() : 0);
    // LOGLN_EX("Quad::draw() - Using texture location: {}", m_pShader->getTextureLocation());

    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}

void Quad::predrawSetShaderUniforms()
{
    glUniform1i(m_pShader->getSpriteSheetXCountLocation(), 1);
    glUniform1i(m_pShader->getSpriteSheetYCountLocation(), 1);
    glUniform2f(m_pShader->getUVOffsetLocation(), 0.0f, 0.0f);
}


Sprite::Sprite(Image* pImage, int nPixelPerUnit/* = 100*/)
    // : Quad(0.5f, 0.5f, {1.f, 1.f, 1.f, 1.f})
{
    m_pImage = pImage;
    m_fWidth = pImage->getWidth() * (1.f / nPixelPerUnit);
    m_fHeight = pImage->getHeight() * (1.f / nPixelPerUnit);
}

Sprite::Sprite(Image* pImage, int nSpriteSheetXCount, int nSpriteSheetYCount, int nPixelPerUnit/* = 100*/)
{
    m_pImage = pImage;
    m_fWidth = (pImage->getWidth() / (float)nSpriteSheetXCount) * (1.f / nPixelPerUnit);
    m_fHeight = (pImage->getHeight() / (float)nSpriteSheetYCount) * (1.f / nPixelPerUnit);
    m_nSpriteSheetXCount = nSpriteSheetXCount;
    m_nSpriteSheetYCount = nSpriteSheetYCount;
}

Sprite::~Sprite()
{
}

void Sprite::draw()
{
    Quad::draw();
}

void Sprite::predrawSetShaderUniforms()
{
    glUniform1i(m_pShader->getSpriteSheetXCountLocation(), m_nSpriteSheetXCount);
    glUniform1i(m_pShader->getSpriteSheetYCountLocation(), m_nSpriteSheetYCount);

    // TODO: change the uv offset
    glUniform2f(m_pShader->getUVOffsetLocation(), 0.0f, 0.0f);
}
