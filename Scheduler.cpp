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

Flow::Task Flow::Scheduler::trySteal(void) noexcept
{
    return Task();
}

void Flow::Scheduler::wait(void) noexcept
{
    while (true) {
        {
            auto count = workerCount();
            for (auto &worker : _cache.workers) {
                if (worker.state() == Worker::State::Waiting && !worker.taskCount())
                    --count;
            }
            if (!count)
                return;
        }
        std::this_thread::yield();
    }
}