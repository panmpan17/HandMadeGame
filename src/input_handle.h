#pragma once

#include <vector>
#include <functional>


typedef struct GLFWwindow GLFWwindow;


enum class KeyCode : int
{
    KEY_UNKNOWN = -1,
    KEY_SPACE = 32,
    KEY_MINUS = 45,
    KEY_0 = 48,
    KEY_1 = 49,
    KEY_2 = 50,
    KEY_3 = 51,
    KEY_4 = 52,
    KEY_5 = 53,
    KEY_6 = 54,
    KEY_7 = 55,
    KEY_8 = 56,
    KEY_9 = 57,
    KEY_EQUAL = 61,
    KEY_A = 65,
    KEY_B = 66,
    KEY_C = 67,
    KEY_D = 68,
    KEY_E = 69,
    KEY_F = 70,
    KEY_G = 71,
    KEY_H = 72,
    KEY_I = 73,
    KEY_J = 74,
    KEY_K = 75,
    KEY_L = 76,
    KEY_M = 77,
    KEY_N = 78,
    KEY_O = 79,
    KEY_P = 80,
    KEY_Q = 81,
    KEY_R = 82,
    KEY_S = 83,
    KEY_T = 84,
    KEY_U = 85,
    KEY_V = 86,
    KEY_W = 87,
    KEY_X = 88,
    KEY_Y = 89,
    KEY_Z = 90,
    KEY_BLACKSLASH = 92,
    KEY_TILDE = 96, // ` (~)
    KEY_ESCAPE = 256,
    KEY_RETURN = 257,
    KEY_TAB = 258,
    KEY_ARROW_RIGHT = 262,
    KEY_ARROW_LEFT = 263,
    KEY_ARROW_DOWN = 264,
    KEY_ARROW_UP = 265,
    KEY_CAP_LOCK = 280,
    KEY_LEFT_SHIFT = 340,
    KEY_LEFT_CONTROL = 341,
    KEY_LEFT_ALT = 342,
    KEY_LEFT_META = 343,
    KEY_RIGHT_SHIFT = 344,
    KEY_RIGHT_CONTROL = 345,
    KEY_RIGHT_ALT = 346,
    KEY_RIGHT_META = 347,
    MAX_KEY_CODE,
};


// typedef void (*KeyPressCallback)(bool bPressed);

struct Event
{
    std::vector<std::function<void(bool)>> listeners;

    void add(std::function<void(bool)> f) { listeners.push_back(f); }
    void invoke(bool bPressed) {
        for (auto& listener : listeners)
        {
            listener(bPressed);
        }
    }
};


enum class MouseButton : int
{
    BUTTON_LEFT = 0,
    BUTTON_RIGHT = 1,
    BUTTON_MIDDLE = 2,
    BUTTON_3 = 3,
    BUTTON_4 = 4,
    BUTTON_5 = 5,
    BUTTON_6 = 6,
    BUTTON_7 = 7,
    MAX_BUTTONS
};


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

    Event m_KeyPressEvent[(int)KeyCode::MAX_KEY_CODE]; // Event for key press callbacks

    constexpr static int MAX_MOUSE_BUTTONS = 8;
    bool m_arrMouseDown[MAX_MOUSE_BUTTONS] = { false, false, false, false, false, false, false, false }; // Track mouse button states, if needed
    Event m_arrMouseButtonEvent[MAX_MOUSE_BUTTONS]; // Events for mouse button callbacks, if needed
};
