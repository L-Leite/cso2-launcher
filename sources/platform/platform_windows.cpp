#include "platform.hpp"

#include <cstdint>

#include <objbase.h>
#include <windows.h>

#include <shellapi.h>
#include <wincon.h>
#include <winsock2.h>

BOOL WINAPI TextConHandler( DWORD ctrlType )
{
    TerminateProcess( GetCurrentProcess(), 2 );
    return TRUE;
}

bool InitTextMode()
{
    BOOL bCreated = AllocConsole();

    if ( bCreated == FALSE )
    {
        return false;
    }

    SetConsoleCtrlHandler( &TextConHandler, TRUE );

    FILE* conIn;
    FILE* conOut;
    FILE* conErr;

    freopen_s( &conIn, "CONIN$", "rb", stdin );
    freopen_s( &conOut, "CONOUT$", "wb", stdout );
    freopen_s( &conErr, "CONOUT$", "wb", stderr );

    SetConsoleCP( CP_UTF8 );
    SetConsoleOutputCP( CP_UTF8 );

    return true;
}

SpewRetval_t LauncherDefaultSpewFunc( SpewType_t spewType, const char* pMsg )
{
    OutputDebugStringA( pMsg );

    switch ( spewType )
    {
        case SPEW_MESSAGE:
        case SPEW_LOG:
            return SPEW_CONTINUE;

        case SPEW_WARNING:
            if ( !stricmp( GetSpewOutputGroup(), "init" ) )
            {
                MessageBoxA( nullptr, pMsg, "Warning!",
                             MB_OK | MB_SYSTEMMODAL | MB_ICONERROR );
            }
            return SPEW_CONTINUE;

        case SPEW_ASSERT:
            if ( !ShouldUseNewAssertDialog() )
            {
                MessageBoxA( nullptr, pMsg, "Assert!",
                             MB_OK | MB_SYSTEMMODAL | MB_ICONERROR );
            }
            return SPEW_DEBUGGER;

        case SPEW_ERROR:
        default:
            MessageBoxA( nullptr, pMsg, "Error!",
                         MB_OK | MB_SYSTEMMODAL | MB_ICONERROR );
            std::exit( 1 );
    }
}

bool NET_Init()
{
    WSAData wsaData;
    return WSAStartup( MAKEWORD( 2, 0 ), &wsaData ) == 0;
}

bool NET_Destroy()
{
    return WSACleanup() == 0;
}

void Proc_SetHighPriority()
{
    SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );
}

void Proc_SetLowPriority()
{
    SetPriorityClass( GetCurrentProcess(), IDLE_PRIORITY_CLASS );
}

void* Sys_LoadLibrary( const char* moduleName )
{
    return reinterpret_cast<void*>( LoadLibraryA( moduleName ) );
}

void Sys_FreeLibrary( void* moduleBase )
{
    FreeLibrary( reinterpret_cast<HMODULE>( moduleBase ) );
}

void Sys_InitPlatformSpecific()
{
    CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED );
}

void Sys_DestroyPlatformSpecific()
{
    CoUninitialize();
}

void* Sys_GetModuleExport( void* moduleBase, const char* exportName )
{
    return reinterpret_cast<void*>(
        GetProcAddress( reinterpret_cast<HMODULE>( moduleBase ), exportName ) );
}

void User_MessageBox( const char* msg, const char* boxTitle )
{
    MessageBoxA( nullptr, msg, boxTitle, 0 );
}