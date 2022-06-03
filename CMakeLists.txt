cmake_minimum_required(VERSION 3.12)

project(it45-project)

add_executable(it45-project
    src/main.c
    src/load.c
)

include_directories(it45-project src/)
