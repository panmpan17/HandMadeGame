#include "gizmos.h"

#include <glad/gl.h>
// #include <imgui.h>

#include "editor.h"
#include "../engine/core/window.h"
#include "../engine/core/debug_macro.h"
#include "../engine/core/camera.h"
#include "../engine/core/scene/node.h"
#include "../engine/core/input/input_manager.h"
#include "../engine/render/shader_loader.h"
#include "../engine/render/image_loader.h"
#include "../engine/render/vertex.h"
#include "../engine/render/core/renderer.h"
#include "../engine/render/models/simple_obj_reader.h"

#define NS_INT(x) static_cast<NS::UInteger>(x)


GizmosManager::GizmosManager()
{
    initImageGizmosShaderAndBuffer();

    m_pMeshGizmosShader = ShaderLoader::getInstance()->getShader("mesh_gizmos");

    if (Renderer::isUsingOpenGL())
    {
        m_pMeshGizmosModelUniform = m_pMeshGizmosShader->getUniformHandle("u_Model");
        m_pMeshGizmosColorUniform = m_pMeshGizmosShader->getUniformHandle("u_GizmosColor");
    }

    initCircleGizmosShaderAndBuffer();
    initSphereGizmosShaderAndBuffer();
    initRectangleGizmosShaderAndBuffer();
    initCubeGizmosShaderAndBuffer();

    InputManager::getInstance()->registerMouseButtonCallback(MouseButton::BUTTON_LEFT, std::bind(&GizmosManager::onMouseClickCheck, this, std::placeholders::_1));
}

GizmosManager::~GizmosManager()
{
    if (Renderer::isUsingOpenGL())
    {
        glDeleteBuffers(1, &m_nCircleGizmosVertexBuffer);
        glDeleteVertexArrays(1, &m_nCircleGizmosVertexArray);

        glDeleteBuffers(1, &m_nSphereGizmosVertexBuffer);
        glDeleteVertexArrays(1, &m_nSphereGizmosVertexArray);

        glDeleteBuffers(1, &m_nRectangleGizmosVertexBuffer);
        glDeleteVertexArrays(1, &m_nRectangleGizmosVertexArray);

        glDeleteBuffers(1, &m_nCubeGizmosVertexBuffer);
        glDeleteVertexArrays(1, &m_nCubeGizmosVertexArray);

        glDeleteBuffers(1, &m_nImageGizmosVertexBuffer);
        glDeleteVertexArrays(1, &m_nImageGizmosVertexArray);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        if (m_pCircleGizmosMetalVertexBuffer)
        {
            m_pCircleGizmosMetalVertexBuffer->release();
            m_pCircleGizmosMetalVertexBuffer = nullptr;
        }

        if (m_pSphereGizmosMetalVertexBuffer)
        {
            m_pSphereGizmosMetalVertexBuffer->release();
            m_pSphereGizmosMetalVertexBuffer = nullptr;
        }

        if (m_pRectangleGizmosMetalVertexBuffer)
        {
            m_pRectangleGizmosMetalVertexBuffer->release();
            m_pRectangleGizmosMetalVertexBuffer = nullptr;
        }

        if (m_pImageGizmosMetalVertexBuffer)
        {
            m_pImageGizmosMetalVertexBuffer->release();
            m_pImageGizmosMetalVertexBuffer = nullptr;
        }
    }
#endif // __APPLE__
}

#pragma region Initialize Gizmos Required Resources

void GizmosManager::initImageGizmosShaderAndBuffer()
{
    m_pImageGizmosShader = ShaderLoader::getInstance()->getShader("simple_gizmos");
    constexpr float fStartX = -1 / 2.0f;
    constexpr float fStartY = -1 / 2.0f;

    VertexWUV arrVertices[4];
    arrVertices[0] = { { fStartX, fStartY }, { 0, 0}  }; // Bottom left
    arrVertices[2] = { { fStartX, fStartY + 1 }, { 0, 1 } }; // Top right
    arrVertices[1] = { { fStartX + 1, fStartY }, { 1, 0 } }; // Bottom right
    arrVertices[3] = { { fStartX + 1, fStartY + 1 }, { 1, 1 } }; // Top left

    if (Renderer::isUsingOpenGL())
    {
        m_pImageGizmosPositionUniform = m_pImageGizmosShader->getUniformHandle("u_WorldPosition");
        m_pImageGizmosColorUniform = m_pImageGizmosShader->getUniformHandle("u_imageColor");
        m_pImageGizmosTextureUniform = m_pImageGizmosShader->getUniformHandle(SHADER_UNIFORM_TEXTURE_0);
        m_pImageGizmosUseTextureUniform = m_pImageGizmosShader->getUniformHandle("u_useTexture");

        glGenBuffers(1, &m_nImageGizmosVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_nImageGizmosVertexBuffer);

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
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        MTL::Device* pDevice = Window::ins->getMetalDevice();
        m_pImageGizmosMetalVertexBuffer = pDevice->newBuffer(arrVertices, sizeof(arrVertices), MTL::ResourceStorageModeShared);
    }
#endif // __APPLE__
}

