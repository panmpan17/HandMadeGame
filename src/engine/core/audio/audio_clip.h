#pragma once

#include <string>

namespace SoLoud { class Wav; };

class AudioClip
{
public:
    AudioClip(const std::string& strFilePath);
    ~AudioClip();

    SoLoud::Wav& getAudioSource() { return *m_pAudioClip; }
    const SoLoud::Wav& getAudioSource() const { return *m_pAudioClip; }

    bool isLoaded() const { return m_bLoaded; }

private:
    SoLoud::Wav* m_pAudioClip = nullptr;
    bool m_bLoaded = false;
};