#include "music_player.h"

#include "../../engine/core/debug_macro.h"
#include "../../engine/core/audio/audio_engine.h"


MusicPlayer::MusicPlayer()
{
}

MusicPlayer::~MusicPlayer()
{
}

void MusicPlayer::addAudioClip(const std::shared_ptr<AudioClip>& audioClip)
{
    m_audioClips.push_back(audioClip);
}

void MusicPlayer::play()
{
    if (m_audioClips.empty())
    {
        LOGERR("No audio clips available to play.");
        return;
    }

    if (m_bIsPlaying)
    {
        LOGERR("Audio clip is already playing.");
        return;
    }
    m_bIsPlaying = true;

    AudioEngine& audioEngine = AudioEngine::getInstance();
    if (m_nCurrentClipHandle == -1)
    {
        std::shared_ptr<AudioClip> currentClip = m_audioClips[m_nCurrentClipIndex];

        m_nCurrentClipHandle = audioEngine.playOneShotAudio(*currentClip, 1.0f);
        audioEngine.fadeVolume(m_nCurrentClipHandle, m_fVolume, FADE_DURATION);
    }
    else
    {
        audioEngine.setPause(m_nCurrentClipHandle, false);
        audioEngine.setVolume(m_nCurrentClipHandle, 0.0f);
        audioEngine.fadeVolume(m_nCurrentClipHandle, m_fVolume, FADE_DURATION);
    }
}

void MusicPlayer::pause(bool bFadeOut/* = true */)
{
    if (!m_bIsPlaying) { return; }
    m_bIsPlaying = false;

    AudioEngine& audioEngine = AudioEngine::getInstance();
    if (bFadeOut)
    {
        audioEngine.fadeVolume(m_nCurrentClipHandle, 0.0f, FADE_DURATION);
        audioEngine.schedulePause(m_nCurrentClipHandle, FADE_DURATION);
    }
    else
    {
        audioEngine.setPause(m_nCurrentClipHandle, true);
    }
}

void MusicPlayer::stop(bool bFadeOut/* = true */)
{
    if (!m_bIsPlaying) { return; }
    m_bIsPlaying = false;

    AudioEngine& audioEngine = AudioEngine::getInstance();
    if (bFadeOut)
    {
        audioEngine.fadeVolume(m_nCurrentClipHandle, 0.0f, FADE_DURATION, true);
    }
    else
    {
        audioEngine.stop(m_nCurrentClipHandle);
        m_nCurrentClipHandle = -1;
    }
}

void MusicPlayer::update(float fDeltaTime)
{
    if (!m_bIsPlaying) { return; }

    AudioEngine& audioEngine = AudioEngine::getInstance();
    if (!audioEngine.isValidVoiceHandle(m_nCurrentClipHandle))
    {
        // Current clip has finished playing, move to the next clip
        m_nCurrentClipIndex = (m_nCurrentClipIndex + 1) % m_audioClips.size();
        std::shared_ptr<AudioClip> nextClip = m_audioClips[m_nCurrentClipIndex];

        // Play the next clip
        m_nCurrentClipHandle = audioEngine.playOneShotAudio(*nextClip, 1.0f);
        audioEngine.fadeVolume(m_nCurrentClipHandle, m_fVolume, FADE_DURATION);
    }
}
