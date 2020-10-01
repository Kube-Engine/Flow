/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Graph
 */

inline void kF::Flow::Graph::construct(void) noexcept
{
    if (!_data) [[unlikely]]
        _data = std::make_shared<Data>();
}

inline void kF::Flow::Graph::childJoined(void) noexcept
{
    if (const auto count = _data->children.size(); ++_data->joined == count) {
        _data->joined = 0;
        _data->running = false;
    }
}

inline void kF::Flow::Graph::childrenJoined(const std::size_t childrenJoined) noexcept
{
    if (const auto count = _data->children.size(); (_data->joined += childrenJoined) == count) {
        _data->joined = 0;
        _data->running = false;
    }
}

template<typename ...Args, std::enable_if_t<std::is_constructible_v<kF::Flow::Node, Args...>, void>* = nullptr>
inline kF::Flow::Task kF::Flow::Graph::emplace(Args &&...args) noexcept_constructible(kF::Flow::Node, Args...)
{
    construct();
    const auto node = _data->children.emplace_back(std::forward<Args>(args)...).node();
    node->root = this;
    return Task(node);
}