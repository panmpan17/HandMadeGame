#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "triangle.h"
#include "../../render/shader.h"
#include "../../render/shader_loader.h"
#include "../../core/camera.h"
#include "../../core/window.h"
#include "../../core/debug_macro.h"
#include "../../core/scene/node.h"
#include "../../core/serialization/serializer.h"
#include "../../../editor/gizmos.h"


Triangle::Triangle()
{
}

Triangle::~Triangle()
{
    if (Window::ins->isUsingOpenGL())
    {
        glDeleteBuffers(1, &m_nVertexBuffer);
        glDeleteVertexArrays(1, &m_nVertexArray);
    }
}

void Triangle::setShader(Shader* pShader)
{
    m_pShader = pShader;

    if (Window::ins->isUsingOpenGL())
    {
        m_pMVPHandle = m_pShader->getUniformHandle(SHADER_UNIFORM_MVP);
    }
}

void Triangle::registerBuffer()
{
    
    if (Window::ins->isUsingOpenGL())
    {
        glGenBuffers(1, &m_nVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);

        VertexWColor arrVertices[3];
        arrVertices[0] = { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } };
        arrVertices[1] = { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } };
        arrVertices[2] = { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } };
        glBufferData(GL_ARRAY_BUFFER, sizeof(arrVertices), arrVertices, GL_STATIC_DRAW);

        GLuint nVPosAttr =  m_pShader->getAttributeLocation("a_vPos");
        GLuint nVColAttr =  m_pShader->getAttributeLocation("a_vCol");

        glGenVertexArrays(1, &m_nVertexArray);
        glBindVertexArray(m_nVertexArray);
        glEnableVertexAttribArray(nVPosAttr);
        glVertexAttribPointer(nVPosAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWColor), (void*)offsetof(VertexWColor, pos));
        glEnableVertexAttribArray(nVColAttr);
        glVertexAttribPointer(nVColAttr, 3, GL_FLOAT, GL_FALSE, sizeof(VertexWColor), (void*)offsetof(VertexWColor, col));

        // Unbind
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        MTL::Device* pDevice = Window::ins->getMetalDevice();

        const float positions[] = {
            -0.6f, -0.4f,
            0.6f, -0.4f,
            0.f,  0.6f
        };
        const float colors[] = {
            1.f, 0.f, 0.f,
            0.f, 1.f, 0.f,
            0.f, 0.f, 1.f,
        };

        m_pPosBuffer = pDevice->newBuffer(positions, sizeof(positions), MTL::ResourceStorageModeShared);
        m_pColBuffer = pDevice->newBuffer(colors, sizeof(colors), MTL::ResourceStorageModeShared);
    }
#endif // __APPLE__
}

void Triangle::draw()
{
    ASSERT(m_pShader, "Shader must be set before drawing the triangle");

    mat4x4 mvp;
    const mat4x4& matModel = m_pNode->getWorldMatrix();
    const mat4x4& cameraViewMatrix = Camera::main->getViewProjectionMatrix();
    mat4x4_mul(mvp, cameraViewMatrix, matModel);

    if (Window::ins->isUsingOpenGL())
    {
        glUseProgram(m_pShader->getProgram());
        glUniformMatrix4fv(m_pMVPHandle->m_nLocation, 1, GL_FALSE, (const GLfloat*) mvp);
        glBindVertexArray(m_nVertexArray);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        INCREASE_DRAW_CALL_COUNT(1);
        glUseProgram(0);
    }
#if __APPLE__
    else if (Window::ins->isUsingMetal())
    {
        MTL::RenderCommandEncoder* pRenderCommandEncoder = Window::ins->getCurrentFrameRenderEncoder();

        pRenderCommandEncoder->setRenderPipelineState(m_pShader->getMetalPipelineState());
        pRenderCommandEncoder->setVertexBuffer(m_pPosBuffer, 0, 0);
        pRenderCommandEncoder->setVertexBuffer(m_pColBuffer, 0, 1);
        pRenderCommandEncoder->setVertexBytes(&mvp, sizeof(mat4x4), 2);
        pRenderCommandEncoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, (NS::UInteger)0, (NS::UInteger)3);
        INCREASE_DRAW_CALL_COUNT(1);
    }
#endif // __APPLE__
}

void Triangle::serializeToWrapper(DataSerializer& serializer) const
{
    serializer.ADD_ATTRIBUTES(m_pShader);
}

bool Triangle::deserializeField(DataDeserializer& deserializer, const std::string_view& strFieldName, const std::string_view& strFieldValue)
{
    DESERIALIZE_FIELD(m_pShader);
    return false;
}

void Triangle::onNodeFinishedDeserialization()
{
    if (m_pShader)
    {
        setShader(m_pShader);
        registerBuffer();
    }
}

inline constexpr std::string_view GIZMOS_IMAGE = "assets/gizmos/particle.png";

void Triangle::onDrawGizmos(bool bIsSelected)
{
    GizmosManager::getInstance()->addImageGizmos(this, m_pNode->getPositionInWorld(), GIZMOS_IMAGE,
                                                 Vector3(1, 0, 0));
    // GizmosManager::getInstance()->addSphereGizmos(
    //             m_pNode->getPositionInWorld(),
    //             1);
}
