#include <array>
#include <cstdint>
#include <iostream>
#include <string>

#include <math/color.hpp>
#include <tier0/icommandline.hpp>
#include <tier1/convar.hpp>

#include "console.hpp"
#include "hooks.hpp"
#include "utilities.hpp"
#include "utilities/log.hpp"
#include "utilities/memorypatterns.hpp"

#include "source/tierlibs.hpp"

using namespace std::string_literals;

struct IpAddressInfo
{
    std::string szIpAddress;
    uint16_t iPort;
};

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

static std::unique_ptr<PLH::x86Detour> g_pColorPrintHook;
static uint64_t g_ColorPrintOrig = 0;

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

static std::unique_ptr<PLH::x86Detour> g_pEngineWinHook;
static uint64_t g_EngineWinOrig = 0;

NOINLINE LRESULT WINAPI hkHLEngineWindowProc( HWND hWnd, UINT Msg,
                                              WPARAM wParam, LPARAM lParam )
{
    const bool conRes =
        g_GameConsole.OnWindowCallback( hWnd, Msg, wParam, lParam );

    if ( !conRes )
        return 0;

    return PLH::FnCast( g_EngineWinOrig, &hkHLEngineWindowProc )(
        hWnd, Msg, wParam, lParam );
}

inline bool LookupNMLoginAddress( MemoryPatterns& patterns )
{
    // sig used in v24406 korea
    bool found = patterns.AddPattern(
        "\x56\x57\x8B\xF9\xFF\x15\xCC\xCC\xCC\xCC\x8B\xC8", "NMLogin",
        IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    if ( found == false )
    {
        // sig used in china 21 feb 2017
        found = patterns.AddPattern(
            "\x56\x57\x8B\xF1\xE8\xCC\xCC\xCC\xCC\x83\xEC\x10", "NMLogin",
            IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );
    }

    return found;
}

bool LookupEngineAddresses()
{
    Log::Debug( "Looking up addresses in engine.dll...\n" );

    int results = 0;

    MemoryPatterns& patterns = MemoryPatterns::Singleton();

    results += !patterns.AddPattern(
        "\x75\x3C\xFF\x15", "BugTrapInit",
        IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    results += !LookupNMLoginAddress( patterns );

    // should only exist in korean version
    results += !patterns.AddPattern(
        "\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\xC6\x84\x24\xCC\xCC\xCC\xCC"
        "\xCC\x8D\x8C\x24\xCC\xCC\xCC\xCC\xFF\x75\x10",
        "NMCopyPw", IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    // should only exist in korean version
    results += !patterns.AddPattern(
        "\x50\x6A\xCC\x51\xE8\xCC\xCC\xCC\xCC\x8B\x44\x24\x64", "NMClearUser",
        IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    // should only exist in korean version
    results += !patterns.AddPattern(
        "\x2B\xC1\x50\x6A\xCC\x51\xE8\xCC\xCC\xCC\xCC\x83\xC4\x0C", "NMClearPw",
        IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    // should only exist in korean version
    results += !patterns.AddPattern(
        "\x50\x8D\x4C\x24\x18\xE8\xCC\xCC\xCC\xCC\xC6\x84\x24\xCC\xCC\xCC\xCC"
        "\xCC\x8D\x4C\x24\x2C",
        "NMOverwritePw", IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    // should only exist in korean version
    results += !patterns.AddPattern(
        "\x8D\x4C\x24\x60\x50\xE8", "NMOverwriteUser",
        IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    results += !patterns.AddPattern(
        "\x89\xB0\x6C\x01\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x8B\xF0",
        "MsgHandlerUdpInit",
        IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    results += !patterns.AddPattern(
        "\xC7\xCC\xCC\xCC\xCC\xCC\x8B\x46\x14", "UdpHpAddRelay",
        IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    results += !patterns.AddPattern(
        "\x68\xCC\xCC\xCC\xCC\x8B\x01\x52\xFF\x50\x58", "LoginFsHash",
        IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    // TODO: find this in a better way or emulate the NexonManager library
    // (nmcogame.dll)
    results += !patterns.AddPattern(
        "\x55\x8B\xEC\x83\xE4\xF8\x6A\xFF\x68\xCC\xCC\xCC\xCC\x64\xA1\xCC\xCC"
        "\xCC\xCC\x50\x64\x89\x25\xCC\xCC\xCC\xCC\x51\x85\x24\x24\xB8\xCC\xCC"
        "\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x8D\x44\x24\x08\x50\xE8\xCC\xCC\xCC\xCC"
        "\x8B\xC8\xE8\xCC\xCC\xCC\xCC\xC7\x84\x24",
        "NexonManagerIsAdult",
        IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    results += !patterns.AddPattern(
        "\x0F\x85\xCC\xCC\xCC\xCC\x5E\xE9", "CanCheatPatch1",
        IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    results += !patterns.AddPattern(
        "\x74\x07\xB1\x01\xE9", "CanCheatPatch2",
        IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    results += !patterns.AddPattern(
        "\x8D\x4C\x24\xCC\xE8\xCC\xCC\xCC\xCC\x83\x7C\x24\xCC\xCC\x8D\x44\x24"
        "\xCC\x0F\x43\x44\xCC\xCC\x50\xFF\x15\xCC\xCC\xCC\xCC\xFF",
        "GetServerInfo",
        IMemoryPatternsOptions( -1, 5, 9, "engine.dll", 0, true ) );

    results += !patterns.AddPattern(
        "\x33\xC0\x83\x3D\xCC\xCC\xCC\xCC\xCC\x7C\x0B", "CanCheat",
        IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    results += !patterns.AddPattern(
        "\x68\xCC\xCC\xCC\xCC\x6A\xFC", "HLEngineWindowProc",
        IMemoryPatternsOptions( -1, 1, -1, "engine.dll" ) );

    results += !patterns.AddPattern(
        "\xB8\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x80\x3D\xCC\xCC\xCC\xCC\xCC"
        "\x55\x56",
        "Con_ColorPrint", IMemoryPatternsOptions( -1, -1, -1, "engine.dll" ) );

    const bool foundAllAddresses = results == 0;

    if ( foundAllAddresses == true )
    {
        Log::Debug( "Looked up addresses in engine.dll successfully\n" );
    }
    else
    {
        Log::Error( "Failed to find {} engine.dll addresses\n", results );
    }

    return foundAllAddresses;
}

void BytePatchEngine()
{
    MemoryPatterns& patterns = MemoryPatterns::Singleton();

    //
    // don't initialize BugTrap on engine
    //
    const uintptr_t btInitAddr = patterns.GetPattern( "BugTrapInit" );

    if ( btInitAddr != 0 )
    {
        // jmp short 0x3C bytes forward
        const std::array<uint8_t, 5> btPatch = { 0xEB, 0x3C };
        utils::WriteProtectedMemory( btPatch, btInitAddr );
    }

    //
    // skip nexon messenger login
    //
    const uintptr_t nmLoginAddr = patterns.GetPattern( "NMLogin" );

    if ( nmLoginAddr != 0 )
    {
        // mov al, 1; retn 8
        const std::array<uint8_t, 5> nmPatch = { 0xB0, 0x01, 0xC2, 0x08, 0x00 };
        utils::WriteProtectedMemory( nmPatch, nmLoginAddr );
    }

    //
    // copy the password instead of a null string
    //
    const uintptr_t nmCopyPwAddr = patterns.GetPattern( "NMCopyPw" );

    if ( nmCopyPwAddr != 0 )
    {
        // push edi; nops
        const std::array<uint8_t, 5> loginNMPatch = { 0x57, 0x90, 0x90, 0x90,
                                                      0x90 };
        utils::WriteProtectedMemory( loginNMPatch, nmCopyPwAddr );
    }

    //
    // don't null the username string
    //
    const uintptr_t nmClearUserAddr = patterns.GetPattern( "NMClearUser" );

    if ( nmClearUserAddr != 0 )
    {
        const std::array<uint8_t, 20> loginNMPatch2 = {
            0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,  // nops
            0x8B, 0x44, 0x24, 0x64,  // mov eax, [esp+64]
            0x8D, 0x4C, 0x24, 0x54,  // lea ecx, [esp+54]
            0x90, 0x90, 0x90         // nops
        };
        utils::WriteProtectedMemory( loginNMPatch2, nmClearUserAddr );
    }

    //
    // don't clear password string
    // TODO: this is DANGEROUS! find a better way to fix this!
    //
    const uintptr_t nmClearPwAddr = patterns.GetPattern( "NMClearPw" );

    if ( nmClearPwAddr != 0 )
    {
        // nops
        const std::array<uint8_t, 14> loginNMPatch3 = { 0x90, 0x90, 0x90, 0x90,
                                                        0x90, 0x90, 0x90, 0x90,
                                                        0x90, 0x90, 0x90, 0x90,
                                                        0x90, 0x90 };
        utils::WriteProtectedMemory( loginNMPatch3, nmClearPwAddr );
    }

    //
    // don't allow nexon messenger to ovewrite our password
    //
    const uintptr_t nmOverwritePwAddr = patterns.GetPattern( "NMOverwritePw" );

    if ( nmOverwritePwAddr != 0 )
    {
        // nops
        const std::array<uint8_t, 10> loginNMPatch4 = { 0x90, 0x90, 0x90, 0x90,
                                                        0x90, 0x90, 0x90, 0x90,
                                                        0x90, 0x90 };
        utils::WriteProtectedMemory( loginNMPatch4, nmOverwritePwAddr );
    }

    //
    // don't get the nexon username from NM
    //
    const uintptr_t nmOWriteUserAddr = patterns.GetPattern( "NMOverwriteUser" );

    if ( nmOWriteUserAddr != 0 )
    {
        const std::array<uint8_t, 10> loginNMPatch4 = { 0x90, 0x90, 0x90, 0x90,
                                                        0x90, 0x90, 0x90, 0x90,
                                                        0x90, 0x90 };
        utils::WriteProtectedMemory( loginNMPatch4, nmOWriteUserAddr );
    }

    //
    // reenable UDP info packet
    //
    const uintptr_t msgUdpInitAddr = patterns.GetPattern( "MsgHandlerUdpInit" );

    if ( msgUdpInitAddr != 0 )
    {
        const std::array<uint8_t, 6> netPacketPatch = {
            0x89, 0xB0, 0x28, 0x01, 0x00, 0x00
        };  // mov [eax+128h], esi
        utils::WriteProtectedMemory( netPacketPatch, msgUdpInitAddr );
    }

    //
    // force direct UDP connection instead of relay connection
    //
    const uintptr_t addRelayAddr = patterns.GetPattern( "UdpHpAddRelay" );

    if ( addRelayAddr != 0 )
    {
        // mov dword ptr [eax], 2
        const std::array<uint8_t, 6> relayPatch = { 0xC7, 0x00, 0x02,
                                                    0x00, 0x00, 0x00 };
        utils::WriteProtectedMemory( relayPatch, addRelayAddr );
        // mov dword ptr [eax+8], 2
        const std::array<uint8_t, 7> relayPatch2 = { 0xC7, 0x40, 0x08, 0x02,
                                                     0x00, 0x00, 0x00 };
        utils::WriteProtectedMemory( relayPatch2, addRelayAddr + 0x1A );
        // mov dword ptr [eax+4], 2
        const std::array<uint8_t, 7> relayPatch3 = { 0xC7, 0x40, 0x04, 0x02,
                                                     0x00, 0x00, 0x00 };
        utils::WriteProtectedMemory( relayPatch3, addRelayAddr + 0x35 );
    }

    //
    // don't send the filesystem hash
    // stops the weird blinking when you login,
    // but you don't get any client hash in the master server
    //
    const uintptr_t loginFsHashAddr = patterns.GetPattern( "LoginFsHash" );

    if ( loginFsHashAddr != 0 )
    {
        // nops
        const std::array<uint8_t, 11> hashGenPatch = {
            0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90,
        };
        utils::WriteProtectedMemory( hashGenPatch, loginFsHashAddr );
    }

    //
    // always return true when checking if the user is over 18
    //
    const uintptr_t nmIsAdultAddr =
        patterns.GetPattern( "NexonManagerIsAdult" );

    if ( nmIsAdultAddr != 0 )
    {
        const std::array<uint8_t, 11> isAdultPatch = {
            0xB0,
            0x01,  // mov al, 01
            0xC3   // ret
        };
        utils::WriteProtectedMemory( isAdultPatch, nmIsAdultAddr );
    }

    //
    // Patch CanCheat traps
    //
    const uintptr_t canCheatOneAddr = patterns.GetPattern( "CanCheatPatch1" );

    if ( canCheatOneAddr != 0 )
    {
        // nops
        const std::array<uint8_t, 6> canCheatPatch1 = { 0x90, 0x90, 0x90,
                                                        0x90, 0x90, 0x90 };
        utils::WriteProtectedMemory( canCheatPatch1, canCheatOneAddr );
    }

    const uintptr_t canCheatTwoAddr = patterns.GetPattern( "CanCheatPatch2" );

    if ( canCheatTwoAddr != 0 )
    {
        // jmp
        const std::array<uint8_t, 1> canCheatPatch2 = { 0xEB };
        utils::WriteProtectedMemory( canCheatPatch2, canCheatTwoAddr );
    }

    //
    // load plain text weapon description files
    //
    if ( CommandLine()->CheckParm( "-unpackedfiles", nullptr ) )
    {
        const uintptr_t wpnFilesAddr =
            patterns.GetPattern( "ResourceMgrWpnFiles" );

        if ( wpnFilesAddr != 0 )
        {
            // "txt\0" string
            const std::array<uint8_t, 4> patch = { 0x74, 0x78, 0x74, 0x00 };
            utils::WriteProtectedMemory( patch, wpnFilesAddr );
        }
    }

    //
    // http logger that tries to connect to http://cso2dn.nexon.com
    // it no longer works and slows down startup times when running under wine
    //
    const uintptr_t httpLoggerAddr =
        patterns.GetPattern( "HttpStatsLoggerInit" );

    if ( httpLoggerAddr != 0 )
    {
        // retn
        const std::array<uint8_t, 1> patch = { 0xC3 };
        utils::WriteProtectedMemory( patch, httpLoggerAddr );
    }
}

void ApplyHooksEngine()
{
    MemoryPatterns& patterns = MemoryPatterns::Singleton();

    PLH::CapstoneDisassembler dis( PLH::Mode::x86 );

    const uintptr_t serverInfoAddr = patterns.GetPattern( "GetServerInfo" );

    if ( serverInfoAddr != 0 )
    {
        g_pServerAddrHook = SetupDetourHook( serverInfoAddr, &hkGetServerInfo,
                                             &g_ServerAddrOrig, dis );
        g_pServerAddrHook->hook();
    }

    const uintptr_t canCheatAddr = patterns.GetPattern( "CanCheat" );

    if ( canCheatAddr != 0 )
    {
        g_pCanCheatHook =
            SetupDetourHook( canCheatAddr, &hkCanCheat, &g_CanCheatOrig, dis );
        g_pCanCheatHook->hook();
    }

    const uintptr_t winProcAddr = patterns.GetPattern( "HLEngineWindowProc" );

    if ( winProcAddr != 0 )
    {
        g_pEngineWinHook = SetupDetourHook( winProcAddr, &hkHLEngineWindowProc,
                                            &g_EngineWinOrig, dis );
        g_pEngineWinHook->hook();
    }

    const uintptr_t conColPrintAddr = patterns.GetPattern( "Con_ColorPrint" );

    if ( conColPrintAddr != 0 )
    {
        g_pColorPrintHook = SetupDetourHook( conColPrintAddr, &hkCon_ColorPrint,
                                             &g_ColorPrintOrig, dis );
        g_pColorPrintHook->hook();
    }
}

void OnEngineLoaded( const uintptr_t dwEngineBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

    MemoryPatterns& patterns = MemoryPatterns::Singleton();
    patterns.SetPattern( "ResourceMgrWpnFiles", dwEngineBase + 0x61A2AE );
    patterns.SetPattern( "HttpStatsLoggerInit", dwEngineBase + 0x108200 );

    LookupEngineAddresses();
    BytePatchEngine();
    ApplyHooksEngine();
}
