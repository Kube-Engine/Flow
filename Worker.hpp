/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Worker thread
 */

#pragma once

#include <thread>

#include <Kube/Core/MPMCQueue.hpp>

#include "Graph.hpp"

namespace kF::Flow
{
    class Worker;
    class Scheduler;
};

class KF_ALIGN_CACHELINE2 kF::Flow::Worker
{
public:
    /** @brief Current state of the worker */
    enum class State {
        Running,    // Working is running
        Stopping,   // Worker is stopping
        Stopped,    // Worker is stopped
    };

    /** @brief Construct and start the worker */
    Worker(Scheduler * const parent, const std::size_t queueSize);

    /** @brief Destroy the worker without stopping it ! */
    ~Worker(void) = default;

    /** @brief Start the worker */
    void start(void);

    /** @brief Stop the worker */
    void stop(void) noexcept;

    /** @brief Join the worker */
    void join(void) noexcept;

    /** @brief Get internal state of worker */
    [[nodiscard]] State state(void) noexcept { return _state.load(std::memory_order_relaxed); }

    /** @brief Push a task to be processed on the worker thread */
    [[nodiscard]] bool push(const Task task) noexcept { return _queue.push(task); }

    /** @brief Try to steal a task from worker */
    [[nodiscard]] bool steal(Task &task) noexcept { return _queue.pop(task); }

    /** @brief Get the task count of the queue */
    [[nodiscard]] std::size_t taskCount(void) const noexcept { return _queue.size(); }

private:
    struct Cache
    {
        Scheduler *parent { nullptr };
        std::thread thd {};
    };

    KF_ALIGN_CACHELINE std::atomic<State> _state { State::Stopped };
    KF_ALIGN_CACHELINE Cache _cache {};
    Core::MPMCQueue<Task> _queue;

    /** @brief Busy loop */
    void run(void) noexcept;

    /** @brief Execute a task */
    void work(Task &task) noexcept;
};

static_assert(sizeof(kF::Flow::Worker) == 6 * kF::Core::Utils::CacheLineSize, "Worker is not padded correctly");
static_assert(alignof(kF::Flow::Worker) == 2 * kF::Core::Utils::CacheLineSize, "Worker is not aligned correctly");

#include "Worker.ipp"