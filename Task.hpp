/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Task
 */

#pragma once

#include "Node.hpp"

namespace kF::Flow
{
    class Task;
}

/**
 * @brief A task is composed of two functors: 'work' and 'notify'
 *  The 'work' is to be called on a worker thread of Scheduler
 *  The 'notify' is to be called on the main thread to after 'work' processed
 */
class kF::Flow::Task
{
public:
    /** @brief Default constructor */
    Task(void) noexcept = default;

    /** @brief Construct with existing node */
    Task(Node * const node) noexcept : _node(node) {}

    /** @brief Default copy constructor */
    Task(const Task &other) noexcept = default;

    /** @brief Default move constructor */
    Task(Task &&other) noexcept = default;

    /** @brief Default destructor */
    ~Task(void) noexcept = default;

    /** @brief Copy assignment */
    Task &operator=(const Task &other) noexcept = default;

    /** @brief Move assignment */
    Task &operator=(Task &&other) noexcept = default;

    /** @brief Fast check if node is assigned */
    operator bool(void) const noexcept { return _node; }

    /** @brief Get the internal node pointer */
    [[nodiscard]] Node *node(void) noexcept { return _node; }
    [[nodiscard]] const Node *node(void) const noexcept { return _node; }

    /** @brief Retreive the type of the task */
    [[nodiscard]] Node::Type type(void) const noexcept { return static_cast<Node::Type>(_node->workData.index()); }

    /** @brief Set the work event */
    template<typename Work>
    void setWork(Work &&work) noexcept { _node->workData = Node::ForwardWorkData(std::forward<Work>(work)); }

    /** @brief Check if the Task has a notification event */
    [[nodiscard]] bool hasNotification(void) const noexcept { return _node->notifyFunc.operator bool(); }

    /** @brief Execute the notification event */
    void notify(void) { _node->notifyFunc(); }

    /** @brief Set the notification event */
    void setNotify(NotifyFunc &&notifyFunc) noexcept { _node->notifyFunc = notifyFunc; }

    /** @brief Get root graph */
    [[nodiscard]] Graph *root(void) noexcept { return _node->root; }
    [[nodiscard]] const Graph *root(void) const noexcept { return _node->root; }

    /** @brief Get / Set the name property */
    [[nodiscard]] std::string_view name(void) const noexcept { return _node->name.toStdView(); }
    void setName(const std::string_view &name) noexcept { _node->name = name; }

    /** @brief Get / Set the name property */
    [[nodiscard]] bool bypass(void) const noexcept { return _node->bypass.load(); }
    void setBypass(const bool &bypass) noexcept { _node->bypass.store(bypass); }

    /** @brief Add a task linked to this instance */
    Task &precede(Task &task) noexcept;

    /** @brief Add a task linked from this instance */
    Task &succeed(Task &task) noexcept { task.precede(*this); return *this; }

private:
    Node *_node { nullptr };
};

#include "Task.ipp"