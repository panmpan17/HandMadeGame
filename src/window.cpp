// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <linmath.h>
#include <functional>

#include "window.h"
#include "camera.h"
#include "shader.h"
#include "debug_macro.h"

typedef struct Vertex
{
    vec2 pos;
    vec3 col;
} Vertex;
 
static const Vertex vertices[3] =
{
    { { -0.6f, -0.4f }, { 1.f, 0.f, 0.f } },
    { {  0.6f, -0.4f }, { 0.f, 1.f, 0.f } },
    { {   0.f,  0.6f }, { 0.f, 0.f, 1.f } }
};

Window* Window::ins = nullptr;

Window::Window()
{
    ins = this;

    if (!glfwInit())
    {
        DEBUG_PRINT("Failed to initialize GLFW");
        return;
    }

    configureAndCreateWindow();
}

Window::~Window()
{
    if (m_pCamera)
    {
        delete m_pCamera;
        m_pCamera = nullptr;
    }
    if (m_pWindow)
    {
        glfwDestroyWindow(m_pWindow);
    }
    glfwTerminate();
}

void Window::configureAndCreateWindow()
{
    // Configure GL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_pWindow = glfwCreateWindow(m_nWidth, m_nHeight, "My GLFW Window", NULL, NULL);
    if (!m_pWindow)
    {
        DEBUG_PRINT("Failed to create GLFW window");
        glfwTerminate();
        return;
    }
}

void Window::start()
{
    m_pCamera = new Camera();
    m_pCamera->setRatio(m_nWidth / (float) m_nHeight);

    glfwSetKeyCallback(m_pWindow, &Window::onKeyCallback);
    glfwSetCursorEnterCallback(m_pWindow, &Window::onCursorEnterCallback);
    glfwSetCursorPosCallback(m_pWindow, &Window::onCursorPosCallback);
    glfwSetMouseButtonCallback(m_pWindow, &Window::onMouseButtonCallback);

    // glfwSetWindowOpacity(m_pWindow, 0.5f); // Fun

    glfwMakeContextCurrent(m_pWindow);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1); // Enable vsync
    
    setupGLVertex();
    mainLoop();
}

void Window::setupGLVertex()
{
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    m_pBaseShader = new Shader();

    GLint nProgram = m_pBaseShader->getProgram();
    m_nMvpLocation = glGetUniformLocation(nProgram, "MVP");
    const GLint vpos_location = glGetAttribLocation(nProgram, "vPos");
    const GLint vcol_location = glGetAttribLocation(nProgram, "vCol");

    glGenVertexArrays(1, &m_nVertexArray);
    glBindVertexArray(m_nVertexArray);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, pos));
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(Vertex), (void*) offsetof(Vertex, col));
}

void Window::mainLoop()
{
    m_fLastDrawTime = glfwGetTime();

    // glfwGetFramebufferSize(m_pWindow, &width, &height);
    m_fRatio = m_nWidth / (float) m_nHeight;
    while (!glfwWindowShouldClose(m_pWindow))
    {
        glViewport(0, 0, m_nWidth, m_nHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        drawFrame();

        glfwSwapBuffers(m_pWindow);
        glfwPollEvents();
    }

    glfwTerminate();
}

void Window::drawFrame()
{
    m_fCurrentDrawTime = glfwGetTime();
    m_fDeltaTime = m_fCurrentDrawTime - m_fLastDrawTime;
    m_fLastDrawTime = m_fCurrentDrawTime;

    mat4x4 mvp;

    // Michael TODO: translate mouse position to camera position
    // m_pCamera->setPosition(m_fTempMouseX, m_fTempMouseY);
    // m_pCamera->move(3 * m_fDeltaTime, 0, 0); // Move the camera back a bit
    // m_pCamera->rotate(0, 0, 1.f * m_fDeltaTime); // Rotate the camera slightly each frame
    m_pCamera->getViewMatrix(mvp);

    glUseProgram(m_pBaseShader->getProgram());
    glUniformMatrix4fv(m_nMvpLocation, 1, GL_FALSE, (const GLfloat*) mvp);
    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Window::onKeyCallback(GLFWwindow* pWindow, int nKey, int nScanNode, int nAction, int nMods)
{
    // DEBUG_PRINT_EX("Key pressed: {}, Scancode: {}, Action: {}, Mods: {}", nKey, nScanNode, nAction, nMods);
}

void Window::onCursorEnterCallback(GLFWwindow* pWindow, int bEntered)
{
    // if (bEntered)
    // {
    //     DEBUG_PRINT("Cursor entered the window");
    // }
    // else
    // {
    //     DEBUG_PRINT("Cursor left the window");
    // }
}

void Window::onCursorPosCallback(GLFWwindow* pWindow, double fPosX, double fPosY)
{
    ins->m_fTempMouseX = static_cast<float>(fPosX);
    ins->m_fTempMouseY = static_cast<float>(fPosY);
    // DEBUG_PRINT_EX("Cursor position: ({}, {})\r", fPosX, fPosY);
}

void Window::onMouseButtonCallback(GLFWwindow* pWindow, int nButton, int nAction, int nMods)
{
    // DEBUG_PRINT_EX("Mouse button: {}, Action: {}, Mods: {}", nButton, nAction, nMods);
}
