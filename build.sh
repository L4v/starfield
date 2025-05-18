#! /bin/bash
#

PROJECT_ROOT=$(pwd)
# PATH_TO_CIMGUI_SHARED_LIB="${PROJECT_ROOT}/lib/cimgui.dylib" # .dll on Windows
# PATH_TO_CIMGUI_SHARED_LIB="${PROJECT_ROOT}/lib/cimgui.so" # .so on Linux
# PATH_TO_CIMGUI_HEADERS="${PROJECT_ROOT}/lib/cimgui/"
# echo PATH $PATH_TO_CIMGUI_SHARED_LIB 
# echo PATH $PATH_TO_CIMGUI_HEADERS

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

