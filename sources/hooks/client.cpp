#include "hooks.hpp"
#include "utilities.hpp"
#include "utilities/log.hpp"
#include "utilities/memorypatterns.hpp"

#include <engine/cso2/icso2msgmanager.hpp>
#include <tier0/icommandline.hpp>

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

bool LookupClientAddresses()
{
    Log::Debug( "Looking up addresses in client.dll...\n" );

    int results = 0;

    MemoryPatterns& patterns = MemoryPatterns::Singleton();

    results += !patterns.AddPattern(
        "\xE8\xCC\xCC\xCC\xCC\x84\xC0\x0F\x84\xCC\xCC\xCC\xCC\xB9\xCC\xCC\xCC"
        "\xCC\xE8\xCC\xCC\xCC\xCC\x8B\x47\x20",
        "CSO2UIManagerInitMainUI",
        IMemoryPatternsOptions( -1, 1, 5, "client.dll", 0, true ) );

    const bool foundAllAddresses = results == 0;

    if ( foundAllAddresses == true )
    {
        Log::Debug( "Looked up client.dll addresses successfully\n" );
    }
    else
    {
        Log::Error( "Failed to find {} client.dll addresses\n", results );
    }

    return foundAllAddresses;
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

    LookupClientAddresses();
    BytePatchClient( dwClientBase );

    MemoryPatterns& patterns = MemoryPatterns::Singleton();

    PLH::CapstoneDisassembler dis( PLH::Mode::x86 );

    const uintptr_t initUiAddr =
        patterns.GetPattern( "CSO2UIManagerInitMainUI" );

    if ( initUiAddr != 0 )
    {
        g_pInitUIHook = SetupDetourHook(
            initUiAddr, &hkCSO2UIManager_InitMainUI, &g_InitUIOrig, dis );
        g_pInitUIHook->hook();
    }

    if ( CommandLine()->CheckParm( "-unpackedfiles", nullptr ) )
    {
        ApplyLuaClientHooks( dwClientBase );
        ApplyScaleformHooks( dwClientBase );
    }
}