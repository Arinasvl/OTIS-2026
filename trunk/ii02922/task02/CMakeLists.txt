
cmake_minimum_required(VERSION 3.20)

project(task_02 LANGUAGES CXX)

# Стандарт C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Подключение локальной библиотеки Google Test
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(third_party/googletest)

# Основная программа
add_executable(task_02
    src/main.cpp
    src/stability.cpp
)

target_include_directories(task_02 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Библиотека логики для тестирования
add_library(task_02_core STATIC
    src/stability.cpp
)

target_include_directories(task_02_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Модульные тесты
enable_testing()

add_executable(task_02_tests
    test/test_stability.cpp
)

target_include_directories(task_02_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(task_02_tests PRIVATE
    task_02_core
    GTest::gtest_main
    GTest::gmock
)

include(GoogleTest)

gtest_discover_tests(task_02_tests)
