cmake_minimum_required(VERSION 3.12)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

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
if (${CMAKE_BUILD_TYPE} EQUAL "Release")
    target_compile_options(it45-project PRIVATE -O3)
endif()

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
    target_compile_options(it45-project-test PRIVATE -g)

    add_custom_target(test
        COMMAND it45-project-test
        DEPENDS it45-project-test
        WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
    )
endif()
