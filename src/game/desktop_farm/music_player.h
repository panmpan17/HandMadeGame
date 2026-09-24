#pragma once

#include <memory>
#include <vector>

#include "../../engine/components/component.h"
#include "../../engine/core/input/event.h"


class AudioClip;


class MusicPlayer : public NodeComponent
{
public:
    MusicPlayer();
    ~MusicPlayer();

    void addAudioClip(const std::shared_ptr<AudioClip>& audioClip);
    std::shared_ptr<AudioClip> getCurrentAudioClip() const { return m_nCurrentClipIndex >= 0 && m_nCurrentClipIndex < static_cast<int>(m_audioClips.size()) ? m_audioClips[m_nCurrentClipIndex] : nullptr; }

    void play();
    void pause(bool bFadeOut = true);
    void stop(bool bFadeOut = true);

    void nextTrack();

    virtual inline bool isIDrawable() const override { return false; }
    virtual inline bool isUpdatable() const override { return true; }

    virtual void update(float fDeltaTime) override;

    bool isPlaying() const { return m_bIsPlaying; }
    
    void addOnProgressUpdateCallback(const std::function<void(float, float)>& callback) { m_onProgressUpdate.add(callback); }
    void addOnSongChangeCallback(const std::function<void()>& callback) { m_onSongChange.add(callback); }

private:
    static constexpr float FADE_DURATION = 1.0f; // Duration for fade in/out in seconds

    std::vector<std::shared_ptr<AudioClip>> m_audioClips;
    
    float m_fVolume = 1.0f;
    int m_nCurrentClipIndex = 0;
    int m_nCurrentClipHandle = -1;
    bool m_bIsPlaying = false;

    CustomEvent<float, float> m_onProgressUpdate;
    VoidEvent m_onSongChange;
};