// General mesh buffer initialization functions
void GizmosManager::initMeshBufferUsingOpenGL(const std::string_view& strFilePath, int& nVertexCount, GLuint& nVertexBuffer, GLuint& nVertxArray)
{
    glGenBuffers(1, &nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, nVertexBuffer);

    std::vector<Vector3> vecSphereVertices;
    SimpleObjReader::readVertexBufferFromWavefrontFile(strFilePath, vecSphereVertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * vecSphereVertices.size(), vecSphereVertices.data(), GL_STATIC_DRAW);
    nVertexCount = static_cast<int>(vecSphereVertices.size());

    GLuint nVPosAttr = m_pMeshGizmosShader->getAttributeLocation("a_vPos");

    glGenVertexArrays(1, &nVertxArray);
    glBindVertexArray(nVertxArray);
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

#if __APPLE__
void GizmosManager::initMeshBufferUsingMetal(const std::string_view& strFilePath, int& nVertexCount, MTL::Buffer*& pMetalVertexBuffer)
{
    MTL::Device* pDevice = Window::ins->getMetalDevice();

    std::vector<Vector3> vecSphereVertices;
    SimpleObjReader::readVertexBufferFromWavefrontFile(strFilePath, vecSphereVertices);
    nVertexCount = static_cast<int>(vecSphereVertices.size());

    pMetalVertexBuffer = pDevice->newBuffer(
        vecSphereVertices.data(),
        sizeof(Vector3) * vecSphereVertices.size(),
        MTL::ResourceStorageModeShared);
}
#endif // __APPLE__

// Specific mesh gizmos initialization functions
void GizmosManager::initCircleGizmosShaderAndBuffer()
{
    constexpr std::string_view strFilePath = "assets/gizmos/circle.obj";

    if (Renderer::isUsingOpenGL())
    {
        initMeshBufferUsingOpenGL(strFilePath, m_nCircleGizmosVertexCount,
                                  m_nCircleGizmosVertexBuffer, m_nCircleGizmosVertexArray);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        initMeshBufferUsingMetal(strFilePath, m_nCircleGizmosVertexCount,
                                 m_pCircleGizmosMetalVertexBuffer);
    }
#endif // __APPLE__
}

void GizmosManager::initSphereGizmosShaderAndBuffer()
{
    constexpr std::string_view strFilePath = "assets/gizmos/sphere.obj";

    if (Renderer::isUsingOpenGL())
    {
        initMeshBufferUsingOpenGL(strFilePath, m_nSphereGizmosVertexCount,
                                  m_nSphereGizmosVertexBuffer, m_nSphereGizmosVertexArray);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        initMeshBufferUsingMetal(strFilePath, m_nSphereGizmosVertexCount,
                                 m_pSphereGizmosMetalVertexBuffer);
    }
#endif // __APPLE__
}

void GizmosManager::initRectangleGizmosShaderAndBuffer()
{
    constexpr std::string_view strFilePath = "assets/gizmos/rectangle.obj";

    if (Renderer::isUsingOpenGL())
    {
        initMeshBufferUsingOpenGL(strFilePath, m_nRectangleGizmosVertexCount,
                                  m_nRectangleGizmosVertexBuffer, m_nRectangleGizmosVertexArray);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        initMeshBufferUsingMetal(strFilePath, m_nRectangleGizmosVertexCount,
                                 m_pRectangleGizmosMetalVertexBuffer);
    }
#endif // __APPLE__
}

void GizmosManager::initCubeGizmosShaderAndBuffer()
{
    constexpr std::string_view strFilePath = "assets/gizmos/cube.obj";

    if (Renderer::isUsingOpenGL())
    {
        initMeshBufferUsingOpenGL(strFilePath, m_nCubeGizmosVertexCount,
                                  m_nCubeGizmosVertexBuffer, m_nCubeGizmosVertexArray);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        initMeshBufferUsingMetal(strFilePath, m_nCubeGizmosVertexCount,
                                 m_pCubeGizmosMetalVertexBuffer);
    }
#endif // __APPLE__
}

#pragma endregion


#pragma region Immediate Gizmos Registration

void GizmosManager::clearAllGizmos()
{
    m_nImageGizmosSize = 0;
    m_nCircleGizmosSize = 0;
    m_nSphereGizmosSize = 0;
    m_nRectangleGizmosSize = 0;
    m_nCubeGizmosSize = 0;
}

const Vector3 DEFAULT_IMAGE_GIZMOS_COLOR = Vector3(1, 1, 1);

void GizmosManager::addImageGizmos(NodeComponent* const pComponent, const Vector3& vecPosition, const std::string_view& m_strImagePath)
{
    addImageGizmos(pComponent, vecPosition, m_strImagePath, DEFAULT_IMAGE_GIZMOS_COLOR);
}

void GizmosManager::addImageGizmos(NodeComponent* const pComponent, const Vector3& vecPosition, const std::string_view& m_strImagePath, const Vector3& vecColor)
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

void GizmosManager::addCircleGizmos(const Vector3& vecPosition, const Quaternion& rotation, float fRadius)
{
    addCircleGizmos(vecPosition, rotation, fRadius, DEFAULT_GIZMOS_COLOR);
}
void GizmosManager::addCircleGizmos(const Vector3& vecPosition, const Quaternion& rotation, float fRadius, const Color& vecColor)
{
    if (m_nCircleGizmosSize + 1 < m_vecCircleGizmos.size())
    {
        CircleGizmosData& oData = m_vecCircleGizmos.at(m_nCircleGizmosSize++);
        oData.m_vecPosition = vecPosition;
        oData.m_fRadius = fRadius;
        oData.m_oRotation = rotation;
        oData.m_color = vecColor;
        return;
    }

    m_vecCircleGizmos.push_back(CircleGizmosData {
        .m_vecPosition = vecPosition,
        .m_fRadius = fRadius,
        .m_oRotation = rotation,
        .m_color = vecColor,
    });
}

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

void GizmosManager::addRectangleGizmos(const Vector3& vecPosition, const Quaternion& rotation, const Vector2& vecSize)
{
    addRectangleGizmos(vecPosition, rotation, vecSize, DEFAULT_GIZMOS_COLOR);
}
void GizmosManager::addRectangleGizmos(const Vector3& vecPosition, const Quaternion& rotation, const Vector2& vecSize, const Color& vecColor)
{
    if (m_nRectangleGizmosSize + 1 < m_vecRectangleGizmos.size())
    {
        RectangleGizmosData& oData = m_vecRectangleGizmos.at(m_nRectangleGizmosSize++);
        oData.m_vecPosition = vecPosition;
        oData.m_vecSize = vecSize;
        oData.m_oRotation = rotation;
        oData.m_color = vecColor;
        return;
    }
    m_vecRectangleGizmos.push_back(RectangleGizmosData {
        .m_vecPosition = vecPosition,
        .m_vecSize = vecSize,
        .m_oRotation = rotation,
        .m_color = vecColor,
    });
}

void GizmosManager::addCubeGizmos(const Vector3& vecPosition, const Quaternion& rotation, const Vector3& vecSize)
{
    addCubeGizmos(vecPosition, rotation, vecSize, DEFAULT_GIZMOS_COLOR);
}
void GizmosManager::addCubeGizmos(const Vector3& vecPosition, const Quaternion& rotation, const Vector3& vecSize, const Color& vecColor)
{
    if (m_nCubeGizmosSize + 1 < m_vecCubeGizmos.size())
    {
        CubeGizmosData& oData = m_vecCubeGizmos.at(m_nCubeGizmosSize++);
        oData.m_vecPosition = vecPosition;
        oData.m_vecSize = vecSize;
        oData.m_oRotation = rotation;
        oData.m_color = vecColor;
        return;
    }

    m_vecCubeGizmos.push_back(CubeGizmosData {
        .m_vecPosition = vecPosition,
        .m_vecSize = vecSize,
        .m_oRotation = rotation,
        .m_color = vecColor,
    });
}

#pragma endregion


#pragma region Draw Gizmos

void GizmosManager::drawAllGizmos()
{
    if (Renderer::isUsingOpenGL())
    {
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        MTL::RenderCommandEncoder* pRenderCommandEncoder = Window::ins->getCurrentFrameRenderEncoder();
        pRenderCommandEncoder->setDepthStencilState(MetalRenderer::m_pDepthOffStencilState);
    }
#endif


    drawCircleGizmos();
    drawSphereGizmos();
    drawRectangleGizmos();
    drawCubeGizmos();
    drawImageGizmos();

    if (Renderer::isUsingOpenGL())
    {
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        MTL::RenderCommandEncoder* pRenderCommandEncoder = Window::ins->getCurrentFrameRenderEncoder();
        pRenderCommandEncoder->setDepthStencilState(MetalRenderer::m_pDepthOnStencilState);
    }
#endif
}

void GizmosManager::drawCircleGizmos()
{
    mat4x4 oScaleMatrix;
    mat4x4 oRotationMatrix;
    mat4x4 oModelMatrix;

    if (Renderer::isUsingOpenGL())
    {
        glDisable(GL_CULL_FACE);

        glUseProgram(m_pMeshGizmosShader->getProgram());
        glBindVertexArray(m_nCircleGizmosVertexArray);

        for (int i = 0; i < m_nCircleGizmosSize; ++i)
        {
            CircleGizmosData& oData = m_vecCircleGizmos.at(i);

            mat4x4_translate(oModelMatrix, oData.m_vecPosition.x, oData.m_vecPosition.y, oData.m_vecPosition.z);
            
            mat4x4_identity(oScaleMatrix);
            mat4x4_scale(oScaleMatrix, oScaleMatrix, oData.m_fRadius);

            oData.m_oRotation.toMat4x4(oRotationMatrix);

            mat4x4_mul(oRotationMatrix, oRotationMatrix, oScaleMatrix);
            mat4x4_mul(oModelMatrix, oModelMatrix, oRotationMatrix);

            glUniformMatrix4fv(m_pMeshGizmosModelUniform->m_nLocation, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(oModelMatrix));
            glUniform4f(m_pMeshGizmosColorUniform->m_nLocation, oData.m_color.r, oData.m_color.g, oData.m_color.b, oData.m_color.a);

            glDrawArrays(GL_TRIANGLES, 0, m_nCircleGizmosVertexCount);

            INCREASE_DRAW_CALL_COUNT(m_nCircleGizmosVertexCount / 3);
        }

        glBindVertexArray(0);
        glUseProgram(0);

        glEnable(GL_CULL_FACE);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        MTL::RenderCommandEncoder* pRenderCommandEncoder = Window::ins->getCurrentFrameRenderEncoder();

        pRenderCommandEncoder->setRenderPipelineState(m_pMeshGizmosShader->getMetalPipelineState());
        pRenderCommandEncoder->setVertexBuffer(m_pCircleGizmosMetalVertexBuffer, 0, 0);
        pRenderCommandEncoder->setVertexBuffer(Camera::main->getCameraMetalUBO(), 0, 1);

        for (int i = 0; i < m_nCircleGizmosSize; ++i)
        {
            CircleGizmosData& oData = m_vecCircleGizmos.at(i);

            mat4x4_translate(oModelMatrix, oData.m_vecPosition.x, oData.m_vecPosition.y, oData.m_vecPosition.z);
            
            mat4x4_identity(oScaleMatrix);
            mat4x4_scale(oScaleMatrix, oScaleMatrix, oData.m_fRadius);

            oData.m_oRotation.toMat4x4(oRotationMatrix);

            mat4x4_mul(oRotationMatrix, oRotationMatrix, oScaleMatrix);
            mat4x4_mul(oModelMatrix, oModelMatrix, oRotationMatrix);

            pRenderCommandEncoder->setVertexBytes(&oModelMatrix, sizeof(oModelMatrix), 2);
            pRenderCommandEncoder->setFragmentBytes(&oData.m_color, sizeof(Color), 0);

            pRenderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS_INT(0), NS_INT(m_nCircleGizmosVertexCount));
            INCREASE_DRAW_CALL_COUNT(m_nCircleGizmosVertexCount / 3);
        }
    }
#endif // __APPLE__
}

