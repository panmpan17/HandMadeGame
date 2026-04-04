#include "file_watch_dog.h"

#include <filesystem>
#include <chrono>
#include <unordered_map>


void FileWatchDog::startWatching()
{
    if (m_bStarted.load(std::memory_order_acquire))
    {
        return;
    }

    m_bStarted.store(true, std::memory_order_release);

    m_watchThread = std::thread(&FileWatchDog::watchLoop, this);
}

void FileWatchDog::watchLoop()
{
    std::unordered_map<std::string, std::filesystem::file_time_type> mapFileLastState;

    for (auto& p : std::filesystem::recursive_directory_iterator(m_strWatchedFolders))
    {
        if (std::filesystem::is_regular_file(p.path()))
        {
            mapFileLastState[p.path().string()] = std::filesystem::last_write_time(p.path());
        }
    }

    while (m_bStarted.load(std::memory_order_acquire))
    {
        std::this_thread::sleep_for(std::chrono::duration<float>(m_fCheckInterval));

        for (auto iter = mapFileLastState.begin(); iter != mapFileLastState.end(); )
        {
            if (!std::filesystem::exists(iter->first))
            {
                // File deleted
                if (m_funcFileChangeCallback)
                {
                    m_funcFileChangeCallback(iter->first, eFileChangeType::FILE_DELETED);
                }
                iter = mapFileLastState.erase(iter);
            }
            else
            {
                auto currentFileTime = std::filesystem::last_write_time(iter->first);
                if (currentFileTime != iter->second)
                {
                    // File modified
                    if (m_funcFileChangeCallback)
                    {
                        m_funcFileChangeCallback(iter->first, eFileChangeType::FILE_MODIFIED);
                    }
                    iter->second = currentFileTime;
                }
                ++iter;
            }
        }

        for (auto& p : std::filesystem::recursive_directory_iterator(m_strWatchedFolders))
        {
            if (std::filesystem::is_regular_file(p.path()))
            {
                std::string strPath = p.path().string();
                if (mapFileLastState.find(strPath) == mapFileLastState.end())
                {
                    // New file created
                    if (m_funcFileChangeCallback)
                    {
                        m_funcFileChangeCallback(strPath, eFileChangeType::FILE_CREATED);
                    }
                    mapFileLastState[strPath] = std::filesystem::last_write_time(p.path());
                }
            }
        }
    }
}

void FileWatchDog::stopWatching()
{
    m_bStarted.store(false, std::memory_order_release);
    m_watchThread.join();
}
