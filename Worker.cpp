/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Worker thread
 */

#include <iostream>

#include "Scheduler.hpp"

using namespace kF;

Flow::Worker::Worker(Scheduler * const parent, const std::size_t queueSize)
    : _cache(Cache {
        parent: parent,
        thd: std::thread()
    }),
    _queue(queueSize)
{
}

void Flow::Worker::run(void) noexcept
{
    while (state() == State::Running) {
        if (Task task; _queue.pop(task) || _cache.parent->steal(task))
            work(task);
        else
            std::this_thread::yield();
    }
    _state = State::Stopped;
}

void Flow::Worker::work(Task &task) noexcept
{
    constexpr auto scheduleNode = [](Scheduler * const parent, Node * const link) {
        if (const auto count = link->linkedFrom.size(); count && count == ++link->joined) {
            link->joined = 0;
            parent->schedule(link);
        }
    };
    const auto parent = _cache.parent;
    const auto node = task.node();

    try {
        switch (task.type()) {
        case Node::Type::Static:
            std::get<static_cast<std::size_t>(Node::Type::Static)>(node->workData)();
            for (Node * const link : node->linkedTo)
                scheduleNode(parent, link);
            node->root->childJoined();
            break;
        case Node::Type::Dynamic:
            std::get<static_cast<std::size_t>(Node::Type::Dynamic)>(node->workData)(42);
            node->root->childJoined();
            break;
        case Node::Type::Switch:
        {
            const auto index = std::get<static_cast<std::size_t>(Node::Type::Switch)>(node->workData)();
            const auto count = node->linkedTo.size();
            kFAssert(index >= 0 && index < count,
                throw std::logic_error("Invalid switch task return index"));
            scheduleNode(parent, node->linkedTo[index]);
            node->root->childrenJoined(count);
            break;
        }
        case Node::Type::Graph:
        {
            const auto graph = std::get<static_cast<std::size_t>(Node::Type::Graph)>(node->workData);
            parent->schedule(*graph);
            while (graph->running() && state() == State::Running) {
                if (Task task; _queue.pop(task) || _cache.parent->steal(task))
                    work(task);
                else
                    std::this_thread::yield();
            }
            for (const auto link : node->linkedTo)
                scheduleNode(parent, link);
            node->root->childJoined();
            break;
        }
        default:
            throw std::logic_error("Flow::Worker::Work: Undefined node");
        }
        if (!node->notifyFunc) [[likely]]
            return;
        // If the task has notification, loop until parent scheduler receive it
        while (!parent->notify(task) && state() == State::Running) {
            if (Task task; _queue.pop(task) || _cache.parent->steal(task))
                work(task);
            else
                std::this_thread::yield();
        }
    } catch (const std::exception &e) {
        std::cout << "Flow::Worker::work: Exception thrown in task '" << task.name() << "': " << e.what() << std::endl;
    } catch (...) {
        std::cout << "Flow::Worker::work: Unknown exception thrown in task '" << task.name() << '\'' << std::endl;
    }
}