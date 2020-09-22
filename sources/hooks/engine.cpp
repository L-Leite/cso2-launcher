#include <array>
#include <cstdint>
#include <iostream>
#include <string>

#include <math/color.hpp>
#include <tier0/icommandline.hpp>
#include <tier1/convar.hpp>

#include "hooks.hpp"
#include "platform.hpp"
#include "utilities.hpp"

#include "source/tierlibs.hpp"

using namespace std::string_literals;

struct IpAddressInfo
{
    std::string szIpAddress;
    uint16_t iPort;
};

uintptr_t g_dwEngineBase = 0;

static std::unique_ptr<PLH::x86Detour> g_pServerAddrHook;
static uint64_t g_ServerAddrOrig = 0;

//
// Allows the user to choose a specific master server's IP address and/or port
// number through command line arguments. Defaults to 127.0.0.1:30001 if no
// arguments are given Usage: "-masterip [desired master IP address]" and/or
// "-masterport [desired master port number]"
//
NOINLINE void __fastcall hkGetServerInfo( IpAddressInfo& info )
{
    const char* szMasterIp =
        CommandLine()->ParmValue( "-masterip", "127.0.0.1" );
    const int iMasterPort = CommandLine()->ParmValue( "-masterport", 30001 );

    std::cout << "GetServerInfo - IP: " << szMasterIp
              << " port: " << iMasterPort << "\n";

    info.szIpAddress = szMasterIp;
    info.iPort = iMasterPort;
}

//
// Restore original CanCheat check
// This fixes the use of sv_cheats as a host
// Credit goes to GEEKiDoS
//
ConVar* sv_cheats = nullptr;

static std::unique_ptr<PLH::x86Detour> g_pCanCheatHook;
static uint64_t g_CanCheatOrig = 0;

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
    // don't clear password string
    // TODO: this is DANGEROUS! find a better way to fix this!
    //
    // nops
    const std::array<uint8_t, 14> loginNMPatch3 = { 0x90, 0x90, 0x90, 0x90,
                                                    0x90, 0x90, 0x90, 0x90,
                                                    0x90, 0x90, 0x90, 0x90,
                                                    0x90, 0x90 };
    utils::WriteProtectedMemory( loginNMPatch3, dwEngineBase + 0x2849CB );

    //
    // don't allow nexon messenger to ovewrite our password
    //
    // nops
    const std::array<uint8_t, 10> loginNMPatch4 = { 0x90, 0x90, 0x90, 0x90,
                                                    0x90, 0x90, 0x90, 0x90,
                                                    0x90, 0x90 };
    utils::WriteProtectedMemory( loginNMPatch4, dwEngineBase + 0x284A22 );

    //
    // don't get the nexon username from NM
    //
    utils::WriteProtectedMemory( loginNMPatch4, dwEngineBase + 0x284A57 );

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
    const std::array<uint8_t, 11> isAdultPatch = {
        0xB0,
        0x01,  // mov al, 01
        0xC3   // ret
    };
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

    //
    // load plain text weapon description files
    //
    if ( CommandLine()->CheckParm( "-unpackedfiles", nullptr ) )
    {
        // "txt\0" string
        const std::array<uint8_t, 4> unpFilesPatch = { 0x74, 0x78, 0x74, 0x00 };
        utils::WriteProtectedMemory( unpFilesPatch, dwEngineBase + 0x61A2AE );
    }

    //
    // http logger that tries to connect to http://cso2dn.nexon.com
    // it no longer works and it slows down startup times when running under
    // wine
    //
    // retn
    const std::array<uint8_t, 1> httpLoggerPatch = { 0xC3 };
    utils::WriteProtectedMemory( httpLoggerPatch, dwEngineBase + 0x108200 );
}

void ApplyHooksEngine( const uintptr_t dwEngineBase )
{
    PLH::CapstoneDisassembler dis( PLH::Mode::x86 );

    g_pServerAddrHook = SetupDetourHook(
        dwEngineBase + 0x285FE0, &hkGetServerInfo, &g_ServerAddrOrig, dis );
    g_pServerAddrHook->hook();

    g_pCanCheatHook = SetupDetourHook( dwEngineBase + 0xCE8B0, &hkCanCheat,
                                       &g_CanCheatOrig, dis );
    g_pCanCheatHook->hook();
}

extern void ApplyEngineVguiHooks( const uintptr_t dwEngineBase );
extern void ApplyKeyEventsHooks( const uintptr_t dwEngineBase );

void OnEngineLoaded( const uintptr_t dwEngineBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

    g_dwEngineBase = dwEngineBase;

    BytePatchEngine( dwEngineBase );
    ApplyHooksEngine( dwEngineBase );

    ApplyEngineVguiHooks( dwEngineBase );
    ApplyKeyEventsHooks( dwEngineBase );
}
