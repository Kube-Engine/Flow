/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Graph
 */

#include <thread>

#include "Graph.hpp"

using namespace kF;

Flow::Graph::~Graph(void) noexcept_destructible(Data)
{
    if (running()) [[unlikely]]
        wait();
}

void Flow::Graph::wait(void) noexcept_ndebug
{
    kFAssert(!running() || repeat() == false,
        throw std::logic_error("Flow::Graph::wait: Waiting a repeating graph is unsafe"));
    while (running())
        std::this_thread::yield();
}

void Flow::Graph::clear(void) noexcept_destructible(Node)
{
    wait();
    _data->children.clear();
}