void GizmosManager::drawSphereGizmos()
{
    mat4x4 oScaleMatrix;
    mat4x4 oRotationMatrix;
    Quaternion::Identity.toMat4x4(oRotationMatrix);
    mat4x4 oModelMatrix;

    if (Renderer::isUsingOpenGL())
    {
        glUseProgram(m_pMeshGizmosShader->getProgram());
        glBindVertexArray(m_nSphereGizmosVertexArray);

        for (int i = 0; i < m_nSphereGizmosSize; ++i)
        {
            SphereGizmosData& oData = m_vecSphereGizmos.at(i);

            mat4x4_translate(oModelMatrix, oData.m_vecPosition.x, oData.m_vecPosition.y, oData.m_vecPosition.z);
            
            mat4x4_identity(oScaleMatrix);
            mat4x4_scale(oScaleMatrix, oScaleMatrix, oData.m_fRadius);

            mat4x4_mul(oModelMatrix, oModelMatrix, oScaleMatrix);

            glUniformMatrix4fv(m_pMeshGizmosModelUniform->m_nLocation, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(oModelMatrix));
            glUniform4f(m_pMeshGizmosColorUniform->m_nLocation, oData.m_color.r, oData.m_color.g, oData.m_color.b, oData.m_color.a);

            glDrawArrays(GL_TRIANGLES, 0, m_nSphereGizmosVertexCount);

            INCREASE_DRAW_CALL_COUNT(m_nSphereGizmosVertexCount / 3);
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        MTL::RenderCommandEncoder* pRenderCommandEncoder = Window::ins->getCurrentFrameRenderEncoder();

        pRenderCommandEncoder->setRenderPipelineState(m_pMeshGizmosShader->getMetalPipelineState());
        pRenderCommandEncoder->setVertexBuffer(m_pSphereGizmosMetalVertexBuffer, 0, 0);
        pRenderCommandEncoder->setVertexBuffer(Camera::main->getCameraMetalUBO(), 0, 1);

        for (int i = 0; i < m_nSphereGizmosSize; ++i)
        {
            SphereGizmosData& oData = m_vecSphereGizmos.at(i);

            mat4x4_translate(oModelMatrix, oData.m_vecPosition.x, oData.m_vecPosition.y, oData.m_vecPosition.z);
            
            mat4x4_identity(oScaleMatrix);
            mat4x4_scale(oScaleMatrix, oScaleMatrix, oData.m_fRadius);

            mat4x4_mul(oModelMatrix, oModelMatrix, oScaleMatrix);

            pRenderCommandEncoder->setVertexBytes(&oModelMatrix, sizeof(oModelMatrix), 2);
            pRenderCommandEncoder->setFragmentBytes(&oData.m_color, sizeof(Color), 0);

            pRenderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS_INT(0), NS_INT(m_nSphereGizmosVertexCount));
            INCREASE_DRAW_CALL_COUNT(m_nSphereGizmosVertexCount / 3);
        }
    }
#endif
}

