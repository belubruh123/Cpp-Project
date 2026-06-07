cmake_minimum_required(VERSION 3.14)
project(CppProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "Silence CMake 4.3 warning about raylib <3.5" FORCE)
set(CMAKE_TLS_VERIFY OFF)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

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

if(MINGW)
    target_link_options(app PRIVATE -static)
endif()

# Copy wordlist files to the build directory so they are found when running from ./build
file(COPY wordle/validGuesses.txt wordle/answers.txt DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/wordle)
