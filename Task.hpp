/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Task
 */

#pragma once

#include <functional>
#include <variant>
#include <string>

namespace kF::Flow
{
    class Task;
    class Scheduler;

    using Tasks = std::vector<Task>;
}

/**
 * @brief A task is composed of two functors: 'work' and 'notify'
 *  The 'work' is to be called on a worker thread of Scheduler
 *  The 'notify' is to be called on the main thread to after 'work' processed
 */
class KF_ALIGN_CACHELINE2 kF::Flow::Task
{
public:
    using StaticFunc = std::function<void(void)>;
    using DynamicFunc = std::function<void(int)>;
    using ConditionFunc = std::function<bool(void)>;
    using SwitchFunc = std::function<int(void)>;
    using NotifyFunc = std::function<void(void)>;
    using WorkFunc = std::variant<StaticFunc, DynamicFunc, ConditionFunc, SwitchFunc>;

    /** @brief Different types of nodes */
    enum class Type : std::size_t {
        Static = 0ul,
        Dynamic,
        Condition,
        Switch
    };

    /** @brief Default constructor */
    Task(void) = default;

    /** @brief Default copy constructor */
    Task(const Task &other) = default;

    /** @brief Default move constructor */
    Task(Task &&other) = default;

    /** @brief Emplace constructor */
    template<typename ...Args>
    Task(Args &&...args) noexcept { emplace(std::forward<Args>(args)...); }

    /** @brief Default destructor */
    ~Task(void) = default;

    /** @brief Copy assignment */
    Task &operator=(const Task &other) noexcept = default;

    /** @brief Move assignment */
    Task &operator=(Task &&other) noexcept = default;

    /** @brief Retreive the type of the task */
    [[nodiscard]] Type type(void) const noexcept { return static_cast<Type>(_workFunc.index()); }

    /** @brief Execute the task */
    void work(Scheduler * const parent);

    /** @brief Check if the Task has a notification event */
    [[nodiscard]] bool hasNotification(void) const noexcept { return _notifyFunc.operator bool(); }

    /** @brief Execute the notification event */
    void notify(void) { _notifyFunc(); }

    /** @brief Set the notification event */
    void setNotify(NotifyFunc &&notifyFunc) noexcept { _notifyFunc = notifyFunc; }

    /** @brief Get / Set the name property */
    [[nodiscard]] const std::string &name(void) const noexcept { return _name; }
    void setName(const std::string &name) noexcept { _name = name; }

    /** @brief Get children tasks */
    [[nodiscard]] Tasks &children(void) noexcept { return _children; }
    [[nodiscard]] const Tasks &children(void) const noexcept { return _children; }

    /** @brief Emplace a node in this instance */
    template<typename ...Args>
    void emplace(Args &&...args) noexcept { emplace(std::forward<Args>(args)...); }
    void emplace(WorkFunc &&workFunc) noexcept { emplace(std::move(workFunc), NotifyFunc(), std::string()); }
    void emplace(WorkFunc &&workFunc, std::string &&name) noexcept { emplace(std::move(workFunc), NotifyFunc(), std::move(name)); }
    void emplace(WorkFunc &&workFunc, NotifyFunc &&notifyFunc) noexcept { emplace(std::move(workFunc), std::move(notifyFunc), std::string()); }
    void emplace(WorkFunc &&workFunc, NotifyFunc &&notifyFunc, std::string &&name) noexcept { _workFunc = std::move(workFunc); _notifyFunc = std::move(_notifyFunc); _name = std::move(name); }

private:
    WorkFunc _workFunc {};
    Tasks _children {};
    NotifyFunc _notifyFunc {};
    std::string _name {};
};