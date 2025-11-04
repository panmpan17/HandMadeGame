// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "quad.h"
#include "../draw/image.h"
#include "../camera.h"
#include "../debug_macro.h"
#include "../node.h"
#include "../window.h"
#include "../serialization/serializer.h"
#include "../draw/shader.h"
#include "../draw/shader_loader.h"
#include "../random.h"


inline constexpr std::string_view SHADER_UNIFORM_COLOR = "u_imageColor";
inline constexpr std::string_view SHADER_UNIFORM_USE_TEXTURE = "u_useTexture";

inline constexpr std::string_view SHADER_UNIFORM_SPRITE_SHEET_X_COUNT = "u_spriteSheetXCount";
inline constexpr std::string_view SHADER_UNIFORM_SPRITE_SHEET_Y_COUNT = "u_spriteSheetYCount";
inline constexpr std::string_view SHADER_UNIFORM_UV_OFFSET = "u_uvOffset";


Quad::Quad(float fWidth, float fHeight, vec4 color) : m_fWidth(fWidth), m_fHeight(fHeight)
{
    m_nID = generateRandomUUID();
    vec4_dup(m_color, color);
}

Quad::~Quad()
{
    glDeleteBuffers(1, &m_nVertexBuffer);
    glDeleteVertexArrays(1, &m_nVertexArray);
}

void Quad::setShader(Shader* pShader)
{
    m_pShader = pShader;

    m_pMVPHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_MVP);
    m_pColorHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_COLOR);
    m_pTextureHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
    m_pUseTextureHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_USE_TEXTURE);

    m_pSpriteSheetXCountHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_SPRITE_SHEET_X_COUNT);
    m_pSpriteSheetYCountHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_SPRITE_SHEET_Y_COUNT);
    m_pUVOffsetHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_UV_OFFSET);
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

    GLuint nVPosAttr = m_pShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr = m_pShader->getAttributeLocation("a_vUV");

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(nVUVAttr);
    glVertexAttribPointer(nVUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Quad::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    mat4x4 mvp;
    const mat4x4& matModel = m_pNode->getWorldMatrix();
    const mat4x4& cameraViewMatrix = Camera::main->getViewProjectionMatrix();
    mat4x4_mul(mvp, cameraViewMatrix, matModel);

    glUseProgram(m_pShader->getProgram());
    glUniformMatrix4fv(m_pMVPHandle->m_nLocation, 1, GL_FALSE, (const GLfloat*) mvp);
    glUniform4f(m_pColorHandle->m_nLocation, m_color[0], m_color[1], m_color[2], 1);
    predrawSetShaderUniforms();

    if (m_pImage)
    {
        glUniform1i(m_pUseTextureHandle->m_nLocation, 1);
        glUniform1i(m_pTextureHandle->m_nLocation, 0); // Texture unit 0

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_pImage ? m_pImage->getTextureID() : 0);
    }
    else
    {
        glUniform1i(m_pUseTextureHandle->m_nLocation, 0);
    }

    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}

void Quad::predrawSetShaderUniforms()
{
    glUniform1i(m_pSpriteSheetXCountHandle->m_nLocation, 1);
    glUniform1i(m_pSpriteSheetYCountHandle->m_nLocation, 1);
    glUniform2f(m_pUVOffsetHandle->m_nLocation, 0.0f, 0.0f);
}

void Quad::serializeToWrapper(DataSerializer& serializer) const
{
    serializer.ADD_ATTRIBUTES(m_fWidth);
    serializer.ADD_ATTRIBUTES(m_fHeight);
    serializer.ADD_ATTRIBUTES(m_color);

    if (m_pImage)
    {
        serializer.ADD_ATTRIBUTES_VALUE(m_pImage, m_pImage->getPath());
    }

    if (m_pShader)
    {
        serializer.ADD_ATTRIBUTES_VALUE(m_pShader, m_pShader->getId());
    }
}

