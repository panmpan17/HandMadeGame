#include "render_process_queue.h"

#include <glad/gl.h>
#include "../window.h"
#include "../draw/vertex.h"
#include "../draw/shader.h"
#include "../draw/shader_loader.h"
#include "../input_handle.h"
#include "bloom_test.h"
#include "order_dithering.h"
#include "difference_of_gaussian.h"

void IRenderProcess::registerShaderPosAndUV(Shader* pShader)
{
    GLuint nVPosAttr = pShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr = pShader->getAttributeLocation("a_vUV");
    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(nVUVAttr);
    glVertexAttribPointer(nVUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));
}

void IRenderProcess::initializeRenderTextureAndFBO(GLuint& nFBO, GLuint& nTexture, int nWidth, int nHeight)
{
    glGenFramebuffers(1, &nFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, nFBO);

    glGenTextures(1, &nTexture);
    glBindTexture(GL_TEXTURE_2D, nTexture);

    // Set the texture's format and size to match your window
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nWidth, nHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // Set texture parameters for correct filtering and wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Attach the texture to the FBO's color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGERR("Framebuffer is not complete!");
    }
}



RenderProcessQueue::RenderProcessQueue(Window* pWindow)
{
    m_pWindow = pWindow;

    init(pWindow->GetActualWidth(), pWindow->GetActualHeight());
}

RenderProcessQueue::~RenderProcessQueue()
{
}

int RenderProcessQueue::getActualWidth() const { return m_pWindow->GetActualWidth(); }
int RenderProcessQueue::getActualHeight() const { return m_pWindow->GetActualHeight(); }

void RenderProcessQueue::init(int nWidth, int nHeight)
{
    m_nRenderWidth = nWidth;
    m_nRenderHeight = nHeight;

    initializeQuad();
    initializeOriginalFBO();
}

