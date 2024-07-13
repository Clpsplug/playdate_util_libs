cmake_minimum_required(VERSION 3.21)

project(playdate_util_libs)

add_custom_target(build_util_libs
        COMMAND ${CMAKE_COMMAND} -E env "bash" "${CMAKE_SOURCE_DIR}/install.sh"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Runs install.sh, which creates includes and libs folder here."
)