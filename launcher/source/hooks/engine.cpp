#include "stdafx.hpp"

#include "color.h"
#include "convar.h"
#include "tier0/icommandline.h"

#include "console.hpp"
#include "hooks.hpp"
#include "tierextra.hpp"

struct IpAddressInfo
{
    std::string szIpAddress;
    uint16_t iPort;
};

HOOK_DETOUR_DECLARE( hkGetServerIpAddressInfo );

//
// Allows the user to choose a specific master server's IP address and/or port
// number through command line arguments. Defaults to 127.0.0.1:30001 if no
// arguments are given Usage: "-masterip [desired master IP address]" and/or
// "-masterport [desired master port number]"
//
NOINLINE void __fastcall hkGetServerIpAddressInfo( IpAddressInfo& info )
{
    const char* szMasterIp = CommandLine()->ParmValue( "-masterip" );
    const char* szMasterPort = CommandLine()->ParmValue( "-masterport" );

    info.szIpAddress = szMasterIp ? szMasterIp : "127.0.0.1"s;
    info.iPort =
        szMasterPort ? static_cast<uint16_t>( atoi( szMasterPort ) ) : 30001;
}

//
// Restore original CanCheat check
// This fixes the use of sv_cheats as a host
// Credit goes to GEEKiDoS
//
ConVar* sv_cheats = nullptr;

HOOK_DETOUR_DECLARE( hkCanCheat );

NOINLINE bool __fastcall hkCanCheat()
{
    assert( g_pCVar != nullptr );  // this should be already available

    if ( !sv_cheats )
    {
        sv_cheats = g_pCVar->FindVar( "sv_cheats" );
        assert( sv_cheats != nullptr );  // this should always succeed
    }

    return sv_cheats->GetBool();
}

static std::unique_ptr<PLH::x86Detour> g_pColorPrint;
static uint64_t g_ColorPrintOrig = NULL;

NOINLINE void __fastcall hkCon_ColorPrint( Color& clr, const char* msg )
{
    if ( clr.r() > clr.g() + clr.b() )  // Error
    {
        g_GameConsole.Error( msg );
    }
    else if ( clr.r() + clr.g() > clr.g() + clr.b() )  // Warn
    {
        g_GameConsole.Warning( msg );
    }
    else if ( clr.g() > clr.r() + clr.b() )  // Green message idk
    {
        g_GameConsole.DevInfo( msg );
    }
    else if ( clr.g() < 255 && clr.r() < 255 && clr.b() < 255 )  // Grey
    {
        g_GameConsole.WriteLine( "^6%s", msg );
    }
    else
    {
        g_GameConsole.WriteLine( msg );
    }

    return PLH::FnCast( g_ColorPrintOrig, &hkCon_ColorPrint )( clr, msg );
}

HOOK_DETOUR_DECLARE( hkHLEngineWindowProc );

NOINLINE LRESULT WINAPI hkHLEngineWindowProc( HWND hWnd, UINT Msg,
                                              WPARAM wParam, LPARAM lParam )
{	   
    bool conRes = g_GameConsole.OnWindowCallback( hWnd, Msg, wParam, lParam );

    if ( !conRes )
        return NULL;

	return HOOK_DETOUR_GET_ORIG( hkHLEngineWindowProc )( hWnd, Msg, wParam,
                                                         lParam );
}

