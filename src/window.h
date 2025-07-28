
class GLFWwindow;

class Window {
public:
    Window();
    ~Window();

    bool isValid() const { return m_pWindow != nullptr; }

    void start();

private:
    GLFWwindow* m_pWindow = nullptr;

    void configureAndCreateWindow();
};