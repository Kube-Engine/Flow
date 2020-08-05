project(KubeSchedulerTests)

get_filename_component(KubeSchedulerTestsDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeSchedulerTestsSources
    ${KubeSchedulerTestsDir}/tests_Scheduler.cpp
    ${KubeSchedulerTestsDir}/tests_ThreadPool.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${KubeSchedulerTestsSources})

add_test(NAME ${CMAKE_PROJECT_NAME} COMMAND ${CMAKE_PROJECT_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME}
PUBLIC
    KubeScheduler
    GTest::GTest GTest::Main
)