void BytePatchEngine( const uintptr_t dwEngineBase )
{
    //
    // don't initialize BugTrap on engine
    //
    // jmp short 0x3C bytes forward
    const std::array<uint8_t, 5> btPatch = { 0xEB, 0x3C };
    utils::WriteProtectedMemory( btPatch, dwEngineBase + 0x15877B );

    //
    // skip nexon messenger login
    //
    // mov al, 1; retn 8
    const std::array<uint8_t, 5> nmPatch = { 0xB0, 0x01, 0xC2, 0x08, 0x00 };
    utils::WriteProtectedMemory( nmPatch, dwEngineBase + 0x289490 );

    //
    // copy the password instead of a null string
    //
    // push edi; nops
    const std::array<uint8_t, 5> loginNMPatch = { 0x57, 0x90, 0x90, 0x90,
                                                  0x90 };
    utils::WriteProtectedMemory( loginNMPatch, dwEngineBase + 0x284786 );

    //
    // don't null the username string
    //
    const std::array<uint8_t, 20> loginNMPatch2 = {
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,  // nops
        0x8B, 0x44, 0x24, 0x64,  // mov eax, [esp+64]
        0x8D, 0x4C, 0x24, 0x54,  // lea ecx, [esp+54]
        0x90, 0x90, 0x90         // nops
    };
    utils::WriteProtectedMemory( loginNMPatch2, dwEngineBase + 0x28499D );

    //
    // don't allow nexon messenger to ovewrite our password
    //
    // nops
    const std::array<uint8_t, 10> loginNMPatch3 = {
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
    };
    utils::WriteProtectedMemory( loginNMPatch3, dwEngineBase + 0x284A22 );

    //
    // don't get the nexon username from NM
    //
    utils::WriteProtectedMemory( loginNMPatch3, dwEngineBase + 0x284A57 );

    //
    // reenable UDP info packet
    //
    const std::array<uint8_t, 6> netPacketPatch = {
        0x89, 0xB0, 0x28, 0x01, 0x00, 0x00
    };  // mov [eax+128h], esi
    utils::WriteProtectedMemory( netPacketPatch, dwEngineBase + 0x283604 );

    //
    // force direct UDP connection instead of relay connection
    //
    // mov dword ptr [eax], 2
    const std::array<uint8_t, 6> relayPatch = { 0xC7, 0x00, 0x02,
                                                0x00, 0x00, 0x00 };
    utils::WriteProtectedMemory( relayPatch, dwEngineBase + 0x2BE552 );
    // mov dword ptr [eax+8], 2
    const std::array<uint8_t, 7> relayPatch2 = { 0xC7, 0x40, 0x08, 0x02,
                                                 0x00, 0x00, 0x00 };
    utils::WriteProtectedMemory( relayPatch2, dwEngineBase + 0x2BE56C );
    // mov dword ptr [eax+4], 2
    const std::array<uint8_t, 7> relayPatch3 = { 0xC7, 0x40, 0x04, 0x02,
                                                 0x00, 0x00, 0x00 };
    utils::WriteProtectedMemory( relayPatch3, dwEngineBase + 0x2BE587 );

    //
    // don't send the filesystem hash
    // stops the weird blinking when you login,
    // but you don't get any client hash in the master server
    //
    // nops
    const std::array<uint8_t, 11> hashGenPatch = {
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
    };
    utils::WriteProtectedMemory( hashGenPatch, dwEngineBase + 0x2BC50D );

    //
    // always return true when checking if the user is over 18
    //
    // mov al, 01
    // ret
    const std::array<uint8_t, 11> isAdultPatch = { 0xB0, 0x01, 0xC3 };
    utils::WriteProtectedMemory( isAdultPatch, dwEngineBase + 0x288FF0 );

    //
    // Patch CanCheat traps
    //
    // nops
    const std::array<uint8_t, 6> canCheatPatch1 = { 0x90, 0x90, 0x90,
                                                    0x90, 0x90, 0x90 };
    utils::WriteProtectedMemory( canCheatPatch1, dwEngineBase + 0x1EFEF6 );

    // jmp
    const std::array<uint8_t, 1> canCheatPatch2 = { 0xEB };
    utils::WriteProtectedMemory( canCheatPatch2, dwEngineBase + 0x19F4D2 );
}

void OnEngineLoaded( const uintptr_t dwEngineBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

    // setup engine interfaces
    CreateInterfaceFn pEngineFactory = Sys_GetFactory( "engine.dll" );
    ConnectExtraLibraries( &pEngineFactory, 1 );

    BytePatchEngine( dwEngineBase );

    PLH::CapstoneDisassembler hookDisasm( PLH::Mode::x86 );

    HOOK_DETOUR( dwEngineBase + 0x285FE0, hkGetServerIpAddressInfo );
    HOOK_DETOUR( dwEngineBase + 0xCE8B0, hkCanCheat );
    HOOK_DETOUR( dwEngineBase + 0x15EAF0, hkHLEngineWindowProc );

    g_pColorPrint = SetupDetourHook( dwEngineBase + 0x1C4B40, &hkCon_ColorPrint,
                                     &g_ColorPrintOrig, hookDisasm );
    g_pColorPrint->hook();
}
