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
