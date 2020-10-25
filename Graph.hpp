/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Task graph
 */

#pragma once

#include <memory>

#include <Kube/Core/Assert.hpp>

#include "Task.hpp"

namespace kF::Flow
{
    class NodeInstance;
    class Graph;
}

class kF::Flow::Graph
{
public:
    /** @brief Data of the task graph */
    struct KF_ALIGN_CACHELINE Data
    {
        std::vector<NodeInstance> children; // Children instances
        std::atomic<std::size_t> joined { 0 }; // Number of joined parent
        std::atomic<bool> repeat { false }; // True if the graph should repeat indefinitly
        std::atomic<bool> running { false }; // True if the graph is already processing
    };

    /** @brief Shared pointer to data structure */
    using DataPtr = std::shared_ptr<Data>;

    /** @brief Children iterator */
    using Iterator = decltype(Data::children)::iterator;
    using ConstIterator = decltype(Data::children)::const_iterator;

    /** @brief Default construtor */
    Graph(void) noexcept = default;

    /** @brief Copy constructor */
    Graph(const Graph &other) noexcept = default;

    /** @brief Move constructor */
    Graph(Graph &&other) noexcept = default;

    /** @brief Destructor */
    ~Graph(void) noexcept_destructible(Data);

    /** @brief Copy assignment */
    Graph &operator=(const Graph &other) noexcept_copy_constructible(DataPtr) = default;

    /** @brief Move assignment */
    Graph &operator=(Graph &&other) noexcept_move_constructible(DataPtr) = default;

    /** @brief Fast check */
    operator bool(void) const noexcept { return _data.operator bool(); }

    /** @brief Swap two graph */
    void swap(Graph &other) noexcept { _data.swap(other._data); }

    /** @brief Construct an instance if not already done */
    void construct(void) noexcept;

    /** @brief Get / Set the repeat property */
    [[nodiscard]] bool repeat(void) const noexcept { return _data->repeat.load(std::memory_order_seq_cst); }
    void setRepeat(const bool repeat) noexcept { construct(); return _data->repeat.store(repeat, std::memory_order_seq_cst); }

    /** @brief Get the joined property */
    [[nodiscard]] std::size_t joined(void) const noexcept { return _data->joined.load(std::memory_order_seq_cst); }

    /** @brief Callback that increment join count (to know when graph is done) */
    void childJoined(void) noexcept { childrenJoined(1); }
    void childrenJoined(const std::size_t childrenJoined) noexcept;

    /** @brief Get / Set the running property */
    [[nodiscard]] bool running(void) const noexcept { return _data->running.load(std::memory_order_seq_cst); }
    void setRunning(const bool running) noexcept { return _data->running.store(running, std::memory_order_seq_cst); }

    /** @brief Emplace a node in the graph */
    template<typename ...Args>
    Task emplace(Args &&...args);

    /** @brief Wait for the graph to be executed */
    void wait(void) noexcept_ndebug;

    /** @brief Clear every node link (node are still valid) */
    void clearLinks(void) noexcept;

    /** @brief Clear the graph children */
    void clear(void) noexcept_destructible(Node);

    /** @brief Get the number of owned nodes */
    [[nodiscard]] std::size_t size(void) const noexcept { return _data->children.size(); }

    /** @brief Begin / end iterators to iterate over children nodes */
    [[nodiscard]] Iterator begin(void) noexcept { return _data->children.begin(); }
    [[nodiscard]] Iterator begin(void) const noexcept { return _data->children.begin(); }
    [[nodiscard]] ConstIterator end(void) noexcept { return _data->children.end(); }
    [[nodiscard]] ConstIterator end(void) const noexcept { return _data->children.end(); }

private:
    DataPtr _data {};
};

#include "Graph.ipp"