#! /bin/bash
#
#

IMGUI_DIR=./lib/imgui
LIB_DIR=./lib
c++ -std=c++11 -fPIC -shared -o ${LIB_DIR}/libimgui.dylib ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_widgets.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_demo.cpp -I${IMGUI_DIR}/ -I${IMGUI_DIR}/backends

# c++ -std=c++11 -fPIC -shared -o libcimgui.dylib cimgui.cpp -I./ -I/path/to/dear/imgui
