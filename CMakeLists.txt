cmake_minimum_required(VERSION 3.20)

project(kaleidoscope LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(kaleidoscope
    src/main.cpp
    src/lexer.cpp
)

target_include_directories(kaleidoscope PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(kaleidoscope PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)
