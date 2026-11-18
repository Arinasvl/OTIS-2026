cmake_minimum_required(VERSION 3.14)
project(ii02921_task_03 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    DOWNLOAD_EXTRACT_TIMESTAMP true
)
FetchContent_MakeAvailable(googletest)

add_executable(ii02921_task_03
    main.cpp
    simulation.cpp
    pid_controller.cpp
)

target_include_directories(ii02921_task_03 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})