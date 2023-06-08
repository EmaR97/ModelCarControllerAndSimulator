cmake_minimum_required(VERSION 3.25)
project(ControlloModellinoST C)

set(CMAKE_C_STANDARD 11)


add_executable(ControlloModellinoST
        main.c processing_functions.c simulation_function.c micro_functions.c micro_functions.h)