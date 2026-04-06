#include "audio_engine.h"

#include "audio_clip.h"
#include "../debug_macro.h"

// #include <stdio.h>
#include <iostream>
#include <soloud.h>
#include <soloud_wav.h>


AudioEngine::AudioEngine()
{
    m_pSoloudEngine = new SoLoud::Soloud();
    const auto audioInitResult = m_pSoloudEngine->init();
    if (audioInitResult != SoLoud::SO_NO_ERROR)
    {
        std::cerr << "Failed to initialize SoLoud: "
                  << m_pSoloudEngine->getErrorString(audioInitResult) << std::endl;
    }
    else
    {
        m_bAudioInitialized = true;
    }
}

AudioEngine::~AudioEngine()
{
    if (m_bAudioInitialized)
    {
        m_pSoloudEngine->deinit();
    }

    delete m_pSoloudEngine;
    m_pSoloudEngine = nullptr;
}

SoLoud::handle AudioEngine::playOneShotAudio(AudioClip& audioClip, float fVolume/* = -1.0f*/, float fPan/* = 0.0f*/)
{
    if (!m_bAudioInitialized)
    {
        std::cerr << "Audio engine is not initialized. Cannot play audio." << std::endl;
        return 0;
    }

    if (!audioClip.isLoaded())
    {
        std::cerr << "Audio clip is not loaded. Cannot play audio." << std::endl;
        return 0;
    }

    return m_pSoloudEngine->play(audioClip.getAudioSource(), fVolume, fPan);
}
