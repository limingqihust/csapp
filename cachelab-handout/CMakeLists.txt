cmake_minimum_required(VERSION 3.24)
project(cachelab_handout C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(cachelab_handout
        cachelab.c
        cachelab.h
        csim.c
        test-trans.c
        tracegen.c
        trans.c)
