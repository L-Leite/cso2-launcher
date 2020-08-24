#include <array>
#include <cstdio>

#include <tier0/icommandline.hpp>
#include <tier0/platform.hpp>

#include "console.hpp"
#include "hooks.hpp"
#include "utilities.hpp"
#include "utilities/log.hpp"
#include "utilities/memorypatterns.hpp"

static std::unique_ptr<PLH::EatHook> g_pTimestampedHook;
static uint64_t g_pTimestampedOrig = 0;  // unused but needed by polyhook

NOINLINE void hkCOM_TimestampedLog( char const* fmt, ... )
{
    static float s_LastStamp = 0.0;
    static bool s_bShouldLog = false;
    static bool s_bChecked = false;
    static bool s_bFirstWrite = false;

    if ( !s_bChecked )
    {
        s_bShouldLog = CommandLine()->CheckParm( "-profile" ) != nullptr;
        s_bChecked = true;
    }
    if ( !s_bShouldLog )
    {
        return;
    }

    char string[1024];
    va_list argptr;
    va_start( argptr, fmt );
    _vsnprintf( string, sizeof( string ), fmt, argptr );
    va_end( argptr );

    float curStamp = Plat_FloatTime();

    if ( !s_bFirstWrite )
    {
        _unlink( "timestamped.log" );
        s_bFirstWrite = true;
    }

    FILE* file = nullptr;

    if ( fopen_s( &file, "timestamped.log", "at+" ) == 0 )
    {
        fprintf_s( file, "%8.4f / %8.4f:  %s\n", curStamp,
                   curStamp - s_LastStamp, string );
        fclose( file );
    }

    s_LastStamp = curStamp;
}

bool LookupTierZeroAddresses()
{
    Log::Debug( "Looking up addresses in tier0.dll...\n" );

    int results = 0;

    MemoryPatterns& patterns = MemoryPatterns::Singleton();

    results += !patterns.AddPattern(
        "\x80\xBF\x0C\x04", "CmdArgCheck",
        IMemoryPatternsOptions( -1, -1, -1, "tier0.dll" ) );

    results += !patterns.AddPattern(
        "\x53\xE8\xCC\xCC\xCC\xCC\x8B\x4F\x08", "CmdArgCheckTarget",
        IMemoryPatternsOptions( -1, -1, -1, "tier0.dll" ) );

    const bool foundAllAddresses = results == 0;

    if ( foundAllAddresses == true )
    {
        Log::Debug( "Looked up tier0.dll addresses successfully\n" );
    }
    else
    {
        Log::Error( "Failed to find {} tier0.dll addresses\n", results );
    }

    return foundAllAddresses;
}

void BytePatchTier()
{
    MemoryPatterns& patterns = MemoryPatterns::Singleton();

    //
    // disable hard coded command line argument check
    //
    const uintptr_t cmdCheckAddr = patterns.GetPattern( "CmdArgCheck" );
    const uintptr_t cmdCheckTargetAddr =
        patterns.GetPattern( "CmdArgCheckTarget" );

    if ( cmdCheckAddr != 0 && cmdCheckTargetAddr != 0 )
    {
        // jmp near some bytes forward
        std::array<uint8_t, 5> addArgPatch = { 0xE9, 0xD8, 0x01, 0x00, 0x00 };
        *reinterpret_cast<uintptr_t*>( &addArgPatch[1] ) =
            cmdCheckTargetAddr - cmdCheckAddr - 5;
        utils::WriteProtectedMemory( addArgPatch, cmdCheckAddr );
    }
}

void OnTierZeroLoaded()
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

    LookupTierZeroAddresses();
    BytePatchTier();

    g_pTimestampedHook =
        SetupExportHook( "COM_TimestampedLog", L"tier0.dll",
                         &hkCOM_TimestampedLog, &g_pTimestampedOrig );
    g_pTimestampedHook->hook();
}
