project(KubeSchedulerBenchmarks)

get_filename_component(KubeSchedulerBenchmarksDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeSchedulerBenchmarksSources
    ${KubeSchedulerBenchmarksDir}/Main.cpp
    ${KubeSchedulerBenchmarksDir}/bench_ThreadPool.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${KubeSchedulerBenchmarksSources})

target_link_libraries(${CMAKE_PROJECT_NAME}
PUBLIC
    KubeScheduler
    benchmark::benchmark
)