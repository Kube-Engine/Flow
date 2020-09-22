project(KubeFlowBenchmarks)

get_filename_component(KubeFlowBenchmarksDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeFlowBenchmarksSources
    ${KubeFlowBenchmarksDir}/Main.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${KubeFlowBenchmarksSources})

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${KubeFlowBenchmarksDir})

target_link_libraries(${CMAKE_PROJECT_NAME}
PUBLIC
    KubeFlow
    benchmark::benchmark
)