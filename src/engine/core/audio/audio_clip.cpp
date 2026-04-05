#include "audio_clip.h"

#include "../../../utils/file_utils.h"

#include <filesystem>
#include <iostream>
#include <soloud.h>
#include <soloud_wav.h>


AudioClip::AudioClip(const std::string& strFilePath)
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
    }
}

AudioClip::~AudioClip()
{
    delete m_pAudioClip;
    m_pAudioClip = nullptr;
}
