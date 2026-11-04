cmake_minimum_required(VERSION 3.10)
<<<<<<< HEAD

project(SimulationModels)
=======
project(ii02921_task_01_src)
>>>>>>> 658d4fd00ddbb2d7412c1b0e831376e7a4d7c484

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

<<<<<<< HEAD
add_executable(SimulationModels
    labaa.cpp
    labaa.h
)
=======
# Автоматически находим все исходные файлы моделей Варианта 23 и сигнала
file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.h")

# Вычисляем уникальное имя таргета для предотвращения коллизий в общем репозитории (например, ii02921_task_01)
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
get_filename_component(TASK_DIR ${PARENT_DIR} NAME)
get_filename_component(GRAND_DIR ${PARENT_DIR} DIRECTORY)
get_filename_component(STUDENT_DIR ${GRAND_DIR} NAME)
set(TARGET_NAME "${STUDENT_DIR}_${TASK_DIR}")

# Создаем исполняемый файл из всех найденных .cpp и .h файлов
add_executable(${TARGET_NAME} ${SOURCES} ${HEADERS})

# Задаем бинарному файлу стандартное выходное имя task_01(.exe)
set_target_properties(${TARGET_NAME} PROPERTIES 
    OUTPUT_NAME "task_01"
)
>>>>>>> 658d4fd00ddbb2d7412c1b0e831376e7a4d7c484
