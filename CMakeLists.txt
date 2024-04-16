cmake_minimum_required(VERSION 3.25)
project(ControlloModellinoST C)

set(CMAKE_C_STANDARD 11)


add_executable(ControlloModellinoST
        src/main.c src/main.h src/processing_functions.c src/processing_functions.h src/simulation_function.c src/simulation_function.h src/micro_functions.c src/micro_functions.h)