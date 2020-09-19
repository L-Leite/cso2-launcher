#include "hooks.hpp"
#include "utilities.hpp"

std::unique_ptr<PLH::VFuncSwapHook> g_pCFsHook;
PLH::VFuncMap g_StrTblOrig;

NOINLINE bool __fastcall CFS_InitFs( void* )
{
    return true;
}

NOINLINE bool __fastcall CFS_FileExists( void*, void*, const std::wstring& )
{
    return false;
}

NOINLINE std::string* __fastcall CFS_GetModName()
{
    return new std::string( "cstrike" );
}

void ApplyCSO2FileSystemHooks( const uintptr_t dwFsBase )
{
    const PLH::VFuncMap fsRedirects = {
        { uint16_t( 1 ), reinterpret_cast<uint64_t>( &CFS_InitFs ) },
        { uint16_t( 9 ), reinterpret_cast<uint64_t>( &CFS_FileExists ) },
        { uint16_t( 13 ), reinterpret_cast<uint64_t>( &CFS_GetModName ) },
        { uint16_t( 14 ), reinterpret_cast<uint64_t>( &CFS_GetModName ) },
    };

    uintptr_t targetVtable = dwFsBase + 0xF8008;

    g_pCFsHook = SetupVfuncSwap( &targetVtable, fsRedirects, &g_StrTblOrig );
    g_pCFsHook->hook();
}
