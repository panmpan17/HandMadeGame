#pragma once


#include "audio_clip.h"


namespace SoLoud { class Soloud; };


class AudioEngine
{
public:
    static AudioEngine& getInstance()
    {
        static AudioEngine instance;
        return instance;
    }

    AudioEngine();
    ~AudioEngine();

    AudioEngine(const AudioEngine&) = delete;
    AudioEngine& operator=(const AudioEngine&) = delete;

    void playOneShotAudio(AudioClip& audioClip);

private:
    SoLoud::Soloud* m_pSoloudEngine = nullptr; // Engine core
    bool m_bAudioInitialized = false;
};