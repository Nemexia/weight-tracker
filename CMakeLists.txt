cmake_minimum_required(VERSION 3.15...4.0)

project(
  weight-tracker
  VERSION 0.3.0
  LANGUAGES CXX)

add_executable(main main.cpp core.cpp core.hpp)
