/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Task Scheduler
 */

#include "Scheduler.hpp"

using namespace kF;

Flow::Scheduler::Scheduler(const std::size_t workerCount, const std::size_t taskQueueSize, const std::size_t notificationQueueSize)
    : _notifications(notificationQueueSize)
{
    auto count = workerCount;
    if (count == AutoWorkerCount)
        count = std::thread::hardware_concurrency();
    if (!count)
        count = DefaultWorkerCount;
    _cache.workers.allocate(count, this, taskQueueSize);
}

Flow::Scheduler::~Scheduler(void)
{

}

void Flow::Scheduler::schedule(Task * const task) noexcept
{
    do {
        if (++_cache.lastWorkerId >= _cache.workers.size()) [[unlikely]]
            _cache.lastWorkerId = 0;
    } while (!_cache.workers[_cache.lastWorkerId].push(task));
}

Flow::Task *Flow::Scheduler::trySteal(void) noexcept
{
    return nullptr;
}

void Flow::Scheduler::processNotifications(void)
{
    Task *task;

    while (_notifications.pop(task))
        task->notify();
}