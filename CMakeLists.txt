
cmake_minimum_required(VERSION 3.15)
project(BLAKE3 VERSION 1.8.3 LANGUAGES CXX)


# Add the library
add_library(BLAKE3 STATIC
  src/BLAKE3.cpp
)

# Add include directories for consumers
target_include_directories(Logger
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:src>
)
