#include "hooks.hpp"
#include "utilities.hpp"
#include "utilities/log.hpp"
#include "utilities/memorypatterns.hpp"

#include <intrin.h>

std::unique_ptr<PLH::VFuncSwapHook> g_pCFsHook;
PLH::VFuncMap g_StrTblOrig;

NOINLINE bool __fastcall CFS_InitFs( void* )
{
    return true;
}

NOINLINE void __fastcall CFS_02()
{
    Log::Debug( "CSO2FileSystem::02 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_03()
{
    Log::Debug( "CSO2FileSystem::03 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_04()
{
    Log::Debug( "CSO2FileSystem::05 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_05()
{
    Log::Debug( "CSO2FileSystem::05 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_06()
{
    Log::Debug( "CSO2FileSystem::06 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_07()
{
    Log::Debug( "CSO2FileSystem::07 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_08()
{
    Log::Debug( "CSO2FileSystem::08 called. ret {}\n", _ReturnAddress() );
}

NOINLINE bool __fastcall CFS_FileExists( void*, void*, const std::wstring& )
{
    return false;
}

NOINLINE void __fastcall CFS_10()
{
    Log::Debug( "CSO2FileSystem::10 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_11()
{
    Log::Debug( "CSO2FileSystem::11 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_12()
{
    Log::Debug( "CSO2FileSystem::12 called. ret {}\n", _ReturnAddress() );
}

NOINLINE std::string* __fastcall CFS_GetModName()
{
    return new std::string( "cstrike" );
}

NOINLINE std::string* __fastcall CFS_GetModName2()
{
    return new std::string( "cstrike" );
}

NOINLINE void __fastcall CFS_15()
{
    Log::Debug( "CSO2FileSystem::15 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_16()
{
    Log::Debug( "CSO2FileSystem::16 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_17()
{
    Log::Debug( "CSO2FileSystem::17 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_18()
{
    Log::Debug( "CSO2FileSystem::18 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_19()
{
    Log::Debug( "CSO2FileSystem::19 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_20()
{
    Log::Debug( "CSO2FileSystem::20 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_21()
{
    Log::Debug( "CSO2FileSystem::21 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_22()
{
    Log::Debug( "CSO2FileSystem::22 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_23()
{
    Log::Debug( "CSO2FileSystem::23 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_24()
{
    Log::Debug( "CSO2FileSystem::24 called. ret {}\n", _ReturnAddress() );
}

NOINLINE void __fastcall CFS_25()
{
    Log::Debug( "CSO2FileSystem::25 called. ret {}\n", _ReturnAddress() );
}

void ApplyCSO2FileSystemHooks( const uintptr_t dwFsBase )
{
    Log::Debug( "dwFsBase: {}\n", (void*)dwFsBase );
    const PLH::VFuncMap fsRedirects = {
        { uint16_t( 1 ), reinterpret_cast<uint64_t>( &CFS_InitFs ) },
        { uint16_t( 2 ), reinterpret_cast<uint64_t>( &CFS_02 ) },
        { uint16_t( 3 ), reinterpret_cast<uint64_t>( &CFS_03 ) },
        { uint16_t( 4 ), reinterpret_cast<uint64_t>( &CFS_04 ) },
        { uint16_t( 5 ), reinterpret_cast<uint64_t>( &CFS_05 ) },
        { uint16_t( 6 ), reinterpret_cast<uint64_t>( &CFS_06 ) },
        { uint16_t( 7 ), reinterpret_cast<uint64_t>( &CFS_07 ) },
        { uint16_t( 8 ), reinterpret_cast<uint64_t>( &CFS_08 ) },
        { uint16_t( 9 ), reinterpret_cast<uint64_t>( &CFS_FileExists ) },
        { uint16_t( 10 ), reinterpret_cast<uint64_t>( &CFS_10 ) },
        { uint16_t( 11 ), reinterpret_cast<uint64_t>( &CFS_11 ) },
        { uint16_t( 12 ), reinterpret_cast<uint64_t>( &CFS_12 ) },
        { uint16_t( 13 ), reinterpret_cast<uint64_t>( &CFS_GetModName ) },
        { uint16_t( 14 ), reinterpret_cast<uint64_t>( &CFS_GetModName2 ) },
        { uint16_t( 15 ), reinterpret_cast<uint64_t>( &CFS_15 ) },
        { uint16_t( 16 ), reinterpret_cast<uint64_t>( &CFS_16 ) },
        { uint16_t( 17 ), reinterpret_cast<uint64_t>( &CFS_17 ) },
        { uint16_t( 18 ), reinterpret_cast<uint64_t>( &CFS_18 ) },
        { uint16_t( 19 ), reinterpret_cast<uint64_t>( &CFS_19 ) },
        { uint16_t( 20 ), reinterpret_cast<uint64_t>( &CFS_20 ) },
        { uint16_t( 21 ), reinterpret_cast<uint64_t>( &CFS_21 ) },
        { uint16_t( 22 ), reinterpret_cast<uint64_t>( &CFS_22 ) },
        { uint16_t( 23 ), reinterpret_cast<uint64_t>( &CFS_23 ) },
        { uint16_t( 24 ), reinterpret_cast<uint64_t>( &CFS_24 ) },
        { uint16_t( 25 ), reinterpret_cast<uint64_t>( &CFS_25 ) },
    };

    uintptr_t targetVtable = dwFsBase + 0xF8008;

    g_pCFsHook = SetupVfuncSwap( &targetVtable, fsRedirects, &g_StrTblOrig );
    g_pCFsHook->hook();
}
