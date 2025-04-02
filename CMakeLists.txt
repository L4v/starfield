cmake_minimum_required(VERSION 3.10)
project(starfield C)

set(CMAKE_C_STANDARD 11)

message(STATUS "CImgui lib location: ${PATH_TO_CIMGUI_SHARED_LIB}")
message(STATUS "CImgui header location: ${PATH_TO_CIMGUI_HEADERS}")
# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SOURCES
   "src/*.c"
)

# Add glad
add_library(glad STATIC
   glad/src/glad.c
)
target_include_directories(glad PRIVATE
   glad/include
)

# Add cimgui
add_library(cimgui SHARED IMPORTED)
set_target_properties(cimgui PROPERTIES
   IMPORTED_LOCATION "${PATH_TO_CIMGUI_SHARED_LIB}"
   INTERFACE_INCLUDE_DIRECTORIES "${PATH_TO_CIMGUI_HEADERS}"
)

# Add main executable
add_executable(starfield ${SOURCES})
target_compile_features(starfield PRIVATE c_std_11)
target_include_directories(starfield PRIVATE
   ${PATH_TO_CIMGUI_HEADERS}
   glad/include
   include
   src
)

# Link libraries
target_link_libraries(starfield PRIVATE
   OpenGL::GL
   glfw
   glad
   cimgui
)

# Copy cimgui shared library to output directory
add_custom_command(TARGET starfield POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_if_different
   "${PATH_TO_CIMGUI_SHARED_LIB}"
   $<TARGET_FILE_DIR:starfield>
)