bool Quad::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    if (Component::deserializeField(deserializer, strFieldName, strFieldValue)) return true;

    DESERIALIZE_FIELD(m_fWidth);
    DESERIALIZE_FIELD(m_fHeight);
    DESERIALIZE_FIELD(m_color);

    IF_DESERIALIZE_FIELD_CHECK(m_pImage)
    {
        m_pImage = ImageLoader::getInstance()->getImageByPath(strFieldValue);
        return true;
    }

    IF_DESERIALIZE_FIELD_CHECK(m_pShader)
    {
        m_pShader = ShaderLoader::getInstance()->getShader(std::atoi(strFieldValue.data()));
        return true;
    }

    return false;
}

void Quad::onNodeFinishedDeserialization()
{
    if (m_pShader)
    {
        setShader(m_pShader);
        registerBuffer();
    }
}


Sprite::Sprite(Image* pImage, int nPixelPerUnit/* = 100*/)
    // : Quad(0.5f, 0.5f, {1.f, 1.f, 1.f, 1.f})
{
    m_nID = generateRandomUUID();

    m_pImage = pImage;
    m_fWidth = pImage->getWidth() * (1.f / nPixelPerUnit);
    m_fHeight = pImage->getHeight() * (1.f / nPixelPerUnit);
    m_vecUVOOffset[0] = 0;
    m_vecUVOOffset[1] = 0;
}

Sprite::Sprite(Image* pImage, int nSpriteSheetXCount, int nSpriteSheetYCount, int nSpriteIndex/* = 0 */, int nPixelPerUnit/* = 100*/)
{
    m_nID = generateRandomUUID();
    
    m_pImage = pImage;
    m_fWidth = (pImage->getWidth() / (float)nSpriteSheetXCount) * (1.f / nPixelPerUnit);
    m_fHeight = (pImage->getHeight() / (float)nSpriteSheetYCount) * (1.f / nPixelPerUnit);
    m_nSpriteSheetXCount = nSpriteSheetXCount;
    m_nSpriteSheetYCount = nSpriteSheetYCount;

    setSpriteIndex(nSpriteIndex);
}

Sprite::~Sprite()
{
}

void Sprite::setSpriteIndex(int nIndex)
{
    m_nSpriteIndex = nIndex % (m_nSpriteSheetXCount * m_nSpriteSheetYCount);
    int xIndex = m_nSpriteIndex % m_nSpriteSheetXCount;
    int yIndex = m_nSpriteIndex / m_nSpriteSheetXCount;
    m_vecUVOOffset[0] = xIndex / (float)m_nSpriteSheetXCount;
    m_vecUVOOffset[1] = yIndex / (float)m_nSpriteSheetYCount;
}

void Sprite::draw()
{
    Quad::draw();
}

void Sprite::predrawSetShaderUniforms()
{
    glUniform1i(m_pSpriteSheetXCountHandle->m_nLocation, m_nSpriteSheetXCount);
    glUniform1i(m_pSpriteSheetYCountHandle->m_nLocation, m_nSpriteSheetYCount);

    glUniform2f(m_pUVOffsetHandle->m_nLocation, m_vecUVOOffset[0], m_vecUVOOffset[1]);
}

void Sprite::serializeToWrapper(DataSerializer& serializer) const
{
    Quad::serializeToWrapper(serializer);

    serializer.ADD_ATTRIBUTES(m_nSpriteSheetXCount);
    serializer.ADD_ATTRIBUTES(m_nSpriteSheetYCount);
    serializer.ADD_ATTRIBUTES(m_nSpriteIndex);
}

bool Sprite::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    if (Quad::deserializeField(deserializer, strFieldName, strFieldValue))
    {
        return true;
    }

    DESERIALIZE_FIELD(m_nSpriteSheetXCount);
    DESERIALIZE_FIELD(m_nSpriteSheetYCount);
    DESERIALIZE_FIELD(m_nSpriteIndex);

    return false;
}

void Sprite::onNodeFinishedDeserialization()
{
    Quad::onNodeFinishedDeserialization();
}

