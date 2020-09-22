project(KubeFlowTests)

get_filename_component(KubeFlowTestsDir ${CMAKE_CURRENT_LIST_FILE} PATH)

set(KubeFlowTestsSources
    ${KubeFlowTestsDir}/tests_Scheduler.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${KubeFlowTestsSources})

add_test(NAME ${CMAKE_PROJECT_NAME} COMMAND ${CMAKE_PROJECT_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME}
PUBLIC
    KubeFlow
    GTest::GTest GTest::Main
)