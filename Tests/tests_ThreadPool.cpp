/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of ThreadPool
 */

#include <cmath>

#include <gtest/gtest.h>

#include <Kube/Scheduler/ThreadPool.hpp>

using namespace kF;

TEST(ThreadPool, BasicsChainWithConversion)
{
    Meta::Resolver::Clear();
    Meta::RegisterMetadata();
    Scheduler::ThreadPool pool;
    int x = 42;

    pool.addTask([] { return 36; }, [&x](float i) { x = i; });
    while (x == 42) {
        pool.processEvents();
    }
    ASSERT_EQ(x, 36);
    pool.addEvent([&x] { x = 42; });
    ASSERT_EQ(x, 36);
    pool.processEvents();
    ASSERT_EQ(x, 42);
}

#include <iostream>

TEST(ThreadPool, LargeFeed)
{
    constexpr std::size_t MaxRange = 1024;
    constexpr std::size_t DataRange = 1024;

    Scheduler::ThreadPool pool;
    std::size_t counter = 0;

    for (auto i = 0; i < MaxRange; ++i) {
        pool.addTask(
            [] {
                std::vector<int> data(DataRange);
                for (auto i = 0u; i < DataRange; ++i)
                    data[i] = std::pow(i, i);
            },
            [&counter] { ++counter; }
        );
    }
    auto last = counter;
    while (true) {
        pool.processEvents();
        if (counter - last > 1000) {
            last = counter;
            std::cout << "Counter: " << counter << std::endl;
        }
        if (counter == MaxRange)
            break;
        else
            std::this_thread::yield();
    }
}