/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Task graph
 */

#pragma once

#include <memory>

#include "Task.hpp"

namespace kF::Flow
{
    class Graph;
}

class kF::Flow::Graph
{
public:
    /** @brief Data of the task graph */
    struct Data
    {
        std::vector<Task> children;
        std::atomic<int> refCount { 0 };
        std::atomic<bool> repeat { false };
        std::atomic<bool> running { false };
        char padding[2];
    };
    static_assert(sizeof(Data) == 32ul);

    /** @brief Default construtor */
    Graph(void) { construct(); }

    /** @brief Copy constructor */
    Graph(const Graph &other) noexcept { acquire(other); }

    /** @brief Move constructor */
    Graph(Graph &&other) noexcept { swap(other); }

    /** @brief Destructor */
    ~Graph(void) noexcept_destructible(Data) { release(); }

    /** @brief Fast check */
    operator bool(void) const noexcept { return _data; }

    /** @brief Swap two graph */
    void swap(Graph &other) noexcept { std::swap(_data, other._data); }

    /** @brief Construct a new graph */
    void construct(void)
    {
        if (_data) [[unlikely]]
            release();
        _data = new Data();
    }

    /** @brief Acquire other pointer */
    void acquire(const Graph &other) noexcept
    {
        ++_data->refCount;
        _data = other._data;
    }

    /** @brief Release the reference count */
    void release(void) noexcept_destructible(Data)
    {
        if (_data && --_data->refCount == 0) [[likely]]
            delete _data;
    }

    /** @brief Get / Set the repeat property */
    [[nodiscard]] bool repeat(void) const noexcept { return _data->repeat.load(); }
    void setRepeat(const bool repeat) const noexcept { return _data->repeat.store(repeat); }

    /** @brief Get the reference count */
    [[nodiscard]] bool refCount(void) const noexcept { return _data->refCount.load(); }

    /** @brief Get / Set the running property */
    [[nodiscard]] bool running(void) const noexcept { return _data->running.load(); }
    void setRunning(const bool running) const noexcept { return _data->running.store(running); }

    /** @brief Get children */
    [[nodiscard]] Tasks &children(void) noexcept { return _data->children; }
    [[nodiscard]] const Tasks &children(void) const noexcept { return _data->children; }

private:
    Data *_data { nullptr };
};