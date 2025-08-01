#include "input_handle.h"
#include "window.h"

bool InputManager::sm_bMouseEntered = false;
float InputManager::sm_fMouseX = 0.0f;
float InputManager::sm_fMouseY = 0.0f;


void InputManager::windowPositionToGLPosition(float windowX, float windowY, float& glX, float& glY)
{
    int width, height;
    Window::ins->getWindowSize(width, height);

    // Convert window coordinates to OpenGL coordinates
    glX = (windowX / width) * 2.0f - 1.0f; // Normalize to [-1, 1]
    glY = 1.0f - (windowY / height) * 2.0f; // Invert Y
}

void InputManager::GLPositionToWindowPosition(float glX, float glY, float& windowX, float& windowY)
{
    int width, height;
    Window::ins->getWindowSize(width, height);

    // Convert OpenGL coordinates back to window coordinates
    windowX = (glX + 1.0f) / 2.0f * width; // Normalize to [0, width]
    windowY = (1.0f - glY) / 2.0f * height; // Invert Y and normalize to [0, height]
}

void InputManager::onMouseEnterCallback(GLFWwindow* pWindow, int bEntered)
{
    sm_bMouseEntered = (bEntered != 0);
}

void InputManager::onMousePosCallback(GLFWwindow* pWindow, double fPosX, double fPosY)
{
    sm_fMouseX = static_cast<float>(fPosX);
    sm_fMouseY = static_cast<float>(fPosY);
    // TODO: fire event or callback if needed
}

void InputManager::onMouseButtonCallback(GLFWwindow* pWindow, int nButton, int nAction, int nMods)
{
    // LOGLN_EX("Mouse button: {}, Action: {}, Mods: {}", nButton, nAction, nMods);
}

void InputManager::onKeyCallback(GLFWwindow* pWindow, int nKey, int nScanNode, int nAction, int nMods)
{
    // LOGLN_EX("Key pressed: {}, Scancode: {}, Action: {}, Mods: {}", nKey, nScanNode, nAction, nMods);
}