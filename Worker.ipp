/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Worker
 */

inline void kF::Flow::Worker::start(void)
{
    const auto state = _state.load();

    if (state != State::Stopped)
        throw std::logic_error("Flow::Worker::start: Worker already running");
    _state = State::Running;
    _cache.thd = std::thread([this] { run(); });
}

inline void kF::Flow::Worker::stop(void) noexcept
{
    auto currentState = state();
    while (currentState == State::Running && !_state.compare_exchange_strong(currentState, State::Stopping));
}

inline void kF::Flow::Worker::join(void) noexcept
{
    while (state() != State::Stopped) { // To be replaced by atomic wait when available
        std::this_thread::yield();
    }
    if (_cache.thd.joinable())
        _cache.thd.join();
}

inline void kF::Flow::Worker::scheduleNode(Node * const node)
{
    if (const auto count = node->linkedFrom.size(); count && count == ++node->joined) {
        node->joined = 0;
        _cache.parent->schedule(node);
    }
}

inline void kF::Flow::Worker::blockingGraphSchedule(Graph &graph)
{
    _cache.parent->schedule(graph);
    while (graph.running() && state() == State::Running) {
        if (Task task; _queue.pop(task) || _cache.parent->steal(task))
            work(task);
        else
            std::this_thread::yield();
    }
}

inline std::uint32_t kF::Flow::Worker::dispatchStaticNode(Node * const node)
{
    if (!node->bypass.load()) [[likely]]
        std::get<static_cast<std::size_t>(NodeType::Static)>(node->workData)();
    for (Node * const link : node->linkedTo)
        scheduleNode(link);
    return 1u;
}

inline std::uint32_t kF::Flow::Worker::dispatchDynamicNode(Node * const node)
{
    if (!node->bypass.load()) [[likely]] {
        auto &dynamic = std::get<static_cast<std::size_t>(NodeType::Dynamic)>(node->workData);
        dynamic.func(dynamic.graph);
        blockingGraphSchedule(dynamic.graph);
    }
    return 1u;
}

inline std::uint32_t kF::Flow::Worker::dispatchSwitchNode(Node * const node)
{
    const auto index = std::get<static_cast<std::size_t>(NodeType::Switch)>(node->workData)();
    const auto count = node->linkedTo.size();

    kFAssert(!node->bypass.load(),
        throw std::logic_error("A branch task can't be bypassed"));
    kFAssert(index >= 0ul && index < count,
        throw std::logic_error("Invalid switch task return index"));
    scheduleNode(node->linkedTo[index]);
    return count;
}

inline std::uint32_t kF::Flow::Worker::dispatchGraphNode(Node * const node)
{
    if (!node->bypass.load()) [[likely]] {
        auto &graph = std::get<static_cast<std::size_t>(NodeType::Graph)>(node->workData);
        blockingGraphSchedule(graph);
    }
    for (const auto link : node->linkedTo)
        scheduleNode(link);
    return 1u;
}