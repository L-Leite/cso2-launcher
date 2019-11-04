cmake_minimum_required(VERSION 3.13.0)

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()

project(launcher LANGUAGES CXX)

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
  message(
    WARNING
      "Building the launcher for a different platform than 32 bit is not recommended."
    )
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
         "Use MSVC's shared runtime library instead of the static library?" ON)
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
generate_launcher_version(VERSION_FILE
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
  if(LAUNCHER_USE_SHARED_RUNTIME MATCHES ON)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
  elseif(LAUNCHER_USE_SHARED_RUNTIME MATCHES OFF)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  endif()
endif()

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
set(BUILD_DLL ON CACHE BOOL "")

# disable unused polyhook features
set(FEATURE_EXCEPTION OFF CACHE BOOL "")
set(FEATURE_INLINENTD OFF CACHE BOOL "")

add_subdirectory("${LAUNCHER_LIB_PHOOK_DIR}")

#
# add source files to the project
#
set(LAUNCHER_SOURCES_BASE
    "sources/console.cpp"
    "sources/consolethread.cpp"
    "sources/exports.cpp"
    "sources/filesystem_init.cpp"
    "sources/launcher.cpp"
    "sources/modulelist.cpp"
    "sources/sourceapp.cpp"
    "sources/tierextra.cpp")

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

set(LAUNCHER_SOURCES_APPFRAMEWORK "sources/appframework/AppSystemGroup.cpp"
                                  "sources/appframework/WinApp.cpp")

set(LAUNCHER_SOURCES_TIER1
    "sources/tier1/convar.cpp"
    "sources/tier1/generichash.cpp"
    "sources/tier1/interface.cpp"
    "sources/tier1/KeyValues.cpp"
    "sources/tier1/strtools.cpp"
    "sources/tier1/strtools_unicode.cpp"
    "sources/tier1/tier1.cpp"
    "sources/tier1/utlbuffer.cpp"
    "sources/tier1/utlstring.cpp")

set(LAUNCHER_SOURCES_TIER2 "sources/tier2/tier2.cpp")

set(LAUNCHER_SOURCES_TIER3 "sources/tier3/tier3.cpp")

set(LAUNCHER_HEADERS_IMGUIS
    "sources/imgui/imconfig.h"
    "sources/imgui/imgui.h"
    "sources/imgui/imgui_internal.h"
    "sources/imgui/imstb_rectpack.h"
    "sources/mgui/imstb_textedit.h"
    "sources/imgui/imstb_truetype.h"
    "sources/imgui/imgui_impl/imgui_impl_dx9.h"
    "sources/imgui/imgui_impl/imgui_impl_win32.h")

set(LAUNCHER_HEADERS_BASE
    "headers/basedir.hpp"
    "headers/console.hpp"
    "headers/hooks.hpp"
    "headers/ifilesystem.hpp"
    "headers/leakdump.hpp"
    "headers/modulelist.hpp"
    "headers/onloadlib.hpp"
    "headers/sourceapp.hpp"
    "headers/tierextra.hpp"
    "headers/utilities.hpp"
    ${LAUNCHER_VERSION_OUT})

if(WIN32)
  set(LAUNCHER_RESOURCES_WIN32 "resources/launcher.ico" "resources/launcher.rc"
                               "resources/resource.hpp")
else()
  set(LAUNCHER_RESOURCES_WIN32)
endif()

file(GLOB
     LAUNCHER_ALL_SOURCES
     ${LAUNCHER_SOURCES_BASE}
     ${LAUNCHER_SOURCES_HOOKS}
     ${LAUNCHER_SOURCES_IMGUIS}
     ${LAUNCHER_SOURCES_APPFRAMEWORK}
     ${LAUNCHER_SOURCES_TIER1}
     ${LAUNCHER_SOURCES_TIER2}
     ${LAUNCHER_SOURCES_TIER3}
     ${LAUNCHER_HEADERS_BASE}
     ${LAUNCHER_HEADERS_IMGUIS}
     ${LAUNCHER_RESOURCES_WIN32})

source_group("Source Files" FILES ${LAUNCHER_SOURCES_BASE})
source_group("Source Files\\AppFramework" FILES
             ${LAUNCHER_SOURCES_APPFRAMEWORK})
source_group("Source Files\\Hooks" FILES ${LAUNCHER_SOURCES_HOOKS})
source_group("Source Files\\ImGui" FILES ${LAUNCHER_SOURCES_IMGUIS})
source_group("Source Files\\Tier1" FILES ${LAUNCHER_SOURCES_TIER1})
source_group("Source Files\\Tier2" FILES ${LAUNCHER_SOURCES_TIER2})
source_group("Source Files\\Tier3" FILES ${LAUNCHER_SOURCES_TIER3})
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
  target_compile_definitions(launcher
                             PRIVATE
                             -DWIN32_LEAN_AND_MEAN
                             -DNOMINMAX)
endif()

# ensures compatibility with the game's std::string
target_compile_definitions(launcher PRIVATE -D_ITERATOR_DEBUG_LEVEL=0)

# source sdk specific
target_compile_definitions(launcher
                           PRIVATE
                           -DNO_VCR
                           -D_DLL_EXT=.dll)

#
# Enable all warnings
#
if(MSVC)
  target_compile_options(launcher PRIVATE /W4 /WX /SAFESEH:NO)
  target_link_options(launcher PRIVATE /SAFESEH:NO)
else()
  target_compile_options(launcher
                         PRIVATE -Wall
                                 -Wextra
                                 -Wconversion
                                 -pedantic)
  target_compile_options(launcher PRIVATE -g)
endif()

if(UNCSO2_USE_LTO)
  set_property(TARGET uc2 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  set_property(TARGET uncso2 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

#
# project's include directories
#
set(LAUNCHER_SOURCE_SDK_INCLUDES
    "${LAUNCHER_ROOT_DIR}/sdk/common"
    "${LAUNCHER_ROOT_DIR}/sdk/public"
    "${LAUNCHER_ROOT_DIR}/sdk/public/appframework"
    "${LAUNCHER_ROOT_DIR}/sdk/public/mathlib"
    "${LAUNCHER_ROOT_DIR}/sdk/public/tier0"
    "${LAUNCHER_ROOT_DIR}/sdk/public/tier1"
    "${LAUNCHER_ROOT_DIR}/sdk/public/vgui"
    "${LAUNCHER_ROOT_DIR}/sdk/public/vstdlib")

set(LAUNCHER_IMGUI_INCLUDES "sources/imgui" "sources/imgui/imgui_impl")

target_include_directories(launcher
                           PRIVATE "headers"
                                   ${LAUNCHER_SOURCE_SDK_INCLUDES}
                                   ${LAUNCHER_LIB_PHOOK_DIR}
                                   ${LAUNCHER_IMGUI_INCLUDES})

# the generated version header's directory
target_include_directories(launcher PRIVATE ${LAUNCHER_GENERATED_DIR})

#
# library dependencies
#
target_link_libraries(launcher capstone-static PolyHook_2)

# source sdk
target_link_directories(launcher PRIVATE "${LAUNCHER_ROOT_DIR}/sdk/lib/public")

# if(MSVC) target_link_libraries(launcher tier0_msvc.lib vstdlib.lib)
# elseif(MINGW) target_link_libraries(launcher tier0_mingw.lib vstdlib.lib)
# else() message(FATAL_ERROR "Please implement an option to link the Source SDK
# libraries in your compiler.") endif()
target_link_libraries(launcher tier0.lib vstdlib.lib)

# make polyhook's std::string compatible with the game bin
target_compile_definitions(PolyHook_2 PRIVATE _ITERATOR_DEBUG_LEVEL=0)

if(WIN32)
  target_link_libraries(launcher wsock32.lib)
endif()
