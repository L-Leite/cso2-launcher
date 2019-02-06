#include "stdafx.hpp"

#include "convar.h"
#include "hooks.hpp"
#include "tier0/icommandline.h"

HOOK_DETOUR_DECLARE( hkSys_SpewFunc );

NOINLINE int hkSys_SpewFunc( int spewType, char* pMsg )
{
    std::cout << pMsg;
    return HOOK_DETOUR_GET_ORIG( hkSys_SpewFunc )( spewType, pMsg );
}

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

HOOK_DETOUR_DECLARE( hkCanCheat );

//
// Fix sv_cheats
//
ConVar* sv_cheats = 0;

NOINLINE bool __fastcall hkCanCheat()
{
    if ( !sv_cheats && g_pCVar )
        sv_cheats = g_pCVar->FindVar( "sv_cheats" );

    if ( sv_cheats->GetBool() )
        return true;

    return false;
}


void BytePatchEngine( const uintptr_t dwEngineBase )
{
    //
    // don't initialize BugTrap on engine
    //
    // jmp short 0x3C bytes forward
    const std::array<uint8_t, 5> btPatch = { 0xEB, 0x3C };
    WriteProtectedMemory( btPatch, ( dwEngineBase + 0x15877B ) );

    //
    // skip nexon messenger login
    //
    // mov al, 1; retn 8
    const std::array<uint8_t, 5> nmPatch = { 0xB0, 0x01, 0xC2, 0x08, 0x00 };
    WriteProtectedMemory( nmPatch, ( dwEngineBase + 0x289490 ) );

    //
    // copy the password instead of a null string
    //
    // push edi; nops
    const std::array<uint8_t, 5> loginNMPatch = { 0x57, 0x90, 0x90, 0x90,
                                                  0x90 };
    WriteProtectedMemory( loginNMPatch, ( dwEngineBase + 0x284786 ) );

    //
    // don't null the username string
    //
    const std::array<uint8_t, 20> loginNMPatch2 = {
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,  // nops
        0x8B, 0x44, 0x24, 0x64,  // mov eax, [esp+64]
        0x8D, 0x4C, 0x24, 0x54,  // lea ecx, [esp+54]
        0x90, 0x90, 0x90         // nops
    };
    WriteProtectedMemory( loginNMPatch2, ( dwEngineBase + 0x28499D ) );

    //
    // don't allow nexon messenger to ovewrite our password
    //
    // nops
    const std::array<uint8_t, 10> loginNMPatch3 = {
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90
    };
    WriteProtectedMemory( loginNMPatch3, ( dwEngineBase + 0x284A22 ) );

    //
    // don't get the nexon username from NM
    //
    WriteProtectedMemory( loginNMPatch3, ( dwEngineBase + 0x284A57 ) );

    //
    // reenable UDP info packet
    //
    const std::array<uint8_t, 6> netPacketPatch = {
        0x89, 0xB0, 0x28, 0x01, 0x00, 0x00
    };  // mov [eax+128h], esi
    WriteProtectedMemory( netPacketPatch, ( dwEngineBase + 0x283604 ) );

    //
    // force direct UDP connection instead of relay connection
    //
    // mov dword ptr [eax], 2
    const std::array<uint8_t, 6> relayPatch = { 0xC7, 0x00, 0x02,
                                                0x00, 0x00, 0x00 };
    WriteProtectedMemory( relayPatch, ( dwEngineBase + 0x2BE552 ) );
    // mov dword ptr [eax+8], 2
    const std::array<uint8_t, 7> relayPatch2 = { 0xC7, 0x40, 0x08, 0x02,
                                                 0x00, 0x00, 0x00 };
    WriteProtectedMemory( relayPatch2, ( dwEngineBase + 0x2BE56C ) );
    // mov dword ptr [eax+4], 2
    const std::array<uint8_t, 7> relayPatch3 = { 0xC7, 0x40, 0x04, 0x02,
                                                 0x00, 0x00, 0x00 };
    WriteProtectedMemory( relayPatch3, ( dwEngineBase + 0x2BE587 ) );

    //
    // don't send the filesystem hash
    // stops the weird blinking when you login,
    // but you don't get any client hash in the master server
    //
	// nops
    const std::array<uint8_t, 11> hashGenPatch = {
        0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
    };
    WriteProtectedMemory( hashGenPatch, ( dwEngineBase + 0x2BC50D ) );

	//
	// always return true when checking if the user is over 18
	//
	// mov al, 01
	// ret
	const std::array<uint8_t, 11> isAdultPatch = {
        0xB0, 0x01, 0xC3
    };
    WriteProtectedMemory( isAdultPatch, ( dwEngineBase + 0x288FF0 ) );
}

void ConsoleThread();

ON_LOAD_LIB( engine )
{
    const uintptr_t dwEngineBase = GET_LOAD_LIB_MODULE();
    BytePatchEngine( dwEngineBase );

    HOOK_DETOUR( dwEngineBase + 0x155C80, hkSys_SpewFunc );
    HOOK_DETOUR( dwEngineBase + 0x285FE0, hkGetServerIpAddressInfo );
    HOOK_DETOUR( dwEngineBase + 0xCE8B0, hkCanCheat );

	std::thread threadObj( ConsoleThread );
    threadObj.detach();
}
