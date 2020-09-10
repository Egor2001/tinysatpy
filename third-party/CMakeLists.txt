cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

include(ExternalProject)

ExternalProject_Add(tinysat_git 
    GIT_REPOSITORY https://github.com/Egor2001/tinySAT.git
    GIT_TAG develop
    CMAKE_ARGS 
    -DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_SOURCE_DIR}
    -DCMAKE_BUILD_TYPE=Release
    )
