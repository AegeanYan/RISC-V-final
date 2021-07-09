cmake_minimum_required(VERSION 3.14)
project(code)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(code
        ALU.cpp
        ALU.h
        main.cpp
        Memory.cpp
        Memory.h
        Registor.cpp
        Registor.h
        simulator.cpp
        simulator.h)
