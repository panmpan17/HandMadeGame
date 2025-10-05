#pragma once

#include "../expandable_array.h"
#include "../serialization/type_registry.h"

typedef unsigned int GLuint;

class Shader;
class Window;

class RenderProcessQueue;

class IRenderProcess
{
public:
    IRenderProcess(RenderProcessQueue* pQueue) : m_pProcessQueue(pQueue) {}
    virtual ~IRenderProcess() = default;

    virtual void renderProcess() = 0;

    inline bool isActive() const { return m_bActive; }
    inline void setActive(bool bActive) { m_bActive = bActive; }

protected:
    RenderProcessQueue* m_pProcessQueue = nullptr;
    bool m_bActive = true;

    virtual void initialize() = 0;

    static void registerShaderPosAndUV(Shader* pShader);
    static void initializeRenderTextureAndFBO(GLuint& nFBO, GLuint& nTexture, int nWidth, int nHeight);
};


class RenderProcessQueue
{
public:
    static RenderProcessQueue* ins;

    RenderProcessQueue(Window* pWindow);
    ~RenderProcessQueue();

    // void resize(int width, int height);
    void setupProcesses();

    void beginFrame();
    void endFrame();

    void startProcessing();
    void renderToScreen();
    void renderToScreenSplit();

    inline int getRenderWidth() const { return m_nRenderWidth; }
    inline int getRenderHeight() const { return m_nRenderHeight; }
    int getActualWidth() const;
    int getActualHeight() const;

    inline GLuint getOriginalRenderTexture() const { return m_nRenderTexture_original; }
    inline GLuint getFinalRenderTexture() const { return m_nFinalRenderTexture; }
    inline void setFinalRenderTexture(GLuint texture) { m_nFinalRenderTexture = texture; }

    inline GLuint getFullScreenVertexArray() const { return m_nVertexArray; }
    inline GLuint getFullScreenVertexBuffer() const { return m_nVertexBuffer; }

    inline int getProcessCount() const { return m_oProcessArray.getSize(); }
    inline IRenderProcess* getProcessAt(int index) const { return m_oProcessArray.getElement(index); }
    inline int addProcess(IRenderProcess* pProcess) { return m_oProcessArray.addElement(pProcess); }
    inline int getProcessIndex(IRenderProcess* pProcess) const { return m_oProcessArray.getElementIndex(pProcess); }
    inline void swapProcesses(int index1, int index2) { m_oProcessArray.swap(index1, index2); }

    inline bool isSplitScreen() const { return m_bSplitScreen; }
    inline void setSplitScreen(bool bSplit) { m_bSplitScreen = bSplit; }

    inline float getSplitFactor() const { return m_fSplitFactor; }
    inline void setSplitFactor(float fFactor) { m_fSplitFactor = fFactor; }

private:
    Window* m_pWindow = nullptr;

    int m_nRenderWidth = 0;
    int m_nRenderHeight = 0;

    GLuint m_nFBOID_original = 0;
    GLuint m_nRenderTexture_original = 0;

    Shader* m_pShader = nullptr;
    GLuint m_nTextureUniform = 0;

    Shader* m_pSplitShader = nullptr;
    GLuint m_nOriginalTextureUniform_Split = 0;
    GLuint m_nFinalTextureUniform_Split = 0;
    GLuint m_nSplitFactorUniform = 0;

    GLuint m_nVertexBuffer = 0;
    GLuint m_nVertexArray = 0;

    GLuint m_nFinalRenderTexture = 0;

    bool m_bSplitScreen = false;
    float m_fSplitFactor = 0.5f;

    PointerExpandableArray<IRenderProcess*> m_oProcessArray = PointerExpandableArray<IRenderProcess*>(2);

    void init(int nWidth, int nHeight);
    void initializeQuad();
    void initializeOriginalFBO();
};