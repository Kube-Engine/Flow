/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Types of nodes
 */

#pragma once

#include <functional>

namespace kF::Flow
{
    class Graph;

    /** @brief Static functor */
    using StaticFunc = std::function<void(void)>;

    /** @brief Switch functor */
    using SwitchFunc = std::function<std::size_t(void)>;

    /** @brief Dynamic functor */
    using DynamicFunc = std::function<void(Graph &)>;

    /** @brief Notify functor to be called on the event thread */
    using NotifyFunc = std::function<void(void)>;

    /** @brief Different types of nodes */
    enum class NodeType : std::size_t {
        Static = 0ul,
        Dynamic,
        Switch,
        Graph
    };

    /** @brief Empty work placeholder */
    constexpr auto EmptyWork = []{};
}