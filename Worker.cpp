/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Worker thread
 */

#include "Scheduler.hpp"

using namespace kF;

Flow::Worker::Worker(Scheduler * const parent, const std::size_t queueSize)
    : _cache(Cache {
        parent: parent,
        thd: std::thread()
    }),
    _queue(queueSize, false)
{
    start();
}

Flow::Worker::~Worker(void)
{
    stop();
}

void Flow::Worker::start(void)
{
    const auto state = _state.load();

    if (state != Stopped)
        return;
    _state = State::Waiting;
    _cache.thd = std::thread([this] { run(); });
}

void Flow::Worker::stop(void)
{
    if (auto currentState = state(); currentState != State::Stop && currentState != State::Stopped)
        while (!_state.compare_exchange_strong(currentState, State::Stop));
    if (_cache.thd.joinable())
        _cache.thd.join();
}

void Flow::Worker::run(void) noexcept
{
    Task task;
    State current;

    while (true) {
        current = _state.load();
        switch (current) {
        case State::Waiting:
            if (task || _queue.pop(task) || (task = _cache.parent->trySteal()))
                _state.compare_exchange_strong(current, State::Working);
            else
                std::this_thread::yield();
            break;
        case State::Working:
            work(task);
            task = nullptr;
            _state.compare_exchange_weak(current, State::Waiting);
            break;
        case State::Pause:
            break;
        case State::Resume:
            _state.compare_exchange_strong(current, State::Waiting);
            break;
        case State::Stop:
            _state.compare_exchange_strong(current, State::Stopped);
            break;
        case State::Stopped:
            return;
        }
    }
}

void kF::Flow::Worker::work(Task &task) noexcept
{
    try {
        task.work(_cache.parent);
        if (task.hasNotification())
            while (!_cache.parent->notify(task))
                std::this_thread::yield();
    } catch (const std::exception &e) {

    } catch (...) {

    }
}