/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Scheduler
 */

#pragma once

inline void kF::Flow::Scheduler::schedule(Graph &graph) noexcept_ndebug
{
    if (graph.joined())
    kFAssert(graph.running() == false,
        throw std::logic_error("Flow::Scheduler::schedule: Can't schedule a graph if it is already running"));
    graph.setRunning(true);
    for (auto &child : graph) {
        if (child->linkedFrom.empty())
            schedule(Task(child.node()));
    }
}

inline void kF::Flow::Scheduler::schedule(const Task task) noexcept
{
    while (true) {
        if (++_cache.lastWorkerId >= _cache.workers.size()) [[unlikely]]
            _cache.lastWorkerId = 0;
        if (_cache.workers[_cache.lastWorkerId].push(task)) [[likely]]
            break;
    }
}