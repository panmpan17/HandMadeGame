#pragma once

#include <memory>

class Window;
class AudioClip;
class MusicPlayer;
class TextRenderer;
class Node;
class Sprite9Slice;

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

    Node* m_pProgressBarNode = nullptr;
    Sprite9Slice* m_pProgressBarSprite = nullptr;
    TextRenderer* m_pSongNameText = nullptr;

    void onSongChange();
    void onSongProgressUpdate(float fCurrentTime, float fFullLength);
};
