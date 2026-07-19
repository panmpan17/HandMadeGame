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

    /**
     * @brief Plays a one-shot audio clip using the audio engine.
     * @param audioClip The AudioClip to be played.
     * @param fVolume The volume level, if smaller than 0 (exclusive) it will default to clip's volume 1
     * @param fPan The pan level for the audio clip (default is 0.0f, centered).
     * @return A handle to the playing audio clip, or 0 if the audio engine is not initialized or the audio clip is not loaded.
     */
    SoLoud::handle playOneShotAudio(AudioClip& audioClip, float fVolume = 1.0f, float fPan = 0.0f);

    void setLoopPoint(SoLoud::handle handle, float loopPoint);
    void setPause(SoLoud::handle handle, bool bPause);
    void setVolume(SoLoud::handle handle, float fVolume);
    void fadeVolume(SoLoud::handle handle, float fTargetVolume, float fFadeTime, bool bStopOnFadeOut = false);
    void stop(SoLoud::handle handle);
    void scheduleStop(SoLoud::handle handle, float fDelay);
    void schedulePause(SoLoud::handle handle, float fDelay);
    bool isValidVoiceHandle(SoLoud::handle handle);

private:
    SoLoud::Soloud* m_pSoloudEngine = nullptr; // Engine core
    bool m_bAudioInitialized = false;
};