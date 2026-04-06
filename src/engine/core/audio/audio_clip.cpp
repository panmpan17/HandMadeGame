#include "audio_clip.h"

#include "../debug_macro.h"
#include "../../../utils/file_utils.h"

#include <filesystem>
#include <iostream>
#include <soloud.h>
#include <soloud_wav.h>


AudioClip::AudioClip(const std::string& strFilePath, bool bLogAudioInfo/* = false*/)
{
    m_pAudioClip = new SoLoud::Wav();

    std::string strResolvedPath = strFilePath;
    if (!strResolvedPath.empty() && strResolvedPath.front() != '/')
    {
        strResolvedPath = (std::filesystem::path(FileUtils::getResourcesPath()) / strResolvedPath).string();
    }

    const auto loadResult = m_pAudioClip->load(strResolvedPath.c_str());
    m_bLoaded = loadResult == SoLoud::SO_NO_ERROR;

    if (!m_bLoaded)
    {
        std::cerr << "Failed to load audio clip: " << loadResult
                  << " (" << strResolvedPath << ')' << std::endl;
        return;
    }

    if (bLogAudioInfo)
    {
         LOGLN("Length: {}, Sample count: {}, Base sample rate: {}, Channels: {}, Loop point: {}",
              m_pAudioClip->getLength(),
              m_pAudioClip->mSampleCount,
              m_pAudioClip->mBaseSamplerate,
              m_pAudioClip->mChannels,
              m_pAudioClip->getLoopPoint());
    }
}

AudioClip::~AudioClip()
{
    delete m_pAudioClip;
    m_pAudioClip = nullptr;
}