void RenderProcessQueue::initializeQuad()
{
    m_pShader = ShaderLoader::getInstance()->getShader("pure_texture");
    m_nTextureUniform = m_pShader->getUniformLocation("u_tex0");

    m_pSplitShader = ShaderLoader::getInstance()->getShader("split_texture");
    m_nOriginalTextureUniform_Split = m_pSplitShader->getUniformLocation("u_tex0");
    m_nFinalTextureUniform_Split = m_pSplitShader->getUniformLocation("u_tex1");
    m_nSplitFactorUniform = m_pSplitShader->getUniformLocation("u_splitFactor");

    glGenBuffers(1, &m_nVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_nVertexBuffer);

    VertexWUV arrVertices[4];
    arrVertices[0] = { { -1, -1 }, { 0.0f, 0.0f } };
    arrVertices[1] = { { 1, -1 }, { 1.0f, 0.0f } };
    arrVertices[2] = { { -1, 1 }, { 0.0f, 1.0f } };
    arrVertices[3] = { { 1, 1 }, { 1.0f, 1.0f } };
    glBufferData(GL_ARRAY_BUFFER, sizeof(arrVertices), arrVertices, GL_STATIC_DRAW);

    GLuint nVPosAttr = m_pShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr = m_pShader->getAttributeLocation("a_vUV");

    GLuint nVPosAttr_Split = m_pSplitShader->getAttributeLocation("a_vPos");
    GLuint nVUVAttr_Split = m_pSplitShader->getAttributeLocation("a_vUV");

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);

    glEnableVertexAttribArray(nVPosAttr);
    glVertexAttribPointer(nVPosAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(nVUVAttr);
    glVertexAttribPointer(nVUVAttr, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

    glEnableVertexAttribArray(nVPosAttr_Split);
    glVertexAttribPointer(nVPosAttr_Split, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, pos));
    glEnableVertexAttribArray(nVUVAttr_Split);
    glVertexAttribPointer(nVUVAttr_Split, 2, GL_FLOAT, GL_FALSE, sizeof(VertexWUV), (void*)offsetof(VertexWUV, uv));

    // Unbind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderProcessQueue::initializeOriginalFBO()
{
    glGenFramebuffers(1, &m_nFBOID_original);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_original);

    glGenTextures(1, &m_nRenderTexture_original);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_original);

    // Set the texture's format and size to match your window
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_nRenderWidth, m_nRenderHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    // Set texture parameters for correct filtering and wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Attach the texture to the FBO's color attachment
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_nRenderTexture_original, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOGERR("Framebuffer is not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderProcessQueue::setupProcesses()
{
    /*
    auto pBloomTest = new BloomTest(this);
    pBloomTest->initialize();
    m_oProcessArray.addElement(pBloomTest);
    // BloomTest::ins = pBloomTest;

    auto pInput = InputManager::getInstance();
    pInput->registerKeyPressCallback(KeyCode::KEY_ARROW_UP, [pBloomTest](bool pressed) {
        if (!pressed) { return; }

        bool bShiftPressed = InputManager::getInstance()->isKeyPressed(KeyCode::KEY_LEFT_SHIFT) ||
                               InputManager::getInstance()->isKeyPressed(KeyCode::KEY_RIGHT_SHIFT);
        bool bMetaPressed = InputManager::getInstance()->isKeyPressed(KeyCode::KEY_LEFT_META) ||
                              InputManager::getInstance()->isKeyPressed(KeyCode::KEY_RIGHT_META);

        if (!bShiftPressed && !bMetaPressed)
        {
            float fIntensity = pBloomTest->getIntensity();
            fIntensity += 0.1f;
            if (fIntensity > 5.0f) fIntensity = 5.0f;
            pBloomTest->setIntensity(fIntensity);
        }
        else if (bShiftPressed && !bMetaPressed)
        {
            float fRadius = pBloomTest->getBlurRadius();
            fRadius += 1.0f;
            if (fRadius > 20.0f) fRadius = 20.0f;
            pBloomTest->setBlurRadius(fRadius);
        }
        else if (!bShiftPressed && bMetaPressed)
        {
            float fSigma = pBloomTest->getBlurSigma();
            fSigma += 1.0f;
            if (fSigma > 20.0f) fSigma = 20.0f;
            pBloomTest->setBlurSigma(fSigma);
        }
    });
    pInput->registerKeyPressCallback(KeyCode::KEY_ARROW_DOWN, [pBloomTest](bool pressed) {
        if (!pressed) { return; }

        bool bShiftPressed = InputManager::getInstance()->isKeyPressed(KeyCode::KEY_LEFT_SHIFT) ||
                               InputManager::getInstance()->isKeyPressed(KeyCode::KEY_RIGHT_SHIFT);
        bool bMetaPressed = InputManager::getInstance()->isKeyPressed(KeyCode::KEY_LEFT_META) ||
                              InputManager::getInstance()->isKeyPressed(KeyCode::KEY_RIGHT_META);

        // LOGLN_EX("Shift: {}, Meta: {}", bShiftPressed, bMetaPressed);
        if (!bShiftPressed && !bMetaPressed)
        {
            float fIntensity = pBloomTest->getIntensity();
            fIntensity -= 0.1f;
            if (fIntensity < 0.0f) fIntensity = 0.0f;
            pBloomTest->setIntensity(fIntensity);
        }
        else if (bShiftPressed && !bMetaPressed)
        {
            float fRadius = pBloomTest->getBlurRadius();
            fRadius -= 1.0f;
            if (fRadius < 0.0f) fRadius = 0.0f;
            pBloomTest->setBlurRadius(fRadius);
        }
        else if (!bShiftPressed && bMetaPressed)
        {
            float fSigma = pBloomTest->getBlurSigma();
            fSigma -= 1.0f;
            if (fSigma < 0.0f) fSigma = 0.0f;
            pBloomTest->setBlurSigma(fSigma);
        }
    });

    // auto pOrderDithering = new OrderDithering(this);
    // pOrderDithering->initialize();
    // m_oProcessArray.addElement(pOrderDithering);
    */

    auto pDifferenceOfGaussian = new DifferenceOfGaussian(this);
    pDifferenceOfGaussian->initialize();
    m_oProcessArray.addElement(pDifferenceOfGaussian);

    auto pOrderDithering = new OrderDithering(this);
    pOrderDithering->initialize();
    m_oProcessArray.addElement(pOrderDithering);
}

#pragma mark Drawing every frame
void RenderProcessQueue::beginFrame()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_nFBOID_original);

    glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);

    glClearColor(0.f, 0.f, 0.f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void RenderProcessQueue::endFrame()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderProcessQueue::startProcessing()
{
    m_nFinalRenderTexture = m_nRenderTexture_original;
    
    int nSize = m_oProcessArray.getSize();
    for (int i = 0; i < nSize; ++i)
    {
        IRenderProcess* pProcess = m_oProcessArray.getElement(i);
        if (pProcess)
        {
            pProcess->renderProcess();
        }
    }
}

void RenderProcessQueue::renderToScreen()
{
    glViewport(0, 0, m_pWindow->GetActualWidth(), m_pWindow->GetActualHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_pShader->getProgram());

    glUniform1i(m_nTextureUniform, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_nFinalRenderTexture);

    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}

void RenderProcessQueue::renderToScreenSplit()
{
    glViewport(0, 0, m_pWindow->GetActualWidth(), m_pWindow->GetActualHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_pSplitShader->getProgram());

    glUniform1i(m_nFinalTextureUniform_Split, 0);
    glUniform1i(m_nOriginalTextureUniform_Split, 1);
    glUniform1f(m_nSplitFactorUniform, 0.5f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_nRenderTexture_original);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_nFinalRenderTexture);

    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); // Draw the quad using triangle strip
    INCREASE_DRAW_CALL_COUNT();

    glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture
    glBindVertexArray(0); // Unbind the vertex array
    glUseProgram(0);
}
