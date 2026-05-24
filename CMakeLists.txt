cmake_minimum_required(VERSION 3.14)
project(CppProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    raylib
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
FetchContent_MakeAvailable(raylib)

add_library(core_lib STATIC
    2048/2048.cpp
    NN/ai_tools.cpp
    wordle/wordle.cpp
    backrooms/backrooms.cpp
)

target_link_libraries(core_lib PRIVATE raylib)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE core_lib raylib)
