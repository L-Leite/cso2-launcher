#include "hooks.hpp"
#include "utilities.hpp"
#include "utilities/log.hpp"

#include <ienginevgui.hpp>

enum KeyUpTarget_t
{
    KEY_UP_ANYTARGET = 0,
    KEY_UP_ENGINE,
    KEY_UP_VGUI,
    KEY_UP_TOOLS,
    KEY_UP_CLIENT,
    KEY_UP_SCALEFORM_MENU,
    KEY_UP_SCALEFORM_HUD,
};

struct KeyInfo_t
{
    char* m_pKeyBinding;
    unsigned char m_nKeyUpTarget : 3;
    unsigned char m_bKeyDown : 1;
};

extern uintptr_t g_dwEngineBase;

KeyInfo_t* GetKeyInfo()
{
    return reinterpret_cast<KeyInfo_t*>( g_dwEngineBase + 0xAEB8A0 );
}

bool FilterTrappedKey( ButtonCode_t code, bool bDown )
{
    using fn_t = bool( __fastcall* )( ButtonCode_t, bool );
    static fn_t func = nullptr;

    if ( func == nullptr )
    {
        func = reinterpret_cast<fn_t>( g_dwEngineBase + 0xE5E50 );
    }

    return func( code, bDown );
}

inline bool ShouldPassKeyUpToTarget( ButtonCode_t code, KeyUpTarget_t target )
{
    auto keyInfo = GetKeyInfo();

    return keyInfo[code].m_nKeyUpTarget == target ||
           keyInfo[code].m_nKeyUpTarget == KEY_UP_ANYTARGET;
}

using FilterKeyFunc_t = bool ( * )( const InputEvent_t& event );

bool FilterKey( const InputEvent_t& event, KeyUpTarget_t target,
                FilterKeyFunc_t func )
{
    const bool bDown = event.m_nType != IE_ButtonReleased;
    const ButtonCode_t code = static_cast<ButtonCode_t>( event.m_nData );

    if ( !bDown && !ShouldPassKeyUpToTarget( code, target ) )
        return false;

    bool bFiltered = func( event );
    auto keyInfo = GetKeyInfo();

    if ( bDown )
    {
        if ( bFiltered )
        {
            LogAssert( keyInfo[code].m_nKeyUpTarget == KEY_UP_ANYTARGET );
            keyInfo[code].m_nKeyUpTarget = target;
        }
    }
    else
    {
        if ( keyInfo[code].m_nKeyUpTarget == target )
        {
            keyInfo[code].m_nKeyUpTarget = KEY_UP_ANYTARGET;
            bFiltered = true;
        }
        else
        {
            LogAssert( !bFiltered );
        }
    }

    return bFiltered;
}

inline bool IsModifierKeyPressed( KeyInfo_t* keyInfo )
{
    return keyInfo[KEY_LALT].m_bKeyDown == true ||
           keyInfo[KEY_LSHIFT].m_bKeyDown == true ||
           keyInfo[KEY_LCONTROL].m_bKeyDown == true ||
           keyInfo[KEY_RALT].m_bKeyDown == true ||
           keyInfo[KEY_RSHIFT].m_bKeyDown == true ||
           keyInfo[KEY_RCONTROL].m_bKeyDown == true;
}

static std::unique_ptr<PLH::x86Detour> g_pKeyEventHook;
static uint64_t g_KeyEventOrig = 0;

NOINLINE void __fastcall hkKey_Event( const InputEvent_t& event )
{
    const bool bDown = event.m_nType != IE_ButtonReleased;
    const ButtonCode_t code = static_cast<ButtonCode_t>( event.m_nData );

    auto keyInfo = GetKeyInfo();

    LogAssert( keyInfo[code].m_bKeyDown != bDown );
    if ( keyInfo[code].m_bKeyDown == bDown )
        return;

    keyInfo[code].m_bKeyDown = bDown;

    if ( FilterTrappedKey( code, bDown ) )
        return;

    EngineVGui()->UpdateButtonState( event );

    bool bConsoleVisible = EngineVGui()->IsConsoleVisible();

    auto HandleToolKey =
        reinterpret_cast<FilterKeyFunc_t>( g_dwEngineBase + 0xE5EA0 );

    if ( FilterKey( event, KEY_UP_TOOLS, HandleToolKey ) )
        return;

    auto HandleVGuiKey =
        reinterpret_cast<FilterKeyFunc_t>( g_dwEngineBase + 0xE5EE0 );

    if ( bConsoleVisible == true )
    {
        if ( FilterKey( event, KEY_UP_VGUI, HandleVGuiKey ) )
            return;
    }
    else
    {
        if ( keyInfo[KEY_BACKQUOTE].m_bKeyDown == true &&
             IsModifierKeyPressed( keyInfo ) == false )
        {
            extern void OnToggleConsole();
            OnToggleConsole();
            // keyInfo[KEY_BACKQUOTE].m_bKeyDown = false;
            keyInfo[KEY_BACKQUOTE].m_nKeyUpTarget = KEY_UP_VGUI;
            return;
        }
    }

    auto HandleScaleformMenuKey =
        reinterpret_cast<FilterKeyFunc_t>( g_dwEngineBase + 0xE5F00 );

    if ( FilterKey( event, KEY_UP_SCALEFORM_MENU, HandleScaleformMenuKey ) )
        return;

    auto HandleScaleformHudKey =
        reinterpret_cast<FilterKeyFunc_t>( g_dwEngineBase + 0xE5F20 );

    if ( FilterKey( event, KEY_UP_SCALEFORM_HUD, HandleScaleformHudKey ) )
        return;

    if ( bConsoleVisible == false )
    {
        if ( FilterKey( event, KEY_UP_VGUI, HandleVGuiKey ) )
            return;
    }

    auto HandleClientKey =
        reinterpret_cast<FilterKeyFunc_t>( g_dwEngineBase + 0xE5F40 );

    if ( FilterKey( event, KEY_UP_CLIENT, HandleClientKey ) )
        return;

    auto HandleEngineKey =
        reinterpret_cast<FilterKeyFunc_t>( g_dwEngineBase + 0xE5F90 );

    FilterKey( event, KEY_UP_ENGINE, HandleEngineKey );
}

void ApplyKeyEventsHooks( const uintptr_t dwEngineBase )
{
    auto dis = HookDisassembler();

    g_pKeyEventHook = SetupDetourHook( dwEngineBase + 0xE61B0, &hkKey_Event,
                                       &g_KeyEventOrig, dis );
    g_pKeyEventHook->hook();
}