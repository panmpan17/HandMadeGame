#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>
#include <functional>

struct VoidEvent
{
    struct Entry
    {
        std::size_t id;
        std::function<void()> fn;
    };

    std::vector<Entry> listeners;
    std::size_t nextId = 1;

    std::size_t add(std::function<void()> f)
    {
        listeners.push_back({ nextId, std::move(f) });
        return nextId++;
    }

    void remove(std::size_t id)
    {
        listeners.erase(
            std::remove_if(listeners.begin(), listeners.end(), [id](const Entry& entry) { return entry.id == id; }),
            listeners.end());
    }

    void invoke() {
        for (auto& listener : listeners)
        {
            listener.fn();
        }
    }
};

template<typename... Args>
struct CustomEvent
{
    struct Entry
    {
        std::size_t id;
        std::function<void(Args...)> fn;
    };

    std::vector<Entry> listeners;
    std::size_t nextId = 1;

    std::size_t add(std::function<void(Args...)> f)
    {
        listeners.push_back({ nextId, std::move(f) });
        return nextId++;
    }

    void remove(std::size_t id)
    {
        listeners.erase(
            std::remove_if(listeners.begin(), listeners.end(), [id](const Entry& entry) { return entry.id == id; }),
            listeners.end());
    }

    void invoke(Args... args) {
        for (auto& listener : listeners)
        {
            listener.fn(args...);
        }
    }
};
