#include "stdafx.hpp"

#include "console.hpp"
#include "hooks.hpp"
#include "tier0/ICommandLine.h"
#include "tier0/platform.h"

HOOK_EXPORT_DECLARE( hkCOM_TimestampedLog );

NOINLINE void hkCOM_TimestampedLog( char const* fmt, ... )
{
    static float s_LastStamp = 0.0;
    static bool s_bFirstWrite = false;

    char string[1024];
    va_list argptr;
    va_start( argptr, fmt );
    _vsnprintf( string, sizeof( string ), fmt, argptr );
    va_end( argptr );

    float curStamp = Plat_FloatTime();

    if ( !s_bFirstWrite )
    {
        std::filesystem::remove( std::filesystem::current_path() /
                                 "timestamped.log" );
        s_bFirstWrite = true;
    }

    std::ofstream logStream( "timestamped.log", std::ios::app );

    if ( logStream.good() )
    {
        logStream << std::setw( 8 ) << std::setprecision( 4 ) << curStamp
                  << curStamp - s_LastStamp << string << '\n';
    }

    s_LastStamp = curStamp;
}					  

void BytePatchTier( const uintptr_t dwTierBase )
{
    //
    // disable hard coded command line argument check
    //
    // jmp near 0x1D8 bytes forward
    const std::array<uint8_t, 5> addArgPatch = { 0xE9, 0xD8, 0x01, 0x00, 0x00 };
    utils::WriteProtectedMemory( addArgPatch, ( dwTierBase + 0x1D63 ) );
}

void HookTier0()
{
    const uintptr_t dwTierBase = utils::GetModuleBase( "tier0.dll" );
    BytePatchTier( dwTierBase );

	// only hook COM_TimestampedLog if we are actually going to use it
    if ( CommandLine()->CheckParm( "-timestamped", nullptr ) )
    {
        HOOK_EXPORT( "COM_TimestampedLog", L"tier0.dll", hkCOM_TimestampedLog );
    }
}
