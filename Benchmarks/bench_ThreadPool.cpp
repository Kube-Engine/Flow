/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: ThreadPool benchmark
 */

#include <benchmark/benchmark.h>

#include <Kube/Scheduler/ThreadPool.hpp>

#include <unistd.h>

using namespace kF;

static void DummyProcessReference(benchmark::State &state)
{
    constexpr auto size = 4096 * 1024;
    const auto threadIndex = state.thread_index + 1;
    char i = 0;

    auto buffer = new char[size];
    for (auto _ : state) {
        for (auto j = 0; j < size; ++j)
            buffer[j] = ++i * threadIndex;
        benchmark::DoNotOptimize(buffer);
    }
    delete buffer;
}
BENCHMARK(DummyProcessReference)->ThreadRange(1, 24);

static void DummyProcess(benchmark::State &state)
{
    constexpr auto size = 4096 * 1024;
    const auto threadIndex = state.thread_index + 1;
    char i = 0;

    auto buffer = new char[size];
    for (auto _ : state) {
        for (auto j = 0; j < size; ++j)
            buffer[j] = ++i * threadIndex;
        benchmark::DoNotOptimize(buffer);
    }
    delete buffer;
}