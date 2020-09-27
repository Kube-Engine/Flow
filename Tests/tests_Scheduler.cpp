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
    auto func = [&trigger] {
        ++trigger;
    };
    Flow::Task task(func);

    task.children().emplace_back(func).children().emplace_back(func);
    scheduler.schedule(&task);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    ASSERT_EQ(trigger, 3);
}

TEST(Scheduler, NotifyTask)
{
    Flow::Scheduler scheduler(1);
    int trigger = false;
    auto func = [&trigger] {
        ++trigger;
    };
    Flow::Task task(func, func);

    scheduler.schedule(&task);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    scheduler.processNotifications();
    ASSERT_EQ(trigger, 2);
    auto func2 = [&trigger] {
        trigger += 2;
    };
    task = Flow::Task(func2, func2);
    scheduler.schedule(&task);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    scheduler.processNotifications();
    ASSERT_EQ(trigger, 6);
}
