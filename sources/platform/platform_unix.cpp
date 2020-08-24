#include "platform.hpp"

#include <cstdlib>

#include <dlfcn.h>

bool InitTextMode()
{
    return true;
}

SpewRetval_t LauncherDefaultSpewFunc( SpewType_t spewType, const char* pMsg )
{
    printf( pMsg );

    switch ( spewType )
    {
        case SPEW_MESSAGE:
        case SPEW_LOG:
            return SPEW_CONTINUE;

        case SPEW_WARNING:
            return SPEW_CONTINUE;

        case SPEW_ASSERT:
            return SPEW_DEBUGGER;

        case SPEW_ERROR:
        default:
            std::exit( 1 );
    }
}

bool NET_Init()
{
    return true;
}

bool NET_Destroy()
{
    return true;
}

void Proc_SetHighPriority() {}

void Proc_SetLowPriority() {}

void* Sys_LoadLibrary( const char* moduleName )
{
    return dlopen( moduleName, RTLD_NOW );
}

void Sys_FreeLibrary( void* moduleBase )
{
    dlclose( moduleBase );
}

void* Sys_GetModuleBase( const char* moduleName )
{
    return dlopen( moduleName, RTLD_NOLOAD );
}

void* Sys_GetModuleExport( void* moduleBase, const char* exportName )
{
    return dlsym( moduleBase, exportName );
}

void Sys_InitPlatformSpecific() {}

void Sys_DestroyPlatformSpecific() {}

void User_MessageBox( const char* msg, const char* boxTitle ) {}