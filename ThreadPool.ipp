/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Thread Pool
 */

#include "ThreadPool.hpp"

template<typename Worker, typename WorkFunctor, typename Receiver, typename EventFunctor>
kF::Scheduler::Task::Task(const Worker &worker, WorkFunctor &&workFunctor, const Receiver &receiver, EventFunctor &&eventFunctor)
    :   _work(
        Meta::OpaqueFunctor::Construct<Receiver, WorkFunctor, Meta::Internal::FunctionDecomposerHelper<WorkFunctor>, true>(
            Meta::Internal::RetreiveOpaquePtr(worker),
            std::forward<WorkFunctor>(workFunctor)
        )),
        _event(Meta::OpaqueFunctor::Construct<Receiver, EventFunctor, Meta::Internal::FunctionDecomposerHelper<EventFunctor>, true>(
            Meta::Internal::RetreiveOpaquePtr(receiver),
            std::forward<EventFunctor>(eventFunctor)
        ))
{
    constexpr auto IsValidInstance = [] {
        using WorkDecomposer = Meta::Internal::FunctionDecomposerHelper<WorkFunctor>;
        using EventDecomposer = Meta::Internal::FunctionDecomposerHelper<EventFunctor>;

        if constexpr (std::is_same_v<WorkDecomposer, void> && std::is_same_v<EventDecomposer, void>)
            return false;
        else if constexpr (std::is_same_v<WorkDecomposer, void> && !std::is_same_v<EventDecomposer, void>)
            return std::tuple_size_v<typename EventDecomposer::ArgsTuple> == 0;
        else if constexpr (!std::is_same_v<WorkDecomposer, void> && std::is_same_v<EventDecomposer, void>)
            return true;
        else
            return  (std::is_same_v<typename WorkDecomposer::ReturnType, void> && std::tuple_size_v<typename EventDecomposer::ArgsTuple> == 0) ||
                    (!std::is_same_v<typename WorkDecomposer::ReturnType, void> && std::tuple_size_v<typename EventDecomposer::ArgsTuple> == 1);
    }();

    static_assert(IsValidInstance, "Task 'work' function return type must be the only parameter of the 'event' function");
}