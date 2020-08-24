#include <filesystem>
#include <iostream>

#include <windows.h>

#include "hooks.hpp"

extern void OnClientLoaded();
extern void OnEngineLoaded();
extern void OnFileSystemLoaded();
extern void OnShaderApiLoaded();
extern void OnTierZeroLoaded();
extern void OnVguiLoaded();

void OnLoadLibrary( HMODULE hLibrary, std::string_view libPathView )
{
    // convert the library path to just the library file name
    std::filesystem::path libPath = libPathView;
    const std::string szLibName = libPath.filename().string();

    const uintptr_t dwLibraryBase = reinterpret_cast<uintptr_t>( hLibrary );

    if ( szLibName == "engine.dll" )
    {
        OnEngineLoaded();
    }
    else if ( szLibName == "filesystem_stdio.dll" )
    {
        OnFileSystemLoaded();
    }
    else if ( szLibName == "shaderapidx9.dll" )
    {
        OnShaderApiLoaded();
    }
    else if ( szLibName == "vgui2.dll" )
    {
        OnVguiLoaded();
    }
    else if ( szLibName == "client.dll" )
    {
        OnClientLoaded();
    }
    else if ( szLibName == "tier0.dll" )
    {
        OnTierZeroLoaded();
    }
}

static std::unique_ptr<PLH::x86Detour> g_pLoadLibExHook;
static uint64_t g_LoadLibExOrig = 0;

NOINLINE HMODULE WINAPI hkLoadLibraryExA( LPCSTR lpLibFileName, HANDLE hFile,
                                          DWORD dwFlags )
{
    HMODULE hLoadedModule = PLH::FnCast( g_LoadLibExOrig, hkLoadLibraryExA )(
        lpLibFileName, hFile, dwFlags );
    OnLoadLibrary( hLoadedModule, lpLibFileName );
    return hLoadedModule;
}

void HookWinapi()
{
    PLH::CapstoneDisassembler dis( PLH::Mode::x86 );

    g_pLoadLibExHook =
        SetupDetourHook( reinterpret_cast<uintptr_t>( &LoadLibraryExA ),
                         &hkLoadLibraryExA, &g_LoadLibExOrig, dis );
    g_pLoadLibExHook->hook();
}
