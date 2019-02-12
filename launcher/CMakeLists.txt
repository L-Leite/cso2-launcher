cmake_minimum_required(VERSION 3.13.0)

include(cotire)

# Generate version.h at build time.
add_custom_target(LauncherVersion ALL
                  COMMAND ${CMAKE_COMMAND}
                          -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P
                          ${PROJECT_SOURCE_DIR}/cmake_modules/version.cmake)

# Generate git-version.h at build time.
add_custom_target(GitVersion ALL
                  COMMAND ${CMAKE_COMMAND}
                          -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -P
                          ${PROJECT_SOURCE_DIR}/cmake_modules/git-version.cmake)

#
# project's include directories
#
set(LAUNCHER_PROJECT_INCLUDES header)

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

set(LAUNCHER_IMGUI_INCLUDES 
    source/imgui
    source/imgui/imgui_impl)

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
# extra linker flags
# make app support large addresses (>2gb) and
# enable whole program optimization for release builds
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
    "source/console.cpp"
    "source/consolethread.cpp"
    "source/filesystem_init.cpp"
    "source/launcher.cpp"
    "source/main.cpp"
    "source/modulelist.cpp"
    "source/sourceapp.cpp"
    "source/stdafx.cpp"
    "source/tierextra.cpp")

set(LAUNCHER_SOURCES_HOOKS
	"source/hooks/client.cpp"
    "source/hooks/engine.cpp"
	"source/hooks/filesystem.cpp"
    "source/hooks/server.cpp"
    "source/hooks/tier0.cpp"
    "source/hooks/vgui2.cpp"
    "source/hooks/winapi.cpp")

set(LAUNCHER_SOURCES_IMGUIS
    "source/imgui/imgui.cpp"
	"source/imgui/imgui_draw.cpp"
    "source/imgui/imgui_widgets.cpp"
	"source/imgui/Consolas_Unicode.cpp"
    "source/imgui/imgui_impl/imgui_impl_dx9.cpp"
    "source/imgui/imgui_impl/imgui_impl_win32.cpp")

set(LAUNCHER_SOURCES_APPFRAMEWORK "source/appframework/AppSystemGroup.cpp"
    "source/appframework/WinApp.cpp")

set(LAUNCHER_SOURCES_TIER1
    "source/tier1/convar.cpp"
    "source/tier1/generichash.cpp"
    "source/tier1/interface.cpp"
    "source/tier1/KeyValues.cpp"
    "source/tier1/strtools.cpp"
    "source/tier1/strtools_unicode.cpp"
    "source/tier1/tier1.cpp"
    "source/tier1/utlbuffer.cpp"
    "source/tier1/utlstring.cpp")

set(LAUNCHER_SOURCES_TIER2 "source/tier2/tier2.cpp")

set(LAUNCHER_SOURCES_TIER3 "source/tier3/tier3.cpp")

set(LAUNCHER_PRECOMPILED_HEADER "header/stdafx.hpp")

set(LAUNCHER_HEADERS_IMGUIS
    "source/imgui/imconfig.h"
	"source/imgui/imgui.h"
    "source/imgui/imgui_internal.h"
    "source/imgui/imstb_rectpack.h"
    "source/mgui/imstb_textedit.h"
    "source/imgui/imstb_truetype.h"
    "source/imgui/imgui_impl/imgui_impl_dx9.h"
    "source/imgui/imgui_impl/imgui_impl_win32.h")

set(LAUNCHER_HEADERS_BASE
    "header/basedir.hpp"
    "header/console.hpp"
    "header/git-version.hpp"
    "header/hooks.hpp"
    "header/ifilesystem.hpp"
    "header/leakdump.hpp"
    "header/modulelist.hpp"
    "header/onloadlib.hpp"
    "header/sourceapp.hpp"
    "header/tierextra.hpp"
	  "header/utilities.hpp"
    "header/version.hpp"
    ${LAUNCHER_PRECOMPILED_HEADER})

set(LAUNCHER_RESOURCES_BASE
    "resource/launcher.ico"
    "resource/launcher.rc"
    "resource/resource.hpp")

file(GLOB LAUNCHER_ALL_SOURCES
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
  set(LAUNCHER_SYSTEM_OUT_DIR "Win32"
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
  target_sources(launcher PRIVATE "resource/launcher.rc")
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
