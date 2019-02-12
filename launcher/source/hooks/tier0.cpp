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

HOOK_EXPORT_DECLARE( hkMsg );

NOINLINE void hkMsg( const tchar* pMsg, ... )
{
    va_list va;
    va_start( va, pMsg );
    vprintf( pMsg, va );
    va_end( va );
}

HOOK_EXPORT_DECLARE( hkWarning );

NOINLINE void hkWarning( const tchar* pMsg, ... )
{
    va_list va;
    va_start( va, pMsg );
    vprintf( pMsg, va );
    va_end( va );
}

HOOK_DETOUR_DECLARE( hkConMsg );

NOINLINE void hkConMsg( const char* pMsg, ... )
{
    va_list va;
    va_start( va, pMsg );
    vprintf( pMsg, va );
    va_end( va );
}

HOOK_DETOUR_DECLARE( hkDevMsg );

NOINLINE void hkDevMsg( const char* pMsg, ... )
{
    va_list va;
    va_start( va, pMsg );
    vprintf( pMsg, va );
    va_end( va );
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

    //HOOK_EXPORT( "Msg", L"tier0.dll", hkMsg );
    //HOOK_EXPORT( "Warning", L"tier0.dll", hkWarning );
    //HOOK_DETOUR( dwTierBase + 0x5C50, hkConMsg );
    //HOOK_DETOUR( dwTierBase + 0x5550, hkDevMsg );
}
