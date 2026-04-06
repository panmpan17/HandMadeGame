#pragma once


#include "audio_clip.h"


namespace SoLoud { class Soloud; typedef unsigned int handle; typedef double time; };


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

    SoLoud::handle playOneShotAudio(AudioClip& audioClip, float fVolume = -1.0f, float fPan = 0.0f);

private:
    SoLoud::Soloud* m_pSoloudEngine = nullptr; // Engine core
    bool m_bAudioInitialized = false;
};