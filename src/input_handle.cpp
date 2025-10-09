#include "input_handle.h"
#include "window.h"
#include "debug_macro.h"


#define GLFW_RELEASE 0
#define GLFW_PRESS 1
#define GLFW_REPEAT 2


bool InputManager::sm_bMouseEntered = false;
float InputManager::sm_fMouseX = 0.0f;
float InputManager::sm_fMouseY = 0.0f;
InputManager* InputManager::ins = nullptr;


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
    float fMouseX = static_cast<float>(fPosX);
    float fMouseY = static_cast<float>(fPosY);
    // TODO: fire event or callback if needed

    if (ins)
    {
        ins->m_arrMouseMoveEvent.invoke(sm_fMouseX - fMouseX, sm_fMouseY - fMouseY);
    }

    sm_fMouseX = fMouseX;
    sm_fMouseY = fMouseY;
}

void InputManager::onMouseButtonCallback(GLFWwindow* pWindow, int nButton, int nAction, int nMods)
{
    LOGLN_EX("Mouse button: {}, Action: {}, Mods: {}", nButton, nAction, nMods);
    if (!ins)
    {
        LOGERRLN("InputManager instance is not initialized.");
        return;
    }

    if (nButton >= 0 && nButton < MAX_MOUSE_BUTTONS)
    {
        switch (nAction)
        {
            case GLFW_PRESS:
                ins->m_arrMouseDown[nButton] = true;
                ins->m_arrMouseButtonEvent[nButton].invoke(true);
                break;
            case GLFW_RELEASE:
                ins->m_arrMouseDown[nButton] = false;
                ins->m_arrMouseButtonEvent[nButton].invoke(false);
                break;
            default:
                break;
        }
    }
}

void InputManager::onKeyCallback(GLFWwindow* pWindow, int nKey, int nScanNode, int nAction, int nMods)
{
    if (!ins)
    {
        LOGERRLN("InputManager instance is not initialized.");
        return;
    }

    switch (nAction)
    {
        case GLFW_PRESS:
            if (nKey >= 0 && nKey < (int)KeyCode::MAX_KEY_CODE)
            {
                ins->m_bKeyPressed[nKey] = true;
                ins->m_KeyPressEvent[nKey].invoke(true);
            }
            break;
        case GLFW_RELEASE:
            if (nKey >= 0 && nKey < (int)KeyCode::MAX_KEY_CODE)
            {
                ins->m_bKeyPressed[nKey] = false;
                ins->m_KeyPressEvent[nKey].invoke(false);
            }
            break;
        case GLFW_REPEAT:
            break;
        default:
            break;
    }
}

void InputManager::Initialize()
{
    if (ins == nullptr)
    {
        ins = new InputManager();
    }
}

InputManager::InputManager()
{
    // Initialize key states to false
    for (int i = 0; i < 256; ++i)
    {
        m_bKeyPressed[i] = false;
    }
}
