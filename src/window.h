
class GLFWwindow;
class Camera;
class Shader;

typedef unsigned int GLuint;

class Window {
public:
    static Window* ins;

    Window();
    ~Window();

    bool isValid() const { return m_pWindow != nullptr; }

    void start();

    static void onKeyCallback(GLFWwindow* pWindow, int nKey, int nScanNode, int nAction, int nMods);
    static void onCursorEnterCallback(GLFWwindow* pWindow, int bEntered);
    static void onCursorPosCallback(GLFWwindow* pWindow, double fPosX, double fPosY);
    static void onMouseButtonCallback(GLFWwindow* pWindow, int nButton, int nAction, int nMods);

private:
    GLFWwindow* m_pWindow = nullptr;

    GLuint m_nMvpLocation, m_nVertexArray;
    Shader* m_pBaseShader = nullptr;

    int m_nWidth = 800, m_nHeight = 600;
    float m_fRatio = 1.0f;

    double m_fCurrentDrawTime = 0.0;
    double m_fLastDrawTime = 0.0;
    float m_fDeltaTime = 0.0;

    Camera* m_pCamera = nullptr;

    float m_fTempMouseX = 0.0f, m_fTempMouseY = 0.0f;

    void configureAndCreateWindow();
    void setupGLVertex();
    void mainLoop();
    void drawFrame();
};