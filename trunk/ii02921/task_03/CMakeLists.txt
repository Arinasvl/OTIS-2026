cmake_minimum_required(VERSION 3.14)
project(task_03 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==========================================
# 1. ОСНОВНАЯ ПРОГРАММА
# ==========================================
add_executable(task_03
    src/main.cpp
    src/pid_controller.cpp
    src/simulation.cpp
)

# ==========================================
# 2. ПОДКЛЮЧЕНИЕ GOOGLE TEST ИЗ ЛОКАЛЬНОЙ ПАПКИ
# ==========================================
# 🔑 КЛЮЧЕВАЯ СТРОКА: заставляем Google Test использовать 
# ту же CRT (динамическую), что и наш проект
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(third_party/googletest)

# ==========================================
# 3. НАСТРОЙКА ТЕСТОВ
# ==========================================
enable_testing()

add_executable(task_03_tests
    test/test_pid.cpp
    test/test_models.cpp
    test/test_simulation.cpp
)

target_link_libraries(task_03_tests PRIVATE 
    gtest 
    gtest_main
)

target_include_directories(task_03_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
)

target_sources(task_03_tests PRIVATE
    src/pid_controller.cpp
    src/simulation.cpp
)

add_test(NAME Task03Tests COMMAND task_03_tests)