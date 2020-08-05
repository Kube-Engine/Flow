/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Thread Pool
 */

#pragma once

#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include <Kube/Core/SafeQueue.hpp>
#include <Kube/Meta/Meta.hpp>

namespace kF::Scheduler
{
    class Task;
    class ThreadHandler;
    class ThreadPool;

    using TaskPtr = std::unique_ptr<Task>;
}

class kF::Scheduler::Task
{
public:
    Task(void) = default;
    template<typename Worker, typename WorkFunctor, typename Receiver, typename EventFunctor>
    Task(const Worker &worker, WorkFunctor &&workFunctor, const Receiver &receiver, EventFunctor &&eventFunctor);
    Task(Task &&other) = default;
    ~Task(void) = default;
    Task &operator=(Task &&other) = default;

    void work(void) {
        _work.data = ((*_work.invokeFunc)(_work.data, _work.receiver, nullptr));
    }

    void emit(void) {
        if (_work.data.isVoid())
            (*_event.invokeFunc)(_event.data, _event.receiver, nullptr);
        else
            (*_event.invokeFunc)(_event.data, _event.receiver, &_work.data);
    }

private:
    Meta::OpaqueFunctor _work, _event;
};

class kF::Scheduler::ThreadHandler
{
public:
    enum class State : bool {
        Stopped = false,
        Running = true
    };

    ThreadHandler(void) noexcept = default;
    ThreadHandler(ThreadHandler &&) = delete;

    ~ThreadHandler(void) noexcept { stop(); join(); }

    void start(ThreadPool *parent);

    void stop(void) noexcept { _state = State::Stopped; }

    void join(void) noexcept { if (_thd.joinable()) _thd.join(); }

private:
    std::atomic<State> _state { State::Stopped };
    std::thread _thd {};

    void run(ThreadPool *parent);
};

class kF::Scheduler::ThreadPool
{
public:
    ThreadPool(void);
    ThreadPool(ThreadPool &&other) noexcept = default;
    ~ThreadPool(void);
    ThreadPool &operator=(ThreadPool &&other) = default;

    /** @brief Process and consume all delayed events */
    void processEvents(void);

    /** @brief Add a task in one thread's queue in order to be run asynchronously whenever possible */
    void addTask(TaskPtr &&task) noexcept { _tasks.acquireProducer().data().emplace_back(std::move(task)); }

    /** @brief Helpers used to add a task using internal task allocator */
    template<typename Worker, typename WorkFunctor, typename Receiver, typename EventFunctor>
    void addTask(const Worker &worker, WorkFunctor &&workFunctor, const Receiver &receiver, EventFunctor &&eventFunctor) noexcept { addTask(makeTask(worker, std::forward<WorkFunctor>(workFunctor), receiver, std::forward<EventFunctor>(eventFunctor))); }
    template<typename WorkFunctor, typename Receiver, typename EventFunctor>
    void addTask(WorkFunctor &&workFunctor, const Receiver &receiver, EventFunctor &&eventFunctor) noexcept { addTask(nullptr, std::forward<WorkFunctor>(workFunctor), receiver, std::forward<EventFunctor>(eventFunctor)); }
    template<typename Worker, typename WorkFunctor, typename EventFunctor>
    void addTask(const Worker &worker, WorkFunctor &&workFunctor, EventFunctor &&eventFunctor) noexcept { addTask(worker, std::forward<WorkFunctor>(workFunctor), nullptr, std::forward<EventFunctor>(eventFunctor)); }
    template<typename WorkFunctor, typename EventFunctor>
    void addTask(WorkFunctor &&workFunctor, EventFunctor &&eventFunctor) noexcept { addTask(nullptr, std::forward<WorkFunctor>(workFunctor), nullptr, std::forward<EventFunctor>(eventFunctor)); }

    /** @brief Add an event to be called on the next 'processEvents' call */
    void addEvent(TaskPtr &&task) noexcept { _events.acquireProducer().data().emplace_back(std::move(task)); }

    /** @brief Helpers used to add an event using internal task allocator */
    template<typename Receiver, typename EventFunctor>
    void addEvent(const Receiver &receiver, EventFunctor &&eventFunctor) noexcept { addEvent(makeTask(nullptr, nullptr, receiver, std::forward<EventFunctor>(eventFunctor))); }
    template<typename EventFunctor>
    void addEvent(EventFunctor &&eventFunctor) noexcept { addEvent(makeTask(nullptr, nullptr, nullptr, std::forward<EventFunctor>(eventFunctor))); }

    [[nodiscard]] TaskPtr getNextTask(void) noexcept;

private:
    std::vector<ThreadHandler> _handlers;
    Core::SafeQueue<TaskPtr> _tasks;
    Core::SafeQueue<TaskPtr> _events;
    // std::pmr::polymorphic_allocator _allocator;

    template<typename ...Args>
    [[nodiscard]] TaskPtr makeTask(Args &&...args) const noexcept { return std::make_unique<Task>(std::forward<Args>(args)...); }
};

#include "ThreadPool.ipp"