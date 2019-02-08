#include "stdafx.hpp"

#include "hooks.hpp"

extern void OnEngineLoaded( const uintptr_t dwEngineBase );
extern void OnFileSystemLoaded( const uintptr_t dwFilesystemBase );
extern void OnServerLoaded( const uintptr_t dwServerBase );
extern void OnVguiLoaded( const uintptr_t dwVguiBase );

void OnLoadLibrary( HMODULE hLibrary, std::string_view libPathView )
{
    // convert the library path to just the library file name
    std::filesystem::path libPath = libPathView;
    const std::string szLibName = libPath.filename().string();

	const uintptr_t dwLibraryBase = reinterpret_cast<uintptr_t>( hLibrary );

    if ( szLibName == "engine.dll" )
    {
        OnEngineLoaded( dwLibraryBase );
    }
    else if ( szLibName == "filesystem_stdio.dll" )
    {
        OnFileSystemLoaded( dwLibraryBase );
    }
    else if ( szLibName == "server.dll" )
    {
        OnServerLoaded( dwLibraryBase );
    }
    else if ( szLibName == "vgui2.dll" )
    {
        OnVguiLoaded( dwLibraryBase );
    }
}

HOOK_DETOUR_DECLARE( hkLoadLibraryExA );

NOINLINE HMODULE WINAPI hkLoadLibraryExA( LPCSTR lpLibFileName, HANDLE hFile,
                                          DWORD dwFlags )
{
    HMODULE hLoadedModule = HOOK_DETOUR_GET_ORIG( hkLoadLibraryExA )(
        lpLibFileName, hFile, dwFlags );
    OnLoadLibrary( hLoadedModule, lpLibFileName );
    return hLoadedModule;
}

void HookWinapi()
{
    HOOK_DETOUR( LoadLibraryExA, hkLoadLibraryExA );
}
