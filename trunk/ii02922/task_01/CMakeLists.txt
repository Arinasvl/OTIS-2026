cmake_minimum_required(VERSION 3.10)
<<<<<<< HEAD

project(SimulationModels)
=======
project(Variant23_Root)
>>>>>>> 658d4fd00ddbb2d7412c1b0e831376e7a4d7c484

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

<<<<<<< HEAD
add_executable(SimulationModels
    src/labaa.cpp
)
=======
# Указываем CMake зайти в папку src и взять сборку оттуда
add_subdirectory(src)
>>>>>>> 658d4fd00ddbb2d7412c1b0e831376e7a4d7c484
