#pragma once

class GLFWwindow;

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

private:
    static bool sm_bMouseEntered;
    static float sm_fMouseX, sm_fMouseY;
};


class WorldVector
{
public:
    float x, y, z;

    WorldVector() : x(0), y(0), z(0) {}
    WorldVector(float value) : x(value), y(value), z(value) {}
    WorldVector(float x, float y) : x(x), y(y), z(0) {}
    WorldVector(float x, float y, float z) : x(x), y(y), z(z) {}


};