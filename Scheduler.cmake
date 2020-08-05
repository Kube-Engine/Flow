project(KubeScheduler)

get_filename_component(KubeSchedulerDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeSchedulerSources
    ${KubeSchedulerDir}/ThreadPool.hpp
    ${KubeSchedulerDir}/ThreadPool.cpp
    ${KubeSchedulerDir}/ThreadPool.ipp
)

add_library(${PROJECT_NAME} ${KubeSchedulerSources})

target_link_libraries(${PROJECT_NAME}
PUBLIC
    KubeMeta
)

if(${KF_TESTS})
    include(${KubeSchedulerDir}/Tests/SchedulerTests.cmake)
endif()

if(${KF_BENCHMARKS})
    include(${KubeSchedulerDir}/Benchmarks/SchedulerBenchmarks.cmake)
endif()