void GizmosManager::drawRectangleGizmos()
{
    mat4x4 oScaleMatrix;
    mat4x4 oRotationMatrix;
    mat4x4 oModelMatrix;

    if (Renderer::isUsingOpenGL())
    {
        glDisable(GL_CULL_FACE);

        glUseProgram(m_pMeshGizmosShader->getProgram());
        glBindVertexArray(m_nRectangleGizmosVertexArray);

        for (int i = 0; i < m_nRectangleGizmosSize; ++i)
        {
            RectangleGizmosData& oData = m_vecRectangleGizmos.at(i);

            mat4x4_translate(oModelMatrix, oData.m_vecPosition.x, oData.m_vecPosition.y, oData.m_vecPosition.z);
            
            mat4x4_identity(oScaleMatrix);
            mat4x4_scale_aniso(oScaleMatrix, oScaleMatrix, oData.m_vecSize.x, oData.m_vecSize.y, 1.0f);

            oData.m_oRotation.toMat4x4(oRotationMatrix);

            mat4x4_mul(oRotationMatrix, oRotationMatrix, oScaleMatrix);
            mat4x4_mul(oModelMatrix, oModelMatrix, oRotationMatrix);

            glUniformMatrix4fv(m_pMeshGizmosModelUniform->m_nLocation, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(oModelMatrix));
            glUniform4f(m_pMeshGizmosColorUniform->m_nLocation, oData.m_color.r, oData.m_color.g, oData.m_color.b, oData.m_color.a);

            glDrawArrays(GL_TRIANGLES, 0, m_nRectangleGizmosVertexCount);

            INCREASE_DRAW_CALL_COUNT(m_nRectangleGizmosVertexCount / 3);
        }

        glBindVertexArray(0);
        glUseProgram(0);

        glEnable(GL_CULL_FACE);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        MTL::RenderCommandEncoder* pRenderCommandEncoder = Window::ins->getCurrentFrameRenderEncoder();

        pRenderCommandEncoder->setRenderPipelineState(m_pMeshGizmosShader->getMetalPipelineState());
        pRenderCommandEncoder->setVertexBuffer(m_pRectangleGizmosMetalVertexBuffer, 0, 0);
        pRenderCommandEncoder->setVertexBuffer(Camera::main->getCameraMetalUBO(), 0, 1);

        for (int i = 0; i < m_nRectangleGizmosSize; ++i)
        {
            RectangleGizmosData& oData = m_vecRectangleGizmos.at(i);

            mat4x4_translate(oModelMatrix, oData.m_vecPosition.x, oData.m_vecPosition.y, oData.m_vecPosition.z);
            
            mat4x4_identity(oScaleMatrix);
            mat4x4_scale_aniso(oScaleMatrix, oScaleMatrix, oData.m_vecSize.x, oData.m_vecSize.y, 1.0f);

            oData.m_oRotation.toMat4x4(oRotationMatrix);

            mat4x4_mul(oRotationMatrix, oRotationMatrix, oScaleMatrix);
            mat4x4_mul(oModelMatrix, oModelMatrix, oRotationMatrix);

            pRenderCommandEncoder->setVertexBytes(&oModelMatrix, sizeof(oModelMatrix), 2);
            pRenderCommandEncoder->setFragmentBytes(&oData.m_color, sizeof(Color), 0);

            pRenderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS_INT(0), NS_INT(m_nRectangleGizmosVertexCount));
            INCREASE_DRAW_CALL_COUNT(m_nRectangleGizmosVertexCount / 3);
        }
    }
#endif // __APPLE__
}

void GizmosManager::drawCubeGizmos()
{
    mat4x4 oScaleMatrix;
    mat4x4 oRotationMatrix;
    mat4x4 oModelMatrix;

    if (Renderer::isUsingOpenGL())
    {
        glUseProgram(m_pMeshGizmosShader->getProgram());
        glBindVertexArray(m_nCubeGizmosVertexArray);

        for (int i = 0; i < m_nCubeGizmosSize; ++i)
        {
            CubeGizmosData& oData = m_vecCubeGizmos.at(i);

            mat4x4_translate(oModelMatrix, oData.m_vecPosition.x, oData.m_vecPosition.y, oData.m_vecPosition.z);
            
            mat4x4_identity(oScaleMatrix);
            mat4x4_scale_aniso(oScaleMatrix, oScaleMatrix, oData.m_vecSize.x, oData.m_vecSize.y, oData.m_vecSize.z);

            oData.m_oRotation.toMat4x4(oRotationMatrix);

            mat4x4_mul(oRotationMatrix, oRotationMatrix, oScaleMatrix);
            mat4x4_mul(oModelMatrix, oModelMatrix, oRotationMatrix);

            glUniformMatrix4fv(m_pMeshGizmosModelUniform->m_nLocation, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(oModelMatrix));
            glUniform4f(m_pMeshGizmosColorUniform->m_nLocation, oData.m_color.r, oData.m_color.g, oData.m_color.b, oData.m_color.a);

            glDrawArrays(GL_TRIANGLES, 0, m_nCubeGizmosVertexCount);

            INCREASE_DRAW_CALL_COUNT(m_nCubeGizmosVertexCount / 3);
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        MTL::RenderCommandEncoder* pRenderCommandEncoder = Window::ins->getCurrentFrameRenderEncoder();

        pRenderCommandEncoder->setRenderPipelineState(m_pMeshGizmosShader->getMetalPipelineState());
        pRenderCommandEncoder->setVertexBuffer(m_pCubeGizmosMetalVertexBuffer, 0, 0);
        pRenderCommandEncoder->setVertexBuffer(Camera::main->getCameraMetalUBO(), 0, 1);

        for (int i = 0; i < m_nCubeGizmosSize; ++i)
        {
            CubeGizmosData& oData = m_vecCubeGizmos.at(i);

            mat4x4_translate(oModelMatrix, oData.m_vecPosition.x, oData.m_vecPosition.y, oData.m_vecPosition.z);
            
            mat4x4_identity(oScaleMatrix);
            mat4x4_scale_aniso(oScaleMatrix, oScaleMatrix, oData.m_vecSize.x, oData.m_vecSize.y, oData.m_vecSize.z);

            oData.m_oRotation.toMat4x4(oRotationMatrix);

            mat4x4_mul(oRotationMatrix, oRotationMatrix, oScaleMatrix);
            mat4x4_mul(oModelMatrix, oModelMatrix, oRotationMatrix);

            pRenderCommandEncoder->setVertexBytes(&oModelMatrix, sizeof(oModelMatrix), 2);
            pRenderCommandEncoder->setFragmentBytes(&oData.m_color, sizeof(Color), 0);

            pRenderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, NS_INT(0), NS_INT(m_nCubeGizmosVertexCount));
            INCREASE_DRAW_CALL_COUNT(m_nCubeGizmosVertexCount / 3);
        }
    }
#endif // __APPLE__
}

