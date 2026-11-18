cmake_minimum_required(VERSION 3.10)

project(SimulationModels)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SimulationModels_ii02921
    main.cpp
    InputSignal.cpp
    Model1_6.cpp
    Model1_6.h
    Model2_1.cpp
    Model2_1.h
    Model3_7.cpp
    Model3_7.h
    labaa.h
)

set_target_properties(SimulationModels_ii02921 PROPERTIES
    OUTPUT_NAME "SimulationModels"
)