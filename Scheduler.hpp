/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Task scheduler
 */

#pragma once

#include <vector>

#include <Kube/Core/MPMCQueue.hpp>
#include <Kube/Core/HeapArray.hpp>

#include "Worker.hpp"

namespace kF::Flow
{
    class Scheduler;

    using Workers = Core::HeapArray<Worker>;
};

class KF_ALIGN_CACHELINE2 kF::Flow::Scheduler
{
public:
    static constexpr std::size_t AutoWorkerCount { 0ul };
    static constexpr std::size_t DefaultWorkerCount { 4ul };
    static constexpr std::size_t DefaultTaskQueueSize { 4096ul };
    static constexpr std::size_t DefaultNotificationQueueSize { 4096ul };

    /** @brief Construct a set of workers */
    Scheduler(const std::size_t workerCount = AutoWorkerCount, const std::size_t taskQueueSize = DefaultTaskQueueSize, const std::size_t notificationQueueSize = DefaultNotificationQueueSize);

    /** @brief Destroy and join all workers */
    ~Scheduler(void);

    /** @brief Schedule a set of tasks */
    void schedule(Task * const task) noexcept;

    /** @brief Tries to steal a task from a busy worker (only used by workers) */
    [[nodiscard]] Task *trySteal(void) noexcept;

    /** @brief Tries to add a notification task to be executed on the event processing thread */
    [[nodiscard]] bool notify(Task * const task) noexcept { return _notifications.push(task); }

    /** @brief Process all pending notifications on the current thread */
    void processNotifications(void);

    /** @brief Get the count of worker */
    [[nodiscard]] std::size_t workerCount(void) const noexcept { return _cache.workers.size(); }

private:
    struct Cache
    {
        Workers workers {};
        std::size_t lastWorkerId { 0 };
    };

    KF_ALIGN_CACHELINE2 Cache _cache;
    Core::MPMCQueue<Task *> _notifications;
};