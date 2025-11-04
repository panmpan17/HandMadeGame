#pragma once

#include "event.h"
#include "keycode.h"


typedef struct GLFWwindow GLFWwindow;


class InputManager
{
public:
    static void windowPositionToGLPosition(float windowX, float windowY, float& glX, float& glY);
    static void GLPositionToWindowPosition(float glX, float glY, float& windowX, float& windowY);

    static void getMousePosition(float& x, float& y) { x = sm_fMouseX; y = sm_fMouseY; }
    static void getMousePositionX(float& x) { x = sm_fMouseX; }
    static void getMousePositionY(float& y) { y = sm_fMouseY; }
    static void getMousePositionInGL(float& glX, float& glY) { windowPositionToGLPosition(sm_fMouseX, sm_fMouseY, glX, glY); }

    static bool getIsMouseEntered() { return sm_bMouseEntered; }

    /* GL callback */
    static void onMouseEnterCallback(GLFWwindow* pWindow, int bEntered);
    static void onMousePosCallback(GLFWwindow* pWindow, double fPosX, double fPosY);
    static void onMouseButtonCallback(GLFWwindow* pWindow, int nButton, int nAction, int nMods);

    static void onKeyCallback(GLFWwindow* pWindow, int nKey, int nScanNode, int nAction, int nMods);

    inline static InputManager* getInstance() { return ins; }

    static void Initialize();
    static void Cleanup() { delete ins; ins = nullptr; }

    void registerKeyPressCallback(KeyCode key, std::function<void(bool)> callback)
    {
        if (key > KeyCode::KEY_UNKNOWN && key < KeyCode::MAX_KEY_CODE)
        {
            m_KeyPressEvent[static_cast<int>(key)].add(callback);
        }
    }
    // void unregisterKeyPressCallback(KeyCode key, std::function<void(bool)> callback)
    // {
    //     if (key > KeyCode::KEY_UNKNOWN && key < KeyCode::MAX_KEY_CODE)
    //     {
    //         m_KeyPressEvent[static_cast<int>(key)].remove(callback);
    //     }
    // }

    void registerMouseMoveCallback(std::function<void(float, float)> callback)
    {
        m_arrMouseMoveEvent.add(callback);
    }
    // void unregisterMouseMoveCallback(std::function<void(float, float)> callback)
    // {
    //     m_arrMouseMoveEvent.remove(callback);
    // }

    inline bool isKeyPressed(KeyCode key) const
    {
        if (key > KeyCode::KEY_UNKNOWN && key < KeyCode::MAX_KEY_CODE)
        {
            return m_bKeyPressed[static_cast<int>(key)];
        }
        return false;
    }

    inline bool isMouseButtonPressed(MouseButton eButton) const
    {
        if (eButton >= MouseButton::BUTTON_LEFT && eButton < MouseButton::MAX_BUTTONS)
        {
            return m_arrMouseDown[static_cast<int>(eButton)];
        }
        return false;
    }
    inline bool isMouseButtonPressed(int nButton) const
    {
        if (nButton >= 0 && nButton < MAX_MOUSE_BUTTONS)
        {
            return m_arrMouseDown[nButton];
        }
        return false;
    }

private:
    static bool sm_bMouseEntered;
    static float sm_fMouseX, sm_fMouseY;

    static InputManager* ins; // Singleton instance, if needed

    InputManager();
    ~InputManager() = default;

    bool m_bKeyPressed[(int)KeyCode::MAX_KEY_CODE]; // Array to track key states, if needed

    Event<bool> m_KeyPressEvent[(int)KeyCode::MAX_KEY_CODE]; // Event for key press callbacks

    constexpr static int MAX_MOUSE_BUTTONS = 8;
    bool m_arrMouseDown[MAX_MOUSE_BUTTONS] = { false, false, false, false, false, false, false, false }; // Track mouse button states, if needed
    Event<bool> m_arrMouseButtonEvent[MAX_MOUSE_BUTTONS]; // Events for mouse button callbacks, if needed

    Event<float, float> m_arrMouseMoveEvent;
};
