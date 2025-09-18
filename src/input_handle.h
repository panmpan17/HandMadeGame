#pragma once

#include <vector>
#include <functional>


typedef struct GLFWwindow GLFWwindow;


enum class KeyCode : int
{
    KEY_UNKNOWN = -1,
    KEY_SPACE = 32,
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
        if (key >= KeyCode::KEY_SPACE && key <= KeyCode::KEY_Z)
        {
            m_KeyPressEvent[static_cast<int>(key)].add(callback);
        }
    }

private:
    static bool sm_bMouseEntered;
    static float sm_fMouseX, sm_fMouseY;

    static InputManager* ins; // Singleton instance, if needed

    InputManager();
    ~InputManager() = default;

    bool m_bKeyPressed[256]; // Array to track key states, if needed

    Event m_KeyPressEvent[256]; // Event for key press callbacks
};
