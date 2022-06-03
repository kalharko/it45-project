cmake_minimum_required(VERSION 3.12)

project(it45-project)

set(SRC_FILES
    src/load.c
    src/initial.c
    src/optimize.c
)

add_executable(it45-project
    src/main.c
    ${SRC_FILES}
)

include_directories(src/)

# Testing project

project(it45-project-test)
add_executable(it45-project-test
    test/main.c
    ${SRC_FILES}
)
# TODO: use cmake's find_library
include_directories(/usr/include/unity)
target_link_libraries(it45-project-test unity)
