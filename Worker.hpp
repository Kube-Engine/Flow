/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Worker thread
 */

#pragma once

#include <thread>

#include <Kube/Core/SPSCQueue.hpp>

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
    enum State {
        Waiting, // Waiting for tasks
        Working, // Working on a task
        Pause,   // Worker must wait until 'Resume' state is set
        Resume,  // Worker can resume its state
        Stop,    // Worker must stop as soon as possible and set its 'Stopped' state
        Stopped  // Worker is now stopped
    };

    /** @brief Construct and start the worker */
    Worker(Scheduler * const parent, const std::size_t queueSize);

    /** @brief Stop and join the worker */
    ~Worker(void);

    /** @brief Get internal state of worker */
    [[nodiscard]] State state(void) noexcept { return _state.load(); }

    /** @brief Push a task to be processed on the worker thread */
    [[nodiscard]] bool push(Task * const task) noexcept { return _queue.push(task); }

private:
    struct Cache
    {
        Scheduler *parent { nullptr };
        std::thread thd {};
    };

    KF_ALIGN_CACHELINE std::atomic<State> _state { State::Waiting };
    KF_ALIGN_CACHELINE Cache _cache {};
    Core::SPSCQueue<Task *> _queue;

    /** @brief Busy loop */
    void run(void) noexcept;

    /** @brief Execute a task */
    void work(Task * const task) noexcept;
};

static_assert(sizeof(kF::Flow::Worker) == 6 * kF::Core::Utils::CacheLineSize, "Worker is not padded correctly");
static_assert(alignof(kF::Flow::Worker) == 2 * kF::Core::Utils::CacheLineSize, "Worker is not aligned correctly");