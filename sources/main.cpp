#include <windows.h>

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

int LauncherMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,
                  int nCmdShow );

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nShowCmd )
{
    return LauncherMain( hInstance, hPrevInstance, lpCmdLine, nShowCmd );
}
