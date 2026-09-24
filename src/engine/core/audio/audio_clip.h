#pragma once

#include <string>

namespace SoLoud { class Wav; };

class AudioClip
{
public:
    AudioClip(const std::string& strFilePath, bool bLogAudioInfo = false);
    ~AudioClip();

    SoLoud::Wav& getAudioSource() { return *m_pAudioClip; }
    const SoLoud::Wav& getAudioSource() const { return *m_pAudioClip; }

    bool isLoaded() const { return m_bLoaded; }

    float getLength() const;

    const std::string& getFilePath() const { return m_strFilePath; }
    const std::string getFileName() const { return m_strFilePath.substr(m_strFilePath.find_last_of("/\\") + 1); }

private:
    SoLoud::Wav* m_pAudioClip = nullptr;
    bool m_bLoaded = false;

    std::string m_strFilePath;
};