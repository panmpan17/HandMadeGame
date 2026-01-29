#include <glad/gl.h>

#include "quad.h"
#include "../../render/image_loader.h"
#include "../../core/camera.h"
#include "../../core/debug_macro.h"
#include "../../core/window.h"
#include "../../core/scene/node.h"
#include "../../core/serialization/serializer.h"
#include "../../core/math/random.h"
#include "../../render/shader_loader.h"
#include "../../render/core/renderer.h"


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
    if (Renderer::isUsingOpenGL())
    {
        glDeleteBuffers(1, &m_nVertexBuffer);
        glDeleteVertexArrays(1, &m_nVertexArray);
    }
}

void Quad::setShader(Shader* pShader)
{
    m_pShader = pShader;

    if (Renderer::isUsingOpenGL())
    {
        m_pMVPHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_MVP);
        m_pColorHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_COLOR);
        m_pTextureHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
        m_pUseTextureHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_USE_TEXTURE);

        m_pSpriteSheetXCountHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_SPRITE_SHEET_X_COUNT);
        m_pSpriteSheetYCountHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_SPRITE_SHEET_Y_COUNT);
        m_pUVOffsetHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_UV_OFFSET);
    }
}

void Quad::registerBuffer()
{
    const float fStartX = -m_fWidth / 2.0f;
    const float fStartY = -m_fHeight / 2.0f;

    VertexWUV arrVertices[4];
    arrVertices[0] = { { fStartX, fStartY }, { 0.0f, 0.0f } }; // Bottom left
    arrVertices[1] = { { fStartX + m_fWidth, fStartY }, { 1.0f, 0.0f } }; // Bottom right
    arrVertices[2] = { { fStartX, fStartY + m_fHeight }, { 0.0f, 1.0f } }; // Top right
    arrVertices[3] = { { fStartX + m_fWidth, fStartY + m_fHeight }, { 1.0f, 1.0f } }; // Top left

    if (Renderer::isUsingOpenGL())
    {
        glGenBuffers(1, &m_nVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);

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
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        MTL::Device* pDevice = Window::ins->getMetalDevice();
        m_pVertexBuffer = pDevice->newBuffer(arrVertices, sizeof(arrVertices), MTL::ResourceStorageModeShared);
    }
#endif // __APPLE__
}

bool Quad::getIsTransparent() const
{
    if (m_pShader)
    {
        return m_pShader->isTransparent();
    }
    return false;
}

void Quad::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the quad");

    mat4x4 mvp;
    const mat4x4& matModel = m_pNode->getWorldMatrix();
    const mat4x4& cameraViewMatrix = Camera::main->getViewProjectionMatrix();
    mat4x4_mul(mvp, cameraViewMatrix, matModel);

    if (Renderer::isUsingOpenGL())
    {
        glUseProgram(m_pShader->getProgram());
        glUniformMatrix4fv(m_pMVPHandle->m_nLocation, 1, GL_FALSE, (const GLfloat*) mvp);
        glUniform4f(m_pColorHandle->m_nLocation, m_color[0], m_color[1], m_color[2], m_color[3]);

        glUniform1i(m_pSpriteSheetXCountHandle->m_nLocation, m_vecSpriteSheetCount.x);
        glUniform1i(m_pSpriteSheetYCountHandle->m_nLocation, m_vecSpriteSheetCount.y);
        glUniform2f(m_pUVOffsetHandle->m_nLocation, m_vecUVOOffset[0], m_vecUVOOffset[1]);

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
        INCREASE_DRAW_CALL_COUNT(2);

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
        glBindVertexArray(0); // Unbind the vertex array
        glUseProgram(0);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        MTL::RenderCommandEncoder* pRenderCommandEncoder = Window::ins->getCurrentFrameRenderEncoder();

        pRenderCommandEncoder->setRenderPipelineState(m_pShader->getMetalPipelineState());
        pRenderCommandEncoder->setVertexBuffer(m_pVertexBuffer, 0, 0);

        struct
        {
            mat4x4 mvp;
            vec4 color;
            Vector2i spriteSheetCount;
            vec2 uvOffset;
        } uniforms;
        mat4x4_dup(uniforms.mvp, mvp);
        vec4_dup(uniforms.color, m_color);

        vec2_dup(uniforms.uvOffset, m_vecUVOOffset);
        uniforms.spriteSheetCount.x = m_vecSpriteSheetCount.x;
        uniforms.spriteSheetCount.y = m_vecSpriteSheetCount.y;

        pRenderCommandEncoder->setVertexBytes(&uniforms, sizeof(uniforms), 2);
        pRenderCommandEncoder->setFragmentBytes(&uniforms, sizeof(uniforms), 2);

        pRenderCommandEncoder->setFragmentTexture(m_pImage ? m_pImage->getMetalTexture() : ImageLoader::getInstance()->getPureWhite1by1Image()->getMetalTexture(), 0);
        pRenderCommandEncoder->setFragmentSamplerState(MetalRenderer::m_pLinearSampler, 0);

        pRenderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangleStrip, (NS::UInteger)0, (NS::UInteger)4);
        INCREASE_DRAW_CALL_COUNT(1);
    }
#endif // __APPLE__
}

void Quad::serializeToWrapper(DataSerializer& serializer) const
{
    serializer.ADD_ATTRIBUTES(m_fWidth);
    serializer.ADD_ATTRIBUTES(m_fHeight);
    serializer.ADD_ATTRIBUTES(m_color);
    serializer.ADD_ATTRIBUTES(m_pImage);
    serializer.ADD_ATTRIBUTES(m_pShader);
}

bool Quad::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    if (NodeComponent::deserializeField(deserializer, strFieldName, strFieldValue)) return true;
    DESERIALIZE_FIELD(m_fWidth);
    DESERIALIZE_FIELD(m_fHeight);
    DESERIALIZE_FIELD(m_color);
    DESERIALIZE_FIELD(m_pShader);
    DESERIALIZE_FIELD(m_pImage);
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
