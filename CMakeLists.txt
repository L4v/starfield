cmake_minimum_required(VERSION 3.10)
project(starfield C)

set(CMAKE_C_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SOURCES
   "src/*.c"
)
list(FILTER SOURCES EXCLUDE REGEX "src\/main[^\.]*\.c")

add_library(glad STATIC
   glad/src/glad.c
)
target_include_directories(glad PRIVATE
   glad/include
)

# SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-instr-generate")

add_executable(minecraft ${SOURCES} "src/main.c")

target_compile_features(minecraft PRIVATE c_std_11)
target_include_directories(minecraft PRIVATE
   glad/include
   include
   src
)

# Link libraries
target_link_libraries(minecraft PRIVATE
   OpenGL::GL
   glfw
   glad
)
