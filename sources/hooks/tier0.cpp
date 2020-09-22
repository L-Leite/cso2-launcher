#include <array>
#include <cstdio>

#include <tier0/icommandline.hpp>
#include <tier0/platform.hpp>

#include "hooks.hpp"
#include "utilities.hpp"

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

void BytePatchTier( const uintptr_t dwTierBase )
{
    //
    // disable hard coded command line argument check
    //
    // jmp near 0x1D8 bytes forward
    std::array<uint8_t, 5> addArgPatch = { 0xE9, 0xD8, 0x01, 0x00, 0x00 };
    utils::WriteProtectedMemory( addArgPatch, dwTierBase + 0x1D63 );
}

void OnTierZeroLoaded( const uintptr_t dwTierBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

    BytePatchTier( dwTierBase );

    g_pTimestampedHook =
        SetupExportHook( "COM_TimestampedLog", L"tier0.dll",
                         &hkCOM_TimestampedLog, &g_pTimestampedOrig );
    g_pTimestampedHook->hook();
}
