# This file defines PD_UTILS_LIBS,
# which has references to all the libraries included in this package.
# Use the command include() with this file as an argument to your CMakeLists.txt
# to gain access to those variables.

set(PD_UTILS_LIBS
        support
        pd_shorthand
        pd_scene_engine
        pd_text
        pd_image_utils
)

set(PD_UTILS_LIBS_SIM)

foreach(lib IN LISTS PD_UTILS_LIBS)
    set(sim_lib "${lib}_Sim")
    list(APPEND PD_UTILS_LIBS_SIM "${sim_lib}")
endforeach()
