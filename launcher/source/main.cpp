#include "stdafx.hpp"

#include "git-version.hpp"
#include "version.hpp"

enum Sys_Flags
{
    SYS_NOFLAGS = 0x00,
    SYS_NOLOAD = 0x01  // no loading, no ref-counting, only returns handle if
                       // lib is loaded.
};

// same as InternalLoadLibrary
extern "C" __declspec( dllexport ) HMODULE
    _Init( const char* pName, Sys_Flags flags )
{
    HMODULE hModule = nullptr;

    if ( flags & SYS_NOLOAD )
    {
        hModule = GetModuleHandle( pName );
    }
    else
    {
        hModule =
            LoadLibraryExA( pName, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH );
    }

    return hModule;
}

void CreateDebugConsole()
{
    AllocConsole();

    freopen( "CONIN$", "r", stdin );
    freopen( "CONOUT$", "w", stdout );
    freopen( "CONOUT$", "w", stderr );

    SetConsoleTitleA( "cso2-launcher " LAUNCHER_VERSION "-" GIT_BRANCH
                      "-" GIT_COMMIT_HASH " -- Game Console" );
    SetConsoleCP( CP_UTF8 );
    SetConsoleOutputCP( CP_UTF8 );
}

int LauncherMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
                  int nCmdShow );

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                    _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
    CreateDebugConsole();
    return LauncherMain( hInstance, hPrevInstance, lpCmdLine, nShowCmd );
}
