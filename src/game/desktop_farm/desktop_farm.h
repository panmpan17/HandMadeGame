#pragma once

#include <memory>

class Window;
class AudioClip;
class MusicPlayer;

class DesktopFarmGame
{
public:
    DesktopFarmGame();

    void preconfigureWindowObject(Window* pWindow);
    void setupWorldScene();

private:
    static inline DesktopFarmGame* ins = nullptr;

    Window* m_pWindow = nullptr;

    MusicPlayer* m_pMusicPlayer = nullptr;
};
