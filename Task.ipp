/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Task
 */

inline kF::Flow::Task &kF::Flow::Task::precede(Task &task) noexcept
{
    _node->linkedTo.push(task._node);
    task._node->linkedFrom.push(_node);
    return *this;
}