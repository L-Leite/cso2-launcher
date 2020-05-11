#include "launcher.hpp"
#include "platform.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef _WIN32
// hint AMD and NVidia to use dedicated GPU
extern "C"
{
    _declspec( dllexport ) int AmdPowerXpressRequestHighPerformance = 1;
    _declspec( dllexport ) unsigned long NvOptimusEnablement = 0x00000001;
}
#endif

#ifdef _WIN32

enum Sys_Flags
{
    SYS_NOFLAGS = 0x00,
    SYS_NOLOAD = 0x01
};

// exported by the original launcher
// and used in engine.dll
// same as InternalLoadLibrary
extern "C" __declspec( dllexport ) HMODULE
    _Init( const char* pName, Sys_Flags flags )
{
    if ( flags & SYS_NOLOAD )
    {
        return GetModuleHandle( pName );
    }
    else
    {
        return LoadLibraryExA( pName, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH );
    }
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,
                    LPSTR lpCmdLine, int /*nShowCmd*/ )
{
    try
    {
        Launcher l( reinterpret_cast<void*>( hInstance ), lpCmdLine );
        return l.Main();
    }
    catch ( const std::exception& e )
    {
        User_MessageBox( e.what(), "Launcher Error" );
        return 1;
    }
}

#elif defined( POSIX )

int main( int argc, char* argv[] )
{
    try
    {
        Launcher l( nullptr, argv );
        return l.Main();
    }
    catch ( const std::exception& e )
    {
        User_MessageBox( e.what(), "Launcher Error" );
        return 1;
    }
}

#endif