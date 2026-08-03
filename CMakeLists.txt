cmake_minimum_required(VERSION 3.20)
project(Hamboy)

set(CMAKE_CXX_STANDARD 20)

# Use vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

find_package(SDL2 CONFIG REQUIRED)

add_subdirectory(core)
add_subdirectory(desktop)