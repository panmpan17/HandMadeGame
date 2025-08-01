#pragma once


class Window;
class Camera;


class Vector2
{
public:
    float x, y;

    Vector2() : x(0), y(0) {}
    Vector2(float value) : x(value), y(value) {}
    Vector2(float x, float y) : x(x), y(y) {}
};

class Vector3
{
public:
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float value) : x(value), y(value), z(value) {}
    Vector3(float x, float y) : x(x), y(y), z(0) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
};


class WorldVectorSytem
{
public:
    WorldVectorSytem(Window* pWindow, Camera* pCamera) : m_pWindow(pWindow), m_pCamera(pCamera) {}


    void updateWindowSize(int width, int height)
    {
        // Call camera?
    }

    Vector2 screenToWorld(float screenX, float screenY);

private:
    Window* m_pWindow = nullptr;
    Camera* m_pCamera = nullptr;
};
