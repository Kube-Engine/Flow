/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Task
 */

#include <Kube/Core/Assert.hpp>

#include "Scheduler.hpp"

using namespace kF;

void Flow::Task::work(Scheduler * const parent)
{
    switch (type()) {
    case Type::Static:
        std::get<static_cast<std::size_t>(Type::Static)>(_workFunc)();
        for (auto &child : _children)
            parent->schedule(&child);
        break;
    case Type::Dynamic:
        std::get<static_cast<std::size_t>(Type::Dynamic)>(_workFunc)(42);
        break;
    case Type::Condition:
        if (std::get<static_cast<std::size_t>(Type::Condition)>(_workFunc)())
            parent->schedule(&_children[0]);
        else
            parent->schedule(&_children[1]);
        break;
    case Type::Switch:
        const auto index = std::get<static_cast<std::size_t>(Type::Condition)>(_workFunc)();
        kFAssert(index >= 0 && index < _children.size(),
            throw std::logic_error("Invalid switch task return index"));
        parent->schedule(&_children[index]);
        break;
    }
}