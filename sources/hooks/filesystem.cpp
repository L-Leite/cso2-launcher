#include <array>
#include <filesystem>

#include <tier0/icommandline.hpp>

#include "hooks.hpp"
#include "utilities.hpp"
#include "utilities/log.hpp"
#include "utilities/memorypatterns.hpp"

void BytePatchFilesystem()
{
    MemoryPatterns& patterns = MemoryPatterns::Singleton();

    //
    // force files with extensions 'txt', 'csv', 'cfg' or 'db'
    // to be loaded as plain text
    //
    const uintptr_t fileEncAddr = patterns.GetPattern( "FsIsFileEncrypted" );

    if ( fileEncAddr != 0 )
    {
        // mov al, 0; retn
        const std::array<uint8_t, 5> patch = { 0xB0, 0x00, 0xC3 };
        utils::WriteProtectedMemory( patch, fileEncAddr );
    }
}

bool LookupFSAddresses()
{
    Log::Debug( "Looking up addresses in filesystem_stdio.dll...\n" );

    int results = 0;

    MemoryPatterns& patterns = MemoryPatterns::Singleton();

    results += !patterns.AddPattern(
        "\x56\x8B\xF1\x57\x83\x7E\x14\x10\x8B\x7E\x10", "FsIsFileEncrypted",
        IMemoryPatternsOptions( -1, -1, -1, "filesystem_stdio.dll" ) );

    const bool foundAllAddresses = results == 0;

    if ( foundAllAddresses == true )
    {
        Log::Debug( "Looked up filesystem_stdio.dll addresses successfully\n" );
    }
    else
    {
        Log::Error( "Failed to find {} filesystem_stdio.dll addresses\n",
                    results );
    }

    return foundAllAddresses;
}

extern void ApplyCSO2FileSystemHooks( const uintptr_t dwFsBase );

void OnFileSystemLoaded( const uintptr_t dwFsBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
        return;

    bHasLoaded = true;

    if ( CommandLine()->CheckParm( "-unpackedfiles", nullptr ) )
    {
        LookupFSAddresses();
        BytePatchFilesystem();
        ApplyCSO2FileSystemHooks( dwFsBase );
    }
}
