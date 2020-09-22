/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of Scheduler
 */

#include <gtest/gtest.h>

#include <Kube/Flow/Scheduler.hpp>

using namespace kF;

TEST(Scheduler, InitDestroy)
{
    for (auto i = 1ul; i <= 16; ++i) {
        Flow::Scheduler scheduler(i);
        ASSERT_EQ(scheduler.workerCount(), i);
    }
}

TEST(Scheduler, BasicTask)
{
    Flow::Scheduler scheduler(1);
    bool trigger = false;
    Flow::Task task([&trigger] {
        trigger = true;
    });

    scheduler.schedule(&task);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(trigger, true);
}

TEST(Scheduler, SequenceTask)
{
    Flow::Scheduler scheduler(1);
    int trigger = false;
    Flow::Task task([&trigger] {
        ++trigger;
    });

    task.children().emplace_back(task).children().emplace_back(task);
    scheduler.schedule(&task);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(trigger, 3);
}
