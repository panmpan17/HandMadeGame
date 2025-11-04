#pragma once

#include <vector>
#include <functional>

struct VoidEvent
{
    std::vector<std::function<void()>> listeners;

    void add(std::function<void()> f) { listeners.push_back(f); }
    // void remove(std::function<void(Args...)> f) 
    // { 
    //     listeners.erase(std::remove(listeners.begin(), listeners.end(), f), listeners.end()); 
    // }
    void invoke() {
        for (auto& listener : listeners)
        {
            listener();
        }
    }
};

template<typename... Args>
struct Event
{
    std::vector<std::function<void(Args...)>> listeners;

    void add(std::function<void(Args...)> f) { listeners.push_back(f); }
    // void remove(std::function<void(Args...)> f) 
    // { 
    //     listeners.erase(std::remove(listeners.begin(), listeners.end(), f), listeners.end()); 
    // }
    void invoke(Args... args) {
        for (auto& listener : listeners)
        {
            listener(args...);
        }
    }
};
