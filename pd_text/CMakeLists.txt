cmake_minimum_required(VERSION 3.21)
set(CMAKE_C_STANDARD 11)

set(ENVSDK $ENV{PLAYDATE_SDK_PATH})

if (NOT ${ENVSDK} STREQUAL "")
    # Conversion from Windows path
    file(TO_CMAKE_PATH ${ENVSDK} SDK)
else ()
    execute_process(
            COMMAND bash -c "egrep '^\\s*SDKRoot' $HOME/.Playdate/config"
            COMMAND head -n 1
            COMMAND cut -c9-
            OUTPUT_VARIABLE SDK
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif ()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_XCODE_GENERATE_SCHEME TRUE)

set(BASE_CXX_FLAGS -Wall -Werror -DTARGET_EXTENSION=1)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(EXTRA_CXX_FLAGS "-g")
else ()
    set(EXTRA_CXX_FLAGS "-O2")
endif ()

set(LIB_NAME pd_text)
set(LIB_NAME_SIM ${LIB_NAME}_Sim)

project(${LIB_NAME} C ASM)

set(SOURCES
        src/pd_text.c
)

set(SELF_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB INC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

if (TOOLCHAIN STREQUAL "armgcc")
    message(STATUS "Building ${LIB_NAME} for device.")
    add_library(${LIB_NAME} STATIC ${SOURCES})
    target_include_directories(${LIB_NAME} PUBLIC ${SELF_INCLUDE} ${SHORTHAND_INCLUDE} ${SDK}/C_API)
    target_compile_options(${LIB_NAME} PRIVATE ${BASE_CXX_FLAGS} ${EXTRA_CXX_FLAGS})
    install(FILES ${INC_FILES} DESTINATION include PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
    install(TARGETS ${LIB_NAME} LIBRARY DESTINATION lib)
else ()
    message(STATUS "Building ${LIB_NAME} for simulator.")
    add_library(${LIB_NAME_SIM} STATIC ${SOURCES})
    target_include_directories(${LIB_NAME_SIM} PUBLIC ${SELF_INCLUDE} ${SHORTHAND_INCLUDE} ${SDK}/C_API)
    target_compile_options(${LIB_NAME_SIM} PRIVATE ${BASE_CXX_FLAGS} ${EXTRA_CXX_FLAGS})
    install(FILES ${INC_FILES} DESTINATION include PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
    install(TARGETS ${LIB_NAME_SIM} LIBRARY DESTINATION lib)
endif ()

