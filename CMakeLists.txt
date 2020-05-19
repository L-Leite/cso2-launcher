cmake_minimum_required(VERSION 3.13.0)

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

project(launcher LANGUAGES CXX)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(WARNING "The launcher must be build for a 32 bit platform.")
endif()

#
# Put Visual Studio resulting binaries out of their config directory
#
if(MSVC)
  set(PKG_POSSIBLE_CONFIGS "Debug;Release;MinSizeRel;RelWithDebInfo")
  foreach(CUR_CONFIG ${PKG_POSSIBLE_CONFIGS})
    string(TOUPPER ${CUR_CONFIG} CUR_CONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CUR_CONFIG} "${OUTPUT_DIRECTORY}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CUR_CONFIG} "${OUTPUT_DIRECTORY}")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CUR_CONFIG} "${OUTPUT_DIRECTORY}")
  endforeach()
endif()

option(LAUNCHER_USE_LTO "Use 'Link Time Optimizations'" ON)
if(MSVC)
  option(LAUNCHER_USE_SHARED_RUNTIME
         "Use MSVC's shared runtime library instead of the static library?" OFF)
endif()

set(LAUNCHER_ROOT_DIR "${PROJECT_SOURCE_DIR}")
set(LAUNCHER_GENERATED_DIR "${CMAKE_BINARY_DIR}/generated")
set(LAUNCHER_LIB_PHOOK_DIR "${LAUNCHER_ROOT_DIR}/libs/PolyHook_2_0")

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

set(LAUNCHER_VERSION_FILE "${LAUNCHER_ROOT_DIR}/version.txt")
set(LAUNCHER_VERSION_TEMPLATE
    "${CMAKE_SOURCE_DIR}/headers/launcher_version.hpp.in")
set(LAUNCHER_VERSION_OUT "${LAUNCHER_GENERATED_DIR}/launcher_version.hpp")
set(LAUNCHER_GIT_DIR "${CMAKE_SOURCE_DIR}/.git")

include(launcher_version)
generate_launcher_version(
  VERSION_FILE
  ${LAUNCHER_VERSION_FILE}
  HEADER_TEMPLATE
  ${LAUNCHER_VERSION_TEMPLATE}
  HEADER_OUT
  ${LAUNCHER_VERSION_OUT}
  GIT_DIR
  ${LAUNCHER_GIT_DIR})

#
# setup MSVC runtime lib
#
if(MSVC)
  # force static lib
  set(CompilerFlags
      CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
      CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE)
  foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  endforeach()
endif()

message(STATUS "CMAKE_MSVC_RUNTIME_LIBRARY: ${CMAKE_MSVC_RUNTIME_LIBRARY}")

#
# support large addresses (>2gb)
#
if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
elseif(MINGW)
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} -Wl,--large-address-aware")
else()
  message(
    FATAL_ERROR
      "Please implement the large address aware option for your compiler.")
endif()

#
# Dependencies
#

# PolyHook

# force PolyHook to build as a library
set(BUILD_DLL
    ON
    CACHE BOOL "")

# disable unused polyhook features
set(FEATURE_EXCEPTION
    OFF
    CACHE BOOL "")
set(FEATURE_INLINENTD
    OFF
    CACHE BOOL "")

add_subdirectory("${LAUNCHER_LIB_PHOOK_DIR}")

#
# add source files to the project
#
set(LAUNCHER_SOURCES_BASE
    "sources/platform/platform.cpp"
    "sources/source/appsystemgroup.cpp"
    "sources/source/appsystemtable.cpp"
    "sources/source/filesystem_util.cpp"
    "sources/source/sourcesystem.cpp"
    "sources/source/tierlibs.cpp"
    "sources/console.cpp"
    "sources/launcher.cpp"
    "sources/main.cpp"
    "sources/modulecache.cpp"
    "sources/sourcemain.cpp")

if(WIN32)
  set(LAUNCHER_SOURCES_BASE "sources/platform/platform_windows.cpp"
                            ${LAUNCHER_SOURCES_BASE})
elseif(UNIX)
  set(LAUNCHER_SOURCES_BASE "sources/platform/platform_unix.cpp"
                            ${LAUNCHER_SOURCES_BASE})
endif()

set(LAUNCHER_SOURCES_HOOKS
    "sources/hooks/client.cpp"
    "sources/hooks/engine.cpp"
    "sources/hooks/filesystem.cpp"
    "sources/hooks/shaderapidx9.cpp"
    "sources/hooks/tier0.cpp"
    "sources/hooks/vgui2.cpp"
    "sources/hooks/winapi.cpp")

