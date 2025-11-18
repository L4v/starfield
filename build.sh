#! /bin/bash
#

PROJECT_ROOT=$(pwd)

mkdir -p build

pushd ./build

# cmake \
#   -DPATH_TO_CIMGUI_SHARED_LIB=$PATH_TO_CIMGUI_SHARED_LIB\
#   -DPATH_TO_CIMGUI_HEADERS=$PATH_TO_CIMGUI_HEADERS\
#   ..
cmake \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=1\
  ..
cmake --build .
mv ./compile_commands.json ../
popd

