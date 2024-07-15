#!/usr/bin/env bash

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

DEBUG_MODE=0
if [ $1 == "-d" ]; then
    DEBUG_MODE=1
fi

function build_and_install()
{
  echo "-- Building for $2 --"
  pushd "${SCRIPT_DIR:?}/$1" > /dev/null || ( echo "pushd to ${SCRIPT_DIR:?}/$1 failed"; exit 1 )
    if [ $DEBUG_MODE -eq 1 ]; then
      DEBUG_FLAG="-DCMAKE_BUILD_TYPE=Debug"
    fi
    case "$2" in
      "device")
      cmake .. "${DEBUG_FLAG:-}" -DCMAKE_TOOLCHAIN_FILE="$PLAYDATE_SDK_PATH/C_API/buildsupport/arm.cmake"
      ;;
      "sim")
      cmake .. "${DEBUG_FLAG:-}"
      ;;
      *)
        echo "Unknown build target $2";
        exit 1;
        ;;
    esac
    cmake --build . --target clean
    cmake --build . --target build_all
    cmake --build . --target install_all
  popd > /dev/null || ( echo "popd failed"; exit 1 )
}

if [ -z "$SCRIPT_DIR" ]; then
  echo "Could not get script directory... this is odd."
  exit 1
fi

if [ -d "$SCRIPT_DIR/include" ]; then
  /bin/rm -r "${SCRIPT_DIR:?}/include"
fi

if [ -d "$SCRIPT_DIR/lib" ]; then
  /bin/rm -r "${SCRIPT_DIR:?}/lib"
fi

if [ -d "$SCRIPT_DIR/build" ]; then
  /bin/rm -r "${SCRIPT_DIR:?}/build"
fi

if [ -d "$SCRIPT_DIR/build_sim" ]; then
  /bin/rm -r "${SCRIPT_DIR:?}/build_sim"
fi

mkdir -p "${SCRIPT_DIR:?}/build"
mkdir -p "${SCRIPT_DIR:?}/build_sim"

build_and_install build device
build_and_install build_sim sim
