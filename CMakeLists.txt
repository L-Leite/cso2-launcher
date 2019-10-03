cmake_minimum_required(VERSION 3.13.0)

project(launcher LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake_modules")

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

include(cotire)

# Generate version.h at build time.
add_custom_target(LauncherVersion ALL
                  COMMAND ${CMAKE_COMMAND}
                          -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                          -P
                          ${PROJECT_SOURCE_DIR}/cmake_modules/version.cmake)

# Generate git-version.h at build time.
add_custom_target(GitVersion ALL
                  COMMAND ${CMAKE_COMMAND}
                          -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                          -P
                          ${PROJECT_SOURCE_DIR}/cmake_modules/git-version.cmake)

#
# project's include directories
#
set(LAUNCHER_PROJECT_INCLUDES "headers")

set(LAUNCHER_SOURCE_SDK_INCLUDES
    ${PROJECT_SOURCE_DIR}/sdk/common
    ${PROJECT_SOURCE_DIR}/sdk/public
    ${PROJECT_SOURCE_DIR}/sdk/public/appframework
    ${PROJECT_SOURCE_DIR}/sdk/public/mathlib
    ${PROJECT_SOURCE_DIR}/sdk/public/tier0
    ${PROJECT_SOURCE_DIR}/sdk/public/tier1
    ${PROJECT_SOURCE_DIR}/sdk/public/vgui
    ${PROJECT_SOURCE_DIR}/sdk/public/vstdlib)

set(LAUNCHER_LIBRARIES_INCLUDES ${PROJECT_SOURCE_DIR}/libs/PolyHook_2_0)

set(LAUNCHER_IMGUI_INCLUDES "sources/imgui" "sources/imgui/imgui_impl")

set(LAUNCHER_ALL_INCLUDES
    ${LAUNCHER_PROJECT_INCLUDES}
    ${LAUNCHER_SOURCE_SDK_INCLUDES}
    ${LAUNCHER_LIBRARIES_INCLUDES}
    ${LAUNCHER_IMGUI_INCLUDES})

include_directories(${LAUNCHER_ALL_INCLUDES})

#
# macros
#
if(WIN32)
  add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

# ensures compatibility with the game's std::string
add_definitions(-D_ITERATOR_DEBUG_LEVEL=0)

# source sdk specific
add_definitions(-DNO_VCR -D_DLL_EXT=.dll)

#
# extra compiler flags warning level 4, exception handling, multi threaded
# runtime lib also enables whole program optimization for release builds
#
if(MSVC)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W4 /EHsc /MTd")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /W4 /EHsc /MT")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO
      "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /W4 /EHsc /MT /GL")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W4 /EHsc /MT /GL")
endif()

#
# extra linker flags make app support large addresses (>2gb) and enable whole
# program optimization for release builds
#
if(MSVC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /LARGEADDRESSAWARE")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
  set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO
      "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /LTCG")
endif()

#
# add source files to the project
#
set(LAUNCHER_SOURCES_BASE
    "sources/console.cpp"
    "sources/consolethread.cpp"
    "sources/filesystem_init.cpp"
    "sources/launcher.cpp"
    "sources/main.cpp"
    "sources/modulelist.cpp"
    "sources/sourceapp.cpp"
    "sources/stdafx.cpp"
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

set(LAUNCHER_PRECOMPILED_HEADER "headers/stdafx.hpp")

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
    "headers/git-version.hpp"
    "headers/hooks.hpp"
    "headers/ifilesystem.hpp"
    "headers/leakdump.hpp"
    "headers/modulelist.hpp"
    "headers/onloadlib.hpp"
    "headers/sourceapp.hpp"
    "headers/tierextra.hpp"
    "headers/utilities.hpp"
    "headers/version.hpp"
    ${LAUNCHER_PRECOMPILED_HEADER})

set(LAUNCHER_RESOURCES_BASE "resources/launcher.ico" "resources/launcher.rc"
                            "resources/resource.hpp")

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
     ${LAUNCHER_HEADERS_IMGUIS})

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

#
# Add executable to build.
#
if(WIN32)
  add_executable(launcher WIN32 ${LAUNCHER_ALL_SOURCES})
endif()

# set the builds out directory
if(WIN32)
  set(LAUNCHER_SYSTEM_OUT_DIR
      "Win32"
      CACHE INTERNAL "The directory where to save platform specific binaries")
endif()

set(LAUNCHER_OUT_DIR_BIN
    "${PROJECT_SOURCE_DIR}/out/bin/${LAUNCHER_SYSTEM_OUT_DIR}"
    CACHE STRING "The directory where to output compiled executables")

set(LAUNCHER_OUT_DIR_LIB
    "${PROJECT_SOURCE_DIR}/out/lib/${LAUNCHER_SYSTEM_OUT_DIR}"
    CACHE STRING "The directory where to output compiled libraries")

set_target_properties(launcher
                      PROPERTIES ARCHIVE_OUTPUT_DIRECTORY
                                 ${LAUNCHER_OUT_DIR_LIB}
                                 LIBRARY_OUTPUT_DIRECTORY
                                 ${LAUNCHER_OUT_DIR_LIB}
                                 RUNTIME_OUTPUT_DIRECTORY
                                 ${LAUNCHER_OUT_DIR_BIN})

# generate launcher version header
add_dependencies(launcher LauncherVersion)

# generate git version header
add_dependencies(launcher GitVersion)

#
# resource file for windows
#
if(WIN32)
  target_sources(launcher PRIVATE "resources/launcher.rc")
endif()

#
# library dependencies
#
target_link_libraries(launcher ${LAUNCHER_REQUIRED_LIBRARIES})

# source sdk
target_link_directories(launcher PRIVATE ${PROJECT_SOURCE_DIR}/sdk/lib/public)

target_link_libraries(launcher tier0.lib vstdlib.lib)

# enable whole program optimization for dependencies
foreach(lib ${LAUNCHER_REQUIRED_LIBRARIES})
  target_compile_options(${lib} PRIVATE "$<$<CONFIG:Release>:/GL>")
  target_link_options(${lib} PRIVATE "$<$<CONFIG:Release>:/LTCG>")
endforeach()

# make polyhook's std::string compatible with the game bin
target_compile_definitions(PolyHook_2 PRIVATE _ITERATOR_DEBUG_LEVEL=0)

if(WIN32)
  target_link_libraries(launcher wsock32.lib)
endif()

set_target_properties(launcher
                      PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT
                                 ${LAUNCHER_PRECOMPILED_HEADER}
                                 COTIRE_ADD_UNITY_BUILD
                                 FALSE)

cotire(launcher)
