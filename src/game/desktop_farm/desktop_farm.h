#pragma once

#include <memory>

class Window;
class AudioClip;

class DesktopFarmGame
{
public:
    DesktopFarmGame();

    void preconfigureWindowObject(Window* pWindow);
    void setupWorldScene();

private:
    static inline DesktopFarmGame* ins = nullptr;

    Window* m_pWindow = nullptr;

    std::unique_ptr<AudioClip> m_pStartupAudioClip;
};