void GizmosManager::drawImageGizmos()
{
    if (Renderer::isUsingOpenGL())
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

                INCREASE_DRAW_CALL_COUNT(2);
            }
        }

        glBindVertexArray(0); // Unbind the vertex array
        glUseProgram(0);
    }
#if __APPLE__
    else if (Renderer::isUsingMetal())
    {
        MTL::RenderCommandEncoder* pRenderCommandEncoder = Window::ins->getCurrentFrameRenderEncoder();

        pRenderCommandEncoder->setRenderPipelineState(m_pImageGizmosShader->getMetalPipelineState());
        pRenderCommandEncoder->setVertexBuffer(m_pImageGizmosMetalVertexBuffer, 0, 0);
        pRenderCommandEncoder->setVertexBuffer(Camera::main->getCameraMetalUBO(), 0, 3);

        for (int i = 0; i < m_nImageGizmosSize; ++i)
        {
            ImageGizmosData& oData = m_vecImageGizmos.at(i);

            if (Image* pImage = ImageLoader::getInstance()->getImageByPath(oData.m_strImagePath); pImage != nullptr)
            {
                struct
                {
                    Vector3 position;
                    float padding;
                    Color color;
                } uniforms;
                uniforms.position = oData.m_vecPosition;
                uniforms.color.r = oData.m_vecColor.x;
                uniforms.color.g = oData.m_vecColor.y;
                uniforms.color.b = oData.m_vecColor.z;
                uniforms.color.a = 1.0f;

                // LOGLN("Drawing Image Gizmos at Position: {:.2f} {:.2f} {:.2f}", uniforms.position.x, uniforms.position.y, uniforms.position.z);

                pRenderCommandEncoder->setVertexBytes(&uniforms, sizeof(uniforms), 2);
                pRenderCommandEncoder->setFragmentBytes(&uniforms, sizeof(uniforms), 2);

                MTL::Texture* pTexture = pImage ? pImage->getMetalTexture() : ImageLoader::getInstance()->getPureWhite1by1Image()->getMetalTexture();
                pRenderCommandEncoder->setFragmentTexture(pTexture, 0);
                pRenderCommandEncoder->setFragmentSamplerState(MetalRenderer::m_pLinearSampler, 0);

                pRenderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangleStrip, (NS::UInteger)0, (NS::UInteger)4);
                INCREASE_DRAW_CALL_COUNT(1);
            }
        }
    }
#endif // __APPLE__
}

#pragma endregion

void GizmosManager::onMouseClickCheck(bool bPressed)
{
    if (!bPressed) return;

    // ImGuiIO& io = ImGui::GetIO();
    // if (io.WantCaptureMouse) return;

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
