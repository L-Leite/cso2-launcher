#include "hooks.hpp"
#include "platform.hpp"
#include "source/tierlibs.hpp"
#include "utilities.hpp"
#include "utilities/log.hpp"

#include <gameui/igameconsole.hpp>
#include <ienginevgui.hpp>
#include <tier1/convar.hpp>

extern uintptr_t g_dwEngineBase;

void SetStaticGameConsole( void* newConsole )
{
    void** addr = reinterpret_cast<void**>( g_dwEngineBase + 0x1901C20 );
    *addr = newConsole;
}

void Con_HideConsole_f()
{
    using fn_t = void ( * )();
    static fn_t func = nullptr;

    if ( func == nullptr )
    {
        func = reinterpret_cast<fn_t>( g_dwEngineBase + 0x1C4710 );
    }

    func();
}

void Con_ShowConsole_f()
{
    using fn_t = void ( * )();
    static fn_t func = nullptr;

    if ( func == nullptr )
    {
        func = reinterpret_cast<fn_t>( g_dwEngineBase + 0x1C4740 );
    }

    func();
}

void OnToggleConsole()
{
    if ( EngineVGui()->IsConsoleVisible() )
    {
        Con_HideConsole_f();
        EngineVGui()->HideGameUI();
    }
    else
    {
        Con_ShowConsole_f();
    }
}

void FixToggleConsoleCommand()
{
    ConCommand* toggleconsole = g_pCVar->FindCommand( "toggleconsole" );
    assert( toggleconsole != nullptr );

    toggleconsole->SetVoidCallback( &OnToggleConsole );
}

static std::unique_ptr<PLH::x86Detour> g_pEngineVGuiInitHook;
static uint64_t g_EngineVGuiInitOrig = 0;

NOINLINE void __fastcall hkEngineVGuiInit( void* thisptr )
{
    auto engineFactory = Sys_GetFactory( "engine.dll" );
    LogAssert( engineFactory != nullptr );

    void* console = engineFactory( GAMECONSOLE_INTERFACE_VERSION, nullptr );
    LogAssert( console != nullptr );

    SetStaticGameConsole( console );
    FixToggleConsoleCommand();

    PLH::FnCast( g_EngineVGuiInitOrig, &hkEngineVGuiInit )( thisptr );
}

void ApplyEngineVguiHooks( const uintptr_t dwEngineBase )
{
    auto dis = HookDisassembler();

    g_pEngineVGuiInitHook =
        SetupDetourHook( dwEngineBase + 0x27A300, &hkEngineVGuiInit,
                         &g_EngineVGuiInitOrig, dis );
    g_pEngineVGuiInitHook->hook();
}
