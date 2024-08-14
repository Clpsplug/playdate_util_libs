cmake_minimum_required(VERSION 3.21)

include(${CMAKE_SOURCE_DIR}/cmake_support/Setup.cmake)

set(LIB_NAME pd_scene_engine)
set(LIB_NAME_SIM ${LIB_NAME}_Sim)

project(${LIB_NAME} C ASM)

set(SOURCES
        src/pd_scene.c
)

set(DEPENDENCIES pd_shorthand)
include(${CMAKE_SOURCE_DIR}/cmake_support/CompilationConf.cmake)