set(LAUNCHER_SOURCES_IMGUIS
    "sources/imgui/imgui.cpp"
    "sources/imgui/imgui_draw.cpp"
    "sources/imgui/imgui_widgets.cpp"
    "sources/imgui/Consolas_Unicode.cpp"
    "sources/imgui/imgui_impl/imgui_impl_dx9.cpp"
    "sources/imgui/imgui_impl/imgui_impl_win32.cpp")

set(LAUNCHER_HEADERS_IMGUIS
    "sources/imgui/imconfig.h"
    "sources/imgui/imgui.h"
    "sources/imgui/imgui_internal.h"
    "sources/imgui/imstb_rectpack.h"
    "sources/imgui/imstb_textedit.h"
    "sources/imgui/imstb_truetype.h"
    "sources/imgui/imgui_impl/imgui_impl_dx9.h"
    "sources/imgui/imgui_impl/imgui_impl_win32.h")

set(LAUNCHER_HEADERS_BASE
    "headers/source/appsystemgroup.hpp"
    "headers/source/appsystemtable.hpp"
    "headers/source/filesystem_util.hpp"
    "headers/source/sourcesystem.hpp"
    "headers/source/tierlibs.hpp"
    "headers/console.hpp"
    "headers/hooks.hpp"
    "headers/leakdump.hpp"
    "headers/modulecache.hpp"
    "headers/platform.hpp"
    "headers/utilities.hpp"
    ${LAUNCHER_VERSION_OUT})

if(WIN32)
  set(LAUNCHER_RESOURCES_WIN32 "resources/launcher.ico" "resources/launcher.rc")
else()
  set(LAUNCHER_RESOURCES_WIN32)
endif()

file(
  GLOB
  LAUNCHER_ALL_SOURCES
  ${LAUNCHER_SOURCES_BASE}
  ${LAUNCHER_SOURCES_HOOKS}
  ${LAUNCHER_SOURCES_IMGUIS}
  ${LAUNCHER_HEADERS_BASE}
  ${LAUNCHER_HEADERS_IMGUIS}
  ${LAUNCHER_RESOURCES_WIN32})

source_group("Source Files" FILES ${LAUNCHER_SOURCES_BASE})
source_group("Source Files\\Hooks" FILES ${LAUNCHER_SOURCES_HOOKS})
source_group("Source Files\\ImGui" FILES ${LAUNCHER_SOURCES_IMGUIS})
source_group("Header Files" FILES ${LAUNCHER_HEADERS_BASE})
source_group("Header Files\\ImGui" FILES ${LAUNCHER_HEADERS_IMGUIS})
source_group("Resource Files" FILES ${LAUNCHER_RESOURCES_BASE})

# force c++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#
# Add executable to build.
#
if(WIN32)
  add_executable(launcher WIN32 ${LAUNCHER_ALL_SOURCES})
else()
  add_executable(launcher WIN32 ${LAUNCHER_ALL_SOURCES})
endif()

#
# macros
#
if(WIN32)
  target_compile_definitions(launcher PRIVATE -DWIN32_LEAN_AND_MEAN)
endif()

# ensures compatibility with the game's std::string
target_compile_definitions(launcher PRIVATE -D_ITERATOR_DEBUG_LEVEL=0)

#
# Enable all warnings
#
if(MSVC)
  target_compile_options(launcher PRIVATE /W4 /EHs)
else()
  target_compile_options(launcher PRIVATE -Wall -Wextra -Wconversion -pedantic)
  target_compile_options(launcher PRIVATE -g)
endif()

if(LAUNCHER_USE_LTO)
  set_property(TARGET launcher PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

#
# project's include directories
#
set(LAUNCHER_SOURCE_SDK_INCLUDES "${LAUNCHER_ROOT_DIR}/minimal_sdk/public")

set(LAUNCHER_IMGUI_INCLUDES "sources/imgui" "sources/imgui/imgui_impl")

target_include_directories(
  launcher PRIVATE "headers" ${LAUNCHER_SOURCE_SDK_INCLUDES}
                   ${LAUNCHER_LIB_PHOOK_DIR} ${LAUNCHER_IMGUI_INCLUDES})

# the generated version header's directory
target_include_directories(launcher PRIVATE ${LAUNCHER_GENERATED_DIR})

#
# library dependencies
#
target_link_libraries(launcher capstone-static PolyHook_2)

# make polyhook's std::string compatible with the game bin
target_compile_definitions(PolyHook_2 PRIVATE _ITERATOR_DEBUG_LEVEL=0)

if(WIN32)
  target_link_libraries(launcher wsock32.lib)
endif()
