#!/usr/bin/env bash

set -e

if ! which cmake; then
  echo "You need cmake!"
  return 1
fi

builder=""

if which make; then
   echo "Using make to build"
   builder="make"
elif which ninja; then
    echo "Using ninja to build"
    builder="ninja"
fi

if [ -z $builder ]; then
    echo "make/ninja both missing! install either one (if You have both, make will be used)."
    return 1
fi 

function build() {
  case "$2" in
    "simulator")
      build_dir="build_sim";;
    "device")
      build_dir="build";;
    *)
      echo "Unknown build type";;
  esac
  
  echo "Building $1 for $2"
  
  mkdir -p "$1/$build_dir"
  pushd "$1/$build_dir" > /dev/null || exit 2
  if [ $2 == "device" ]; then
    cmake .. -DCMAKE_TOOLCHAIN_FILE="$PLAYDATE_SDK_PATH"/C_API/buildsupport/arm.cmake
  else
    cmake ..
  fi 
  "$builder"
  "$builder" install
  popd > /dev/null || exit 2
}

# Specify the order to get rid of dependency issue
build "shorthand" "device"
build "shorthand" "simulator"
build "scene_engine" "device"
build "scene_engine" "simulator"