// #define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
// #define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <linmath.h>
#include <functional>

#include "window.h"
#include "camera.h"
#include "draw/shader.h"
#include "draw/quad.h"
#include "draw/triangle.h"
#include "debug_macro.h"
#include "input_handle.h"
#include "image.h"
#include "node.h"
#include "components/rotate.h"
#include "components/follow_mouse.h"
#include "components/movement.h"
#include "components/simple_particle_system.h"


Window* Window::ins = nullptr;

Window::Window()
{
    ins = this;

    if (!glfwInit())
    {
        LOGLN("Failed to initialize GLFW");
        return;
    }
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
    if (m_pBaseShader)
    {
        delete m_pBaseShader;
        m_pBaseShader = nullptr;
    }
    if (m_pImageShader)
    {
        delete m_pImageShader;
        m_pImageShader = nullptr;
    }
    // for (int i = 0; i < m_nNodeCount; ++i)
    // {
    //     if (m_pNodes[i])
    //     {
    //         delete m_pNodes[i];
    //     }
    // }
    if (m_pImage)
    {
        delete m_pImage;
        m_pImage = nullptr;
    }

    InputManager::Cleanup();

    glfwTerminate();
}

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char* message, const void* userParam)
{
    // ignore non-significant error/warnings
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::println("Debug message ({}) at {}:{}: {}", id, __FILE__, __LINE__, message);
}

void Window::configureAndCreateWindow()
{
    // Configure GL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RESIZABLE, m_bResizable ? GLFW_TRUE : GLFW_FALSE);

#if IS_DEBUG_VERSION
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    m_pWindow = glfwCreateWindow(m_nWidth, m_nHeight, "My GLFW Window", NULL, NULL);
    if (!m_pWindow)
    {
        LOGLN("Failed to create GLFW window");
        glfwTerminate();
        return;
    }

    // GLint flags;
    // glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    // if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    // {
    //     glEnable(GL_DEBUG_OUTPUT);
    //     glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    //     glDebugMessageCallback(glDebugOutput, nullptr);
    //     glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    // }
}

void Window::start()
{
    InputManager::Initialize();

    m_pCamera = new Camera();

    glfwSetKeyCallback(m_pWindow, &InputManager::onKeyCallback);
    glfwSetCursorEnterCallback(m_pWindow, &InputManager::onMouseEnterCallback);
    glfwSetCursorPosCallback(m_pWindow, &InputManager::onMousePosCallback);
    glfwSetMouseButtonCallback(m_pWindow, &InputManager::onMouseButtonCallback);

    // glfwSetWindowOpacity(m_pWindow, 0.5f); // Fun

    glfwMakeContextCurrent(m_pWindow);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1); // Enable vsync
    
    m_pImage = new Image("assets/images/test.png");
    if (m_pImage->isCPULoaded())
    {
        m_pImage->loadTextureToGL();
        m_pImage->freeCPUData();
    }

    setupShaders();
    setupGLVertex();

    mainLoop();
}

void Window::setupGLVertex()
{
    // Triangle
    auto pNode1 = new Node(-0.5f, 0.f, 0.f, 0.f);

    auto pTriangle = new Triangle();
    pTriangle->setShader(m_pBaseShader);
    pTriangle->registerBuffer();
    pNode1->addComponent(pTriangle);

    pNode1->addComponent(new Rotate(-1.0f));

    pNode1->addComponent(new Movement(1.0f)); // Add movement component with speed 1.0f

    addNode(pNode1);

    // Quads 1
    auto pNode2 = new Node(0.5f, 0.5f, 0.f, 0.f);

    vec4 red = {1.f, 0.f, 0.f, 1.f}; // Red color for the quad
    auto pQuad = new Quad(0, 0, 0.5f, 0.5f, red);
    pQuad->setShader(m_pImageShader);
    static_cast<Quad*>(pQuad)->setImage(m_pImage);
    pQuad->registerBuffer();
    pNode2->addComponent(pQuad);

    pNode2->addComponent(new Rotate(1.0f));

    addNode(pNode2);

    // Quads 2
    auto pNode3 = new Node(0.5f, -0.5f, 0.f, 0.f);

    vec4 color = {0.5f, 0.5f, 1.f, 1.f}; // Blue color for the second quad
    auto pQuad2 = new Quad(0, 0, 0.3f, 0.3f, color);
    pQuad2->setShader(m_pImageShader);
    static_cast<Quad*>(pQuad2)->setImage(m_pImage);
    pQuad2->registerBuffer();
    pNode3->addComponent(pQuad2);

    addNode(pNode3);

    // Particle System
    auto pNode4 = new Node(0, 0, 0, 0);
    auto particle = new SimpleParticleSystem(20);
    particle->setShader(new ParticleShader());
    pNode4->addComponent(particle);
    addNode(pNode4);
}

void Window::addNode(Node* pNode)
{
    if (pNode == nullptr) return;

    // Find an empty slot in the node array
    for (int i = 0; i < m_nNodeCount; ++i)
    {
        if (m_pNodes[i] == nullptr)
        {
            m_pNodes[i] = pNode;
            return;
        }
    }

    // TODO: Handle case where no empty slot is found, e.g., resize the array or log an error
    LOGERRLN("No empty slot found to add the node");
}

void Window::setupShaders()
{
    m_pBaseShader = new TestShader();
    m_pImageShader = new ImageShader();
}

void Window::mainLoop()
{
    m_fLastDrawTime = glfwGetTime();

    while (!glfwWindowShouldClose(m_pWindow))
    {
        // Because mac's retina display has a different pixel ratio (and moving to different monitors)
        // need to adjust the viewport to match the actual framebuffer size.
        glfwGetFramebufferSize(m_pWindow, &m_nActualWidth, &m_nActualHeight);
        m_fRatio = m_nActualWidth / (float) m_nActualHeight;
        m_pCamera->setRatio(m_fRatio);

        glViewport(0, 0, m_nActualWidth, m_nActualHeight);
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

    // Michael TODO: translate mouse position to camera position
    // m_pCamera->setPosition(m_fTempMouseX, m_fTempMouseY);
    // m_pCamera->move(3 * m_fDeltaTime, 0, 0); // Move the camera back a bit
    // m_pCamera->rotate(0, 0, 1.f * m_fDeltaTime); // Rotate the camera slightly each frame

    for (int i = 0; i < m_nNodeCount; ++i)
    {
        if (m_pNodes[i])
        {
            m_pNodes[i]->update(m_fDeltaTime);
            m_pNodes[i]->draw();
        }
    }
}
