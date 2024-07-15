cmake_minimum_required(VERSION 3.21)

# REQUIRES that a project() call has already been made.
if (NOT DEFINED CMAKE_PROJECT_NAME)
    message(FATAL_ERROR "No project call has been made yet. Call project() prior to including CompilationConf.cmake.")
endif ()


# REQUIRES that SOURCES variable is set.
if (NOT DEFINED SOURCES)
    message(FATAL_ERROR "No sources given, set SOURCES prior to including CompilationConf.cmake.")
endif ()

set(GENERAL_INCLUDE ${CMAKE_SOURCE_DIR}/include)
set(SELF_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB INC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)

if (TOOLCHAIN STREQUAL "armgcc")
    message(STATUS "Building ${LIB_NAME} for device.")
    add_library(${LIB_NAME} STATIC ${SOURCES})
    target_include_directories(${LIB_NAME} PUBLIC ${GENERAL_INCLUDE} ${SELF_INCLUDE} ${SHORTHAND_INCLUDE} ${SDK}/C_API)
    if (DEFINED DEPENDENCIES)
        target_link_libraries(${LIB_NAME} PRIVATE ${DEPENDENCIES})
    endif ()
    target_compile_options(${LIB_NAME} PRIVATE ${BASE_CXX_FLAGS} ${EXTRA_CXX_FLAGS})
    install(FILES ${INC_FILES} DESTINATION include PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
    install(TARGETS ${LIB_NAME} LIBRARY DESTINATION lib)
else ()
    message(STATUS "Building ${LIB_NAME} for simulator.")
    add_library(${LIB_NAME_SIM} STATIC ${SOURCES})
    target_include_directories(${LIB_NAME_SIM} PUBLIC ${GENERAL_INCLUDE} ${SELF_INCLUDE} ${SHORTHAND_INCLUDE} ${SDK}/C_API)
    if (DEFINED DEPENDENCIES)
        set(DEPENDENCIES_SIM)
        foreach (element IN LISTS DEPENDENCIES)
            set(modified_element "${element}_Sim")
            list(APPEND DEPENDENCIES_SIM "${modified_element}")
        endforeach ()
        target_link_libraries(${LIB_NAME_SIM} PRIVATE ${DEPENDENCIES_SIM})
    endif ()
    target_compile_options(${LIB_NAME_SIM} PRIVATE ${BASE_CXX_FLAGS} ${EXTRA_CXX_FLAGS})
    install(FILES ${INC_FILES} DESTINATION include PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
    install(TARGETS ${LIB_NAME_SIM} LIBRARY DESTINATION lib)
endif ()

