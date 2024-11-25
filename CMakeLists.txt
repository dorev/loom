cmake_minimum_required(VERSION 3.14)

project(LoomSolution)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)

add_subdirectory(loom)
add_subdirectory(tests)

set(ENABLE_EXTERNAL_LIBS OFF CACHE BOOL "Disable external format libraries")
set(ENABLE_MPEG OFF CACHE BOOL "Disable external format libraries")
set(ENABLE_CPACK OFF CACHE BOOL "Disable external format libraries")
set(BUILD_TESTING OFF CACHE BOOL "Disable libsndfile tests")
set(BUILD_EXAMPLES OFF CACHE BOOL "Disable external format libraries")
set(BUILD_PROGRAMS OFF CACHE BOOL "Disable external format libraries")
set(ENABLE_WAV ON CACHE BOOL "Enable WAV format")
set(ENABLE_AIFF ON CACHE BOOL "Enable AIFF format")
set(ENABLE_FLAC OFF CACHE BOOL "Disable FLAC format")
set(ENABLE_OGG OFF CACHE BOOL "Disable OGG format")
add_subdirectory(${EXTERNAL_DIR}/libsndfile)