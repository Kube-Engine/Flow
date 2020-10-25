/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Node
 */

#pragma once

#include <memory_resource>
#include <variant>

#include <Kube/Core/FlatVector.hpp>
#include <Kube/Core/FlatString.hpp>

namespace kF::Flow
{
    struct Node;
    struct NodeInstance;
    struct Graph;

    /** @brief Static functor */
    using StaticFunc = std::function<void(void)>;

    /** @brief Dynamic functor */
    using DynamicFunc = std::function<void(int)>;

    /** @brief Switch functor */
    using SwitchFunc = std::function<std::size_t(void)>;

    /** @brief Notify functor to be called on the event thread */
    using NotifyFunc = std::function<void(void)>;

    /** @brief Variant holding work struct */
    using WorkData = std::variant<StaticFunc, DynamicFunc, SwitchFunc, Graph *>;

    /** @brief Empty work placeholder */
    constexpr auto EmptyWork = []{};
}

/** @brief A node is a POD structure containing all data of a scheduled task in a graph */
struct KF_ALIGN_CACHELINE kF::Flow::Node
{
    /** @brief Different types of nodes */
    enum class Type : std::size_t {
        Static = 0ul,
        Dynamic,
        Switch,
        Graph
    };

    // Cacheline 1, frequently used members
    WorkData workData {}; // Work data variant
    Core::FlatVector<Node *> linkedTo {}; // List of forward tasks
    Core::FlatVector<Node *> linkedFrom {}; // List of children
    Graph *root { nullptr };

    // Cacheline 2, rarely used members and frequently used atomic counter
    NotifyFunc notifyFunc {}; // Notify functor
    Core::FlatString name; // Node name
    std::atomic<std::size_t> joined { 0 }; // Joining
    std::atomic<bool> bypass { 0 }; // Bypass the node as if it was executed if true

    /** @brief Construct a node with a work functor */
    template<typename Work>
    Node(Work &&work) noexcept
        : workData(ForwardWorkData(std::forward<Work>(work))) {}

    /** @brief Construct a node with a work functor and a name */
    template<typename Work, typename Literal> requires std::constructible_from<decltype(Node::name), Literal>
    Node(Work &&work, Literal &&nodeName) noexcept
        : workData(ForwardWorkData(std::forward<Work>(work))), name(std::forward<Literal>(nodeName)) {}

    /** @brief Construct a node with a work and a notification functor */
    template<typename Work, typename Notify> requires std::constructible_from<NotifyFunc, Notify>
    Node(Work &&work, Notify &&notify) noexcept
        : workData(ForwardWorkData(std::forward<Work>(work))), notifyFunc(std::forward<Notify>(notify)) {}

    /** @brief Construct a node with a work and a notification functor and a name */
    template<typename Work, typename Notify, typename Literal> requires std::constructible_from<decltype(Node::name), Literal>
    Node(Work &&work, Notify &&notify, Literal &&nodeName) noexcept
        : workData(ForwardWorkData(std::forward<Work>(work))), notifyFunc(std::forward<Notify>(notify)), name(std::forward<Literal>(nodeName)) {}

    /** @brief Default destructor */
    ~Node(void) = default;

    /** @brief Helper to return the good workdata type from templated one */
    template<typename Work>
    inline static auto ForwardWorkData(Work &&work)
    {
        if constexpr (!std::is_same_v<DynamicFunc, Work> && std::is_constructible_v<DynamicFunc, Work>)
            return DynamicFunc(std::forward<Work>(work));
        else if constexpr (!std::is_same_v<SwitchFunc, Work> && std::is_constructible_v<SwitchFunc, Work>)
            return SwitchFunc(std::forward<Work>(work));
        else if constexpr (!std::is_same_v<StaticFunc, Work> && std::is_constructible_v<StaticFunc, Work>)
            return StaticFunc(std::forward<Work>(work));
        else if constexpr (std::is_same_v<Graph &, Work>)
            return &work;
        else
            return work;
    }
};

static_assert(sizeof(kF::Flow::Node) == kF::Core::Utils::CacheLineSize * 2, "Graph node must occupy 2 cachelines");

class kF::Flow::NodeInstance
{
public:
    /** @brief Default constructor */
    NodeInstance(void) = default;

    /** @brief Allocate constructor */
    template<typename ...Args>
    NodeInstance(Args &&...args)
        : _node(Allocate(std::forward<Args>(args)...)) {}

    /** @brief Move constructor */
    NodeInstance(NodeInstance &&other) noexcept { swap(other); }

    /** @brief Destroy the instance */
    ~NodeInstance(void) noexcept_destructible(Node) { if (_node) [[likely]] Deallocate(_node); }

    /** @brief Get node pointer */
    [[nodiscard]] Node *node(void) noexcept { return _node; }
    [[nodiscard]] const Node *node(void) const noexcept { return _node; }

    /** @brief Move assignment */
    NodeInstance &operator=(NodeInstance &&other) noexcept { swap(other); return *this; }

    /** @brief Swap two instances */
    void swap(NodeInstance &other) noexcept { std::swap(_node, other._node); }

    /** @brief Access operator */
    Node *operator->(void) noexcept { return _node; }
    const Node *operator->(void) const noexcept { return _node; }

private:
    Node *_node { nullptr };

    inline static std::pmr::synchronized_pool_resource _Pool {};

    template<typename ...Args>
    inline static Node *Allocate(Args &&...args)
        { return new (_Pool.allocate(sizeof(Node), alignof(Node))) Node(std::forward<Args>(args)...); }

    inline static void Deallocate(Node *node) noexcept_destructible(Node)
        { node->~Node(); return _Pool.deallocate(node, sizeof(Node), alignof(Node)); }
};
