cmake_minimum_required(VERSION 3.31)

project(Aetherion)

set(CMAKE_CXX_STANDARD 20)

# Optional: Set architecture if needed, but usually set via command line
set(CMAKE_SYSTEM_PROCESSOR x86_64)

add_executable(aether src/Main.cpp)
