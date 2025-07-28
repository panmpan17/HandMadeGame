// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <linmath.h>
#include <functional>

#include "window.h"
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

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";
 
static const char* fragment_shader_text =
"#version 330\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";

Window::Window()
{
    if (!glfwInit())
    {
        DEBUG_PRINT("Failed to initialize GLFW");
        return;
    }

    configureAndCreateWindow();
}

Window::~Window()
{
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
    glfwSetKeyCallback(m_pWindow, &Window::onKeyCallback);

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

    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    m_nProgram = glCreateProgram();
    glAttachShader(m_nProgram, vertex_shader);
    glAttachShader(m_nProgram, fragment_shader);
    glLinkProgram(m_nProgram);

    m_nMvpLocation = glGetUniformLocation(m_nProgram, "MVP");
    const GLint vpos_location = glGetAttribLocation(m_nProgram, "vPos");
    const GLint vcol_location = glGetAttribLocation(m_nProgram, "vCol");

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
    mat4x4 m, p, mvp;
    mat4x4_identity(m);
    mat4x4_rotate_Z(m, m, (float) glfwGetTime());
    mat4x4_ortho(p, -m_fRatio, m_fRatio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);

    glUseProgram(m_nProgram);
    glUniformMatrix4fv(m_nMvpLocation, 1, GL_FALSE, (const GLfloat*) &mvp);
    glBindVertexArray(m_nVertexArray);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Window::onKeyCallback(GLFWwindow* pWindow, int nKey, int nScanNode, int nAction, int nMods)
{
    // DEBUG_PRINT_EX("Key pressed: {}, Scancode: {}, Action: {}, Mods: {}", nKey, nScanNode, nAction, nMods);
}
