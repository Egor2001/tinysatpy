cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(tinysatpy VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

set(SOURCES src/TspModule.cpp src/TspCommon.cpp src/TspSolver.cpp)

# TODO: to replace with FindPython
include_directories(/usr/include/python3.8 src/)

add_library(tinysatpy SHARED ${SOURCES})

add_subdirectory(tinySAT)

target_include_directories(tinysatpy PUBLIC tinySAT)
target_link_libraries(tinysatpy dpll)

# Is replaced with git submodule to simplify maintenance
# add_subdirectory(third-party)
# include_directories(third-party)
