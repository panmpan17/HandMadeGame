
class GLFWwindow;
class Camera;

typedef unsigned int GLuint;

class Window {
public:
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

    GLuint m_nProgram, m_nMvpLocation, m_nVertexArray;

    int m_nWidth = 800, m_nHeight = 600;
    float m_fRatio = 1.0f;

    Camera* m_pCamera = nullptr;

    void configureAndCreateWindow();
    void setupGLVertex();
    void mainLoop();
    void drawFrame();
};