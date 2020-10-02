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