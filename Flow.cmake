project(KubeFlow)

get_filename_component(KubeFlowDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeFlowSources
    ${KubeFlowDir}/Scheduler.hpp
    ${KubeFlowDir}/Scheduler.cpp
    ${KubeFlowDir}/Scheduler.ipp
    ${KubeFlowDir}/Worker.hpp
    ${KubeFlowDir}/Worker.cpp
    ${KubeFlowDir}/Graph.hpp
    ${KubeFlowDir}/Graph.ipp
    ${KubeFlowDir}/Graph.cpp
    ${KubeFlowDir}/Task.hpp
    ${KubeFlowDir}/Task.ipp
    ${KubeFlowDir}/Node.hpp
)

add_library(${PROJECT_NAME} ${KubeFlowSources})

target_link_libraries(${PROJECT_NAME}
PUBLIC
    KubeCore
    AtomicWait
)

if(${KF_TESTS})
    include(${KubeFlowDir}/Tests/FlowTests.cmake)
endif()

if(${KF_BENCHMARKS})
    include(${KubeFlowDir}/Benchmarks/FlowBenchmarks.cmake)
endif()