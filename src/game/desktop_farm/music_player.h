#pragma once

#include <memory>
#include <vector>

#include "../../engine/components/component.h"


class AudioClip;


class MusicPlayer : public NodeComponent
{
public:
    MusicPlayer();
    ~MusicPlayer();

    void addAudioClip(const std::shared_ptr<AudioClip>& audioClip);

    void play();
    void pause(bool bFadeOut = true);
    void stop(bool bFadeOut = true);

    virtual inline bool isIDrawable() const override { return false; }
    virtual inline bool isUpdatable() const override { return true; }

    virtual void update(float fDeltaTime) override;

private:
    static constexpr float FADE_DURATION = 1.0f; // Duration for fade in/out in seconds

    std::vector<std::shared_ptr<AudioClip>> m_audioClips;
    
    float m_fVolume = 1.0f;
    int m_nCurrentClipIndex = 0;
    int m_nCurrentClipHandle = -1;
    bool m_bIsPlaying = false;
};
