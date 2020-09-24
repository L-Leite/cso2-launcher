#include "hooks.hpp"
#include "utilities.hpp"

#include <filesystem>

#include <windows.h>

extern void OnClientLoaded( const uintptr_t dwClientBase );
extern void OnEngineLoaded( const uintptr_t dwEngineBase );
extern void OnFileSystemLoaded( const uintptr_t dwFsBase );
extern void OnServerLoaded( const uintptr_t dwServerBase );
extern void OnTierZeroLoaded( const uintptr_t dwTierBase );
extern void OnVguiLoaded( const uintptr_t dwVguiBase );

void OnLoadLibrary( HMODULE hLibrary, std::string_view libPathView )
{
    // convert the library path to just the library file name
    std::filesystem::path libPath = libPathView;
    std::string szLibName = libPath.filename().string();
    utils::ToLower( szLibName );

    const uintptr_t dwLibraryBase = reinterpret_cast<uintptr_t>( hLibrary );

    if ( szLibName == "engine.dll" )
    {
        OnEngineLoaded( dwLibraryBase );
    }
    else if ( szLibName == "filesystem_stdio.dll" )
    {
        OnFileSystemLoaded( dwLibraryBase );
    }
    else if ( szLibName == "vgui2.dll" )
    {
        OnVguiLoaded( dwLibraryBase );
    }
    else if ( szLibName == "client.dll" )
    {
        OnClientLoaded( dwLibraryBase );
    }
    else if ( szLibName == "server.dll" )
    {
        OnServerLoaded( dwLibraryBase );
    }
    else if ( szLibName == "tier0.dll" )
    {
        OnTierZeroLoaded( dwLibraryBase );
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
    auto dis = HookDisassembler();

    g_pLoadLibExHook =
        SetupDetourHook( reinterpret_cast<uintptr_t>( &LoadLibraryExA ),
                         &hkLoadLibraryExA, &g_LoadLibExOrig, dis );
    g_pLoadLibExHook->hook();
}
