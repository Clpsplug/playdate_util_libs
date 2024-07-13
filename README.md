# Utility library bundle for Playdate

## What's included

* Shorthand library
* [Scene engine](scene_engine/README.md)

## How to build / use

### On macOS/Linux

Run `install.sh` to get libraries for both device and simulator.

### On Windows

Untested, but you should be able to use cygwin to run the above.

TODO: Add 'Windows' way to get the project running.

### To use (include header, link libraries) the lib to the game

Add the following lines to your `CMakeLists.txt`, or adjust them if you already have those.

```cmake
# Having this repo as a submodule might help.
set(UTIL_LIBS_PATH /path/to/playdate_util_libs)
if (TOOLCHAIN STREQUAL "armgcc")
    # Playdate build
    add_executable(MyPlaydateGame main.c) # you may have more than main.c
    # other stuff...
    target_include_directories(MyPlaydateGame PRIVATE ${UTIL_LIBS_PATH}/include)
    target_link_directories(MyPlaydateGame PRIVATE ${UTIL_LIBS_PATH}/lib)
    target_link_libraries(MyPlaydateGame pd_shorthand scene_engine)
    # more stuff...
else ()
    # Simulator build
    add_library(MyPlaydateSimGame main.c) # you may have more than main.c
    # other stuff...
    target_include_directories(MyPlaydateSimGame PRIVATE ${UTIL_LIBS_PATH}/include)
    target_link_directories(MyPlaydateSimGame PRIVATE ${UTIL_LIBS_PATH}/lib)
    # Append "_Sim" for simulator build.
    target_link_libraries(MyPlaydateSimGame pd_shorthand_Sim scene_engine_Sim)
    # more stuff...
endif ()
```

## License

MIT