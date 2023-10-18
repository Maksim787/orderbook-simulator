cmake_minimum_required(VERSION 3.12)
project(orderbook-lib)

set(CMAKE_CXX_STANDARD 17)

# Library
file(GLOB_RECURSE ORDERBOOK_LIBRARY_SOURCES "src/*.cpp")
add_library(orderbook-lib ${ORDERBOOK_LIBRARY_SOURCES})
target_include_directories(orderbook-lib PUBLIC include/)

# Executables
add_subdirectory(exe)

# TODO: add tests