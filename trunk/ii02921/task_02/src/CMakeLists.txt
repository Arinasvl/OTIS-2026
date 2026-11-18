cmake_minimum_required(VERSION 3.14)

# Библиотека моделей с уникальным именем
add_library(models_lib_ii02921 STATIC
    stability.cpp
)
target_include_directories(models_lib_ii02921 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Исполняемый файл программы
add_executable(task_02_ii02921 main.cpp)
target_link_libraries(task_02_ii02921 PRIVATE models_lib_ii02921)
set_target_properties(task_02_ii02921 PROPERTIES OUTPUT_NAME "task_02")