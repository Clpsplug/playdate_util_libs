# This file defines PD_UTILS_LIBS,
# which has references to all the libraries included in this package.
set(PD_UTILS_LIBS
        pd_shorthand
        pd_scene_engine
        pd_text
)

set(PD_UTILS_LIBS_SIM)

foreach(lib IN LISTS PD_UTILS_LIBS)
    set(sim_lib "${lib}_Sim")
    list(APPEND PD_UTILS_LIBS_SIM "${sim_lib}")
endforeach()
