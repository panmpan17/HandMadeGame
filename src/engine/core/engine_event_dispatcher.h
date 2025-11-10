#pragma once


#include <vector>
#include <functional>
#include <mutex>


class EngineEventDispatcher
{
public:
    static EngineEventDispatcher& getInstance()
    {
        static EngineEventDispatcher instance;
        return instance;
    }

    inline void updateEvents(float deltaTime)
    {
        std::vector<std::function<void()>> eventsToProcess;
        {
            std::lock_guard<std::mutex> lock(m_mutexMainThreadEvents);
            eventsToProcess.swap(m_vecMainThreadEvents);
        }

        for (const auto& eventFunc : eventsToProcess)
        {
            if (eventFunc)
            {
                eventFunc();
            }
        }
    }

    inline void runOnMainThread(const std::function<void()>& func)
    {
        std::lock_guard<std::mutex> lock(m_mutexMainThreadEvents);
        m_vecMainThreadEvents.push_back(func);
    }

private:
    EngineEventDispatcher() {}
    ~EngineEventDispatcher() {}

    std::vector<std::function<void()>> m_vecMainThreadEvents;
    std::mutex m_mutexMainThreadEvents;
};