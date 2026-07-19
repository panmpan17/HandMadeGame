#include "audio_engine.h"

#include "audio_clip.h"
#include "../debug_macro.h"

// #include <stdio.h>
#include <iostream>
#include <soloud.h>
#include <soloud_wav.h>


#define CHECK_AUDIO_ENGINE_INITIALIZED() if (!m_bAudioInitialized) { LOGERR("Audio engine not initialized."); return; }


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

SoLoud::handle AudioEngine::playOneShotAudio(AudioClip& audioClip, float fVolume/* = 1.0f*/, float fPan/* = 0.0f*/)
{
    if (!m_bAudioInitialized)
    {
        return 0;
    }

    if (!audioClip.isLoaded())
    {
        LOGERR("Audio clip is not loaded. Cannot play audio.");
        return 0;
    }

    return m_pSoloudEngine->play(audioClip.getAudioSource(), fVolume, fPan);
}

void AudioEngine::setLoopPoint(SoLoud::handle handle, float loopPoint)
{
    CHECK_AUDIO_ENGINE_INITIALIZED();
    m_pSoloudEngine->setLoopPoint(handle, loopPoint);
    m_pSoloudEngine->setLooping(handle, true);
}

void AudioEngine::setPause(SoLoud::handle handle, bool bPause)
{
    CHECK_AUDIO_ENGINE_INITIALIZED();
    m_pSoloudEngine->setPause(handle, bPause);
}

void AudioEngine::setVolume(SoLoud::handle handle, float fVolume)
{
    CHECK_AUDIO_ENGINE_INITIALIZED();
    m_pSoloudEngine->setVolume(handle, fVolume);
}

void AudioEngine::fadeVolume(SoLoud::handle handle, float fTargetVolume, float fFadeTime, bool bStopOnFadeOut /*= false*/)
{
    CHECK_AUDIO_ENGINE_INITIALIZED();
    m_pSoloudEngine->fadeVolume(handle, fTargetVolume, fFadeTime);
    if (bStopOnFadeOut)
    {
        m_pSoloudEngine->scheduleStop(handle, fFadeTime);
    }
}

void AudioEngine::stop(SoLoud::handle handle)
{
    CHECK_AUDIO_ENGINE_INITIALIZED();
    m_pSoloudEngine->stop(handle);
}

void AudioEngine::scheduleStop(SoLoud::handle handle, float fDelay)
{
    CHECK_AUDIO_ENGINE_INITIALIZED();
    m_pSoloudEngine->scheduleStop(handle, fDelay);
}

void AudioEngine::schedulePause(SoLoud::handle handle, float fDelay)
{
    CHECK_AUDIO_ENGINE_INITIALIZED();
    m_pSoloudEngine->schedulePause(handle, fDelay);
}

bool AudioEngine::isValidVoiceHandle(SoLoud::handle handle)
{
    if (!m_bAudioInitialized)
    {
        return false;
    }

    // Check if the voice handle is valid
    if (handle == 0)
    {
        return false;
    }

    return m_pSoloudEngine->isValidVoiceHandle(handle);
}
