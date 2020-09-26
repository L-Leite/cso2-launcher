#include <array>

#include <tier0/icommandline.hpp>

#include "hooks.hpp"
#include "utilities.hpp"

void BytePatchFilesystem( const uintptr_t dwFilesystemBase )
{
    if ( CommandLine()->CheckParm( "-unpackedfiles", nullptr ) )
    {
        //
        // force files with extensions 'txt', 'csv', 'cfg' or 'db'
        // to be loaded as plain text
        //
        // mov al, 0; retn
        const std::array<uint8_t, 5> encryptPatch = { 0xB0, 0x00, 0xC3 };
        utils::WriteProtectedMemory( encryptPatch, dwFilesystemBase + 0x6A3B0 );
    }
}

extern void ApplyCSO2FileSystemHooks( const uintptr_t dwFsBase );

void OnFileSystemLoaded( const uintptr_t dwFsBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
        return;

    bHasLoaded = true;

    BytePatchFilesystem( dwFsBase );

    if ( CommandLine()->CheckParm( "-unpackedfiles", nullptr ) )
    {
        ApplyCSO2FileSystemHooks( dwFsBase );
    }
}
