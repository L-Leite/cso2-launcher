#include "hooks.hpp"
#include "utilities.hpp"

#include <engine/cso2/icso2msgmanager.hpp>
#include <tier0/icommandline.hpp>

uintptr_t g_dwClientBase = 0;

static std::unique_ptr<PLH::x86Detour> g_pInitUIHook;
static uint64_t g_InitUIOrig = 0;

NOINLINE bool __fastcall hkCSO2UIManager_InitMainUI( void* ecx, void* edx )
{
    const char* szUsername = CommandLine()->ParmValue( "-username" );
    const char* szPassword = CommandLine()->ParmValue( "-password" );

    if ( szUsername && szPassword )
    {
        g_pCSO2MsgHandler->Login( szUsername, szPassword, szUsername );
    }

    return PLH::FnCast( g_InitUIOrig, &hkCSO2UIManager_InitMainUI )( ecx, edx );
}

void BytePatchClient( const uintptr_t dwClientBase )
{
    if ( CommandLine()->CheckParm( "-unpackedfiles", nullptr ) )
    {
        // treat surface prop file as encrypted (even if it's not)
        // nops
        const std::array<uint8_t, 8> surfPropPatch = { 0x90, 0x90, 0x90, 0x90,
                                                       0x90, 0x90, 0x90, 0x90 };
        utils::WriteProtectedMemory( surfPropPatch, dwClientBase + 0x9040AC );
    }
}

extern void ApplyLuaClientHooks( const uintptr_t dwClientBase );
extern void ApplyScaleformHooks( const uintptr_t dwClientBase );

void OnClientLoaded( const uintptr_t dwClientBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

    g_dwClientBase = dwClientBase;

    BytePatchClient( dwClientBase );

    auto dis = HookDisassembler();

    g_pInitUIHook =
        SetupDetourHook( dwClientBase + 0xAE4610, &hkCSO2UIManager_InitMainUI,
                         &g_InitUIOrig, dis );
    g_pInitUIHook->hook();

    if ( CommandLine()->CheckParm( "-unpackedfiles", nullptr ) )
    {
        ApplyLuaClientHooks( dwClientBase );
        ApplyScaleformHooks( dwClientBase );
    }
}