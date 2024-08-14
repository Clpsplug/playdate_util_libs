# Utility library bundle for Playdate

## What's included

* [Shorthand library](pd_shorthand/README.md)
* [Text library](pd_text/README.md)
* [Scene engine](scene_engine/README.md)

## How to build / use

### On macOS/Linux

Run `install.sh` to get libraries for both device and simulator.

### On Windows

Untested, but you should be able to use cygwin to run the above.

TODO: Add 'Windows' way to get the project running.

### Documentation

After building (see previous section), run `doxygen` at the root of this repository.

### To use (include header, link libraries) the lib to the game

Add the following lines to your `CMakeLists.txt`, or adjust them if you already have those.

```cmake
# Having this repo as a submodule might help.
set(UTIL_LIBS_PATH /path/to/playdate_util_libs)
# Then include this cmake file to bulk-link the library.
include(support.cmake)
if (TOOLCHAIN STREQUAL "armgcc")
    # Playdate build
    add_executable(MyPlaydateGame main.c) # you may have more than main.c
    # other stuff...
    target_include_directories(MyPlaydateGame PRIVATE ${UTIL_LIBS_PATH}/include)
    target_link_directories(MyPlaydateGame PRIVATE ${UTIL_LIBS_PATH}/lib)
    target_link_libraries(MyPlaydateGame ${PD_UTILS_LIBS})
    # more stuff...
else ()
    # Simulator build
    add_library(MyPlaydateSimGame main.c) # you may have more than main.c
    # other stuff...
    target_include_directories(MyPlaydateSimGame PRIVATE ${UTIL_LIBS_PATH}/include)
    target_link_directories(MyPlaydateSimGame PRIVATE ${UTIL_LIBS_PATH}/lib)
    # Append "_Sim" for simulator build.
    target_link_libraries(MyPlaydateSimGame ${PD_UTILS_LIBS_SIM})
    # more stuff...
endif ()
```

### To access the features of this library

The easiest way is to include `pd_utils.h`.
Call pdUtil_InitializeAll(void*) to initialize the library
and pdUtil_FinalizeAll(void) to perform cleanup
at `kEventInit` and `kEventTerminate` events, respectively.

```c
#include <pd_utils.h>

int eventHandler(PlaydateAPI *pd, PDSystemEvent eventType, uint32_t arg) {
    switch (eventType)
    {
        case kEventInit:
            pdUtils_InitializeAll(pd);
            break;
        case kEventTerminate:
            pdUtils_FinalizeAll();
            break;
    }
}
```

## License

MIT
