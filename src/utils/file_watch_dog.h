#pragma once


#include <string>
#include <thread>
#include <atomic>
#include <functional>


enum class eFileChangeType : int
{
    FILE_CREATED,
    FILE_DELETED,
    FILE_MODIFIED
};

class FileWatchDog
{
public:
    FileWatchDog(const std::string& strWatchedFolders) : m_strWatchedFolders(strWatchedFolders) {}
    ~FileWatchDog() {}

    inline void setCheckInterval(float fInterval) { if (!m_bStarted.load(std::memory_order_acquire)) m_fCheckInterval = fInterval; }

    void startWatching();
    void stopWatching();

    inline void setFileChangeCallback(const std::function<void(const std::string&, eFileChangeType)>& funcCallback)
    {
        m_funcFileChangeCallback = funcCallback;
    }

private:
    std::string m_strWatchedFolders;
    std::atomic<bool> m_bStarted = { false };

    float m_fCheckInterval = .3f; // in seconds

    std::thread m_watchThread;

    std::function<void(const std::string&, eFileChangeType)> m_funcFileChangeCallback = nullptr;

    void watchLoop();
};
