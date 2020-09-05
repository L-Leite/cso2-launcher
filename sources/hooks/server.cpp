#include "hooks.hpp"
#include "utilities.hpp"

#include <tier0/icommandline.hpp>

void BytePatchServer( const uintptr_t dwServerBase )
{
    // treat surface prop file as encrypted (even if it's not)
    // nops
    const std::array<uint8_t, 8> surfPropPatch = { 0x90, 0x90, 0x90, 0x90,
                                                   0x90, 0x90, 0x90, 0x90 };
    utils::WriteProtectedMemory( surfPropPatch, dwServerBase + 0x41248C );
}

extern void ApplyLuaServerHooks( const uintptr_t dwServerBase );

void OnServerLoaded( const uintptr_t dwServerBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

    if ( CommandLine()->CheckParm( "-unpackedfiles", nullptr ) )
    {
        BytePatchServer( dwServerBase );
        ApplyLuaServerHooks( dwServerBase );
    }
}
