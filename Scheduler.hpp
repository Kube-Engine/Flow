/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Scheduler
 */

#pragma once

#include <vector>

namespace kF
{
    class Task;
    class Pipeline;
    class Scheduler;

    enum class SchedulerPriority {
        Low = 0,
        Medium,
        High,
        Critical
    };
}

class kF::Task
{
public:

private:
};

class kF::Pipeline
{
public:

private:
    std::vector<std::vector<Task>> _graph;
};

class kF::Scheduler
{
public:

private:
    std::vector<Pipeline> _pipelines;
};