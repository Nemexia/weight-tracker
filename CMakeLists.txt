cmake_minimum_required(VERSION 3.15...4.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

project(
  weight-tracker
  VERSION 0.4.0
  DESCRIPTION "Tracks my weight"
  LANGUAGES CXX)

if(MSVC)
  add_compile_options(/W4 /permissive-)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(plotter STATIC src/plotter.cpp include/plotter.hpp)
target_include_directories(plotter PUBLIC include)

add_executable(main src/main.cpp src/core.cpp include/core.hpp)
target_include_directories(main PRIVATE include)
target_link_libraries(main plotter)
