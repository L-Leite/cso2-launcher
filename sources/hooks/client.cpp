#include "hooks.hpp"

#include "engine/cso2/icso2msgmanager.h"
#include "tier0/ICommandLine.h"

static std::unique_ptr<PLH::x86Detour> g_pInitUIHook;
static uint64_t g_InitUIOrig = 0;

NOINLINE bool __fastcall hkCSO2UIManager_InitMainUI( void* ecx, void* edx )
{
    const char* szUsername = CommandLine()->ParmValueStr( "-username" );
    const char* szPassword = CommandLine()->ParmValueStr( "-password" );

    if ( szUsername && szPassword )
    {
        g_pCSO2MsgHandler->Login( szUsername, szPassword, szUsername );
    }

    return PLH::FnCast( g_InitUIOrig, &hkCSO2UIManager_InitMainUI )( ecx, edx );
}

void OnClientLoaded( const uintptr_t dwClientBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

    PLH::CapstoneDisassembler dis( PLH::Mode::x86 );

    g_pInitUIHook =
        SetupDetourHook( dwClientBase + 0xAE4610, &hkCSO2UIManager_InitMainUI,
                         &g_InitUIOrig, dis );
    g_pInitUIHook->hook();
}