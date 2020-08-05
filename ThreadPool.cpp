/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Thread Pool
 */

#include "ThreadPool.hpp"

using namespace kF;

void Scheduler::ThreadHandler::start(ThreadPool *parent)
{
    _state = State::Running;
    _thd = std::thread([this, parent] { run(parent); });
}

void Scheduler::ThreadHandler::run(ThreadPool *parent)
{
    for (auto state = _state.load(); state != State::Stopped; state = _state.load()) {
        try {
            if (auto task = parent->getNextTask(); task) {
                task->work();
                parent->addEvent(std::move(task));
            } else
                std::this_thread::yield();
        } catch (const std::runtime_error &e) {
            parent->addEvent([e] { throw e; });
        } catch (const std::logic_error &e) {
            parent->addEvent([e] { throw e; });
        } catch (const std::exception &e) {
            parent->addEvent([e] { throw e; });
        }
    }
}

Scheduler::ThreadPool::ThreadPool(void)
    : _handlers(std::thread::hardware_concurrency())
{
    for (auto &handler : _handlers)
        handler.start(this);
}

Scheduler::ThreadPool::~ThreadPool(void)
{
    for (auto &handler : _handlers)
        handler.stop();
    for (auto &handler : _handlers)
        handler.join();
}

void Scheduler::ThreadPool::processEvents(void)
{
    for (auto consumer = _events.acquireConsumer(); consumer; consumer = _events.acquireConsumer()) {
        for (auto &task : consumer.data())
            task->emit();
    }
}

Scheduler::TaskPtr Scheduler::ThreadPool::getNextTask(void) noexcept
{
    if (auto consumer = _tasks.acquireConsumer(); consumer) {
        auto task = std::move(consumer.data().front());
        consumer.data().erase(consumer.data().begin());
        consumer.releaseNoClear();
        return task;
    } else
        return TaskPtr();
}