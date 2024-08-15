cmake_minimum_required(VERSION 3.21)

# Set the project name
project(playdate_util_libs)

# Define the root project directory
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Specify the installation prefix to the root directory
set(CMAKE_INSTALL_PREFIX ${ROOT_DIR} CACHE PATH "Installation dir" FORCE)

# List of subprojects
set(SUBPROJECTS
        support
        pd_shorthand
        pd_text
        pd_image_utils
        pd_scene_engine
)

# Loop through each subproject and add subdirectory
foreach (SUBPROJECT ${SUBPROJECTS})
    add_subdirectory(${SUBPROJECT})
endforeach ()

# Collect target names from subprojects
set(TARGET_NAMES)
foreach (SUBPROJECT ${SUBPROJECTS})
    if (TOOLCHAIN STREQUAL "armgcc")
        list(APPEND TARGET_NAMES ${SUBPROJECT})
    else ()
        list(APPEND TARGET_NAMES ${SUBPROJECT}_Sim)
    endif ()
endforeach ()

# Custom target to build all subprojects
add_custom_target(build_all)

# Add dependencies on the default targets of each subproject
foreach (TARGET ${TARGET_NAMES})
    add_dependencies(build_all ${TARGET})
endforeach ()

# Custom target to install all subprojects
add_custom_target(install_all
        COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix ${CMAKE_INSTALL_PREFIX}
        DEPENDS build_all
)
