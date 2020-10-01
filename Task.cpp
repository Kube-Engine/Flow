/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Task
 */

#include <Kube/Core/Assert.hpp>

#include "Scheduler.hpp"

using namespace kF;

void Flow::Task::work(Scheduler * const parent)
{
    constexpr auto scheduleNode = [](Scheduler * const parent, Node * const link) {
        if (const auto count = link->linkedFrom.size(); count && count == ++link->joined) {
            link->joined = 0;
            parent->schedule(link);
        }
    };

    switch (type()) {
    case Node::Type::Static:
        std::get<static_cast<std::size_t>(Node::Type::Static)>(_node->workData)();
        for (Node * const link : _node->linkedTo)
            scheduleNode(parent, link);
        root()->childJoined();
        break;
    case Node::Type::Dynamic:
        std::get<static_cast<std::size_t>(Node::Type::Dynamic)>(_node->workData)(42);
        root()->childJoined();
        break;
    case Node::Type::Switch:
        const auto index = std::get<static_cast<std::size_t>(Node::Type::Switch)>(_node->workData)();
        const auto count = _node->linkedTo.size();
        kFAssert(index >= 0 && index < count,
            throw std::logic_error("Invalid switch task return index"));
        scheduleNode(parent, _node->linkedTo[index]);
        root()->childrenJoined(count);
        break;
    // case Node::Type::Graph:
    //     // const auto graph = std::get<static_cast<std::size_t>(Node::Type::Graph)>(_node->workData);
    //     parent->schedule(graph);
    }
}