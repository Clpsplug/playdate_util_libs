cmake_minimum_required(VERSION 3.21)

include(${CMAKE_SOURCE_DIR}/cmake_support/Setup.cmake)

set(LIB_NAME support)
set(LIB_NAME_SIM ${LIB_NAME}_Sim)

project(${LIB_NAME} C ASM)

set(SOURCES
    src/pd_utils.c
)

set(DEPENDENCIES
        pd_shorthand
        pd_text
        pd_scene_engine
)
include(${CMAKE_SOURCE_DIR}/cmake_support/CompilationConf.cmake)
