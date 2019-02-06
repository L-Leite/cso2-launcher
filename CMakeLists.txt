cmake_minimum_required(VERSION 3.13.0)

project(launcher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "User did not specify build type, defaulting to Release build")
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DNDEBUG)
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(FATAL_ERROR "The launcher must be built for 32 bit platforms.")
endif()

set(LAUNCHER_REQUIRED_LIBRARIES capstone-static PolyHook_2)

# force PolyHook to build as a library
set(BUILD_DLL ON CACHE BOOL "")

# disable unused polyhook features
set(FEATURE_EXCEPTION OFF CACHE BOOL "")
set(FEATURE_INLINENTD OFF CACHE BOOL "")

add_subdirectory(libs/PolyHook_2_0)

add_subdirectory(launcher)
