#pragma once

class Window;

class DesktopFarmGame
{
public:
    DesktopFarmGame();

    void preconfigureWindowObject(Window* pWindow);
    void setupWorldScene();

private:
    static inline DesktopFarmGame* ins = nullptr;

    Window* m_pWindow = nullptr;
};
