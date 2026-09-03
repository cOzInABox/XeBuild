cmake_minimum_required(VERSION 3.14...4.4.2)
project(XeBuild LANGUAGES C)

set(XEBUILD_VERSION_MAJOR 1)
set(XEBUILD_VERSION_MINOR 22)
set(XEBUILD_VERSION_BASE "${XEBUILD_VERSION_MAJOR}.${XEBUILD_VERSION_MINOR}")

message(STATUS "XeBuild version: ${XEBUILD_VERSION_BASE}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

message("CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

message(STATUS "C flags, Debug configuration: ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS "C flags, Release configuration: ${CMAKE_C_FLAGS_RELEASE}")


if (WIN32)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

if (MSVC)
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:/utf-8>")
    add_compile_options("$<$<COMPILE_LANGUAGE:C>:/bigobj>")
endif()

set(BUILD_SHARED_LIBS NO)

#
# build the library
#

add_subdirectory(XeCrypt)

#
# build the exe
#

add_subdirectory(source)



