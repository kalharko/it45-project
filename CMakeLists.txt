cmake_minimum_required(VERSION 3.12)

project(it45-project)

set(SRC_FILES
    src/load.c
    src/initial.c
    src/optimize.c
    src/utils.c
    src/score.c
    src/constraints.c
)

add_executable(it45-project
    src/main.c
    ${SRC_FILES}
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/)

target_link_libraries(it45-project PRIVATE m) # Link math library

# Testing project
find_package(unity CONFIG)

add_custom_target(run
    COMMAND it45-project
    DEPENDS it45-project
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

if (${unity_FOUND})
    project(it45-project-test)
    add_executable(it45-project-test
        test/main.c
        ${SRC_FILES}
    )

    target_link_libraries(it45-project-test unity::framework m)

    add_custom_target(test
        COMMAND it45-project-test
        DEPENDS it45-project-test
        WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
    )
endif()
