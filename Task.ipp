/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Task
 */


inline kF::Flow::NodeType kF::Flow::Task::type(void) const noexcept
{
    return static_cast<NodeType>(_node->workData.index());
}

template<typename Work>
inline void kF::Flow::Task::setWork(Work &&work) noexcept
{
    _node->workData = Node::ForwardWorkData(std::forward<Work>(work));
}

inline bool kF::Flow::Task::hasNotification(void) const noexcept
{
    return _node->notifyFunc.operator bool();
}

inline void kF::Flow::Task::notify(void)
{
    _node->notifyFunc();
}

inline void kF::Flow::Task::setNotify(NotifyFunc &&notifyFunc) noexcept
{
    _node->notifyFunc = std::move(notifyFunc);
}

inline kF::Flow::Graph *kF::Flow::Task::root(void) noexcept
{
    return _node->root;
}

inline const kF::Flow::Graph *kF::Flow::Task::root(void) const noexcept
{
    return _node->root;
}

inline std::string_view kF::Flow::Task::name(void) const noexcept
{
    return _node->name.toStdView();
}

inline void kF::Flow::Task::setName(const std::string_view &name) noexcept
{
    _node->name = name;
}

inline bool kF::Flow::Task::bypass(void) const noexcept
{
    return _node->bypass.load();
}

inline void kF::Flow::Task::setBypass(const bool &bypass) noexcept
{
    _node->bypass.store(bypass);
}

inline kF::Flow::Task &kF::Flow::Task::precede(Task &task) noexcept
{
    _node->linkedTo.push(task._node);
    task._node->linkedFrom.push(_node);
    return *this;
}