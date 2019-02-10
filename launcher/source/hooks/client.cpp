#include "convar.h"
#include "hooks.hpp"

extern bool g_bRenderStarted;

HOOK_DETOUR_DECLARE( hkCSO2UIManager_InitMainUI );
NOINLINE bool __fastcall hkCSO2UIManager_InitMainUI( const char** ecx,
                                                     void* edx )
{
	// Render console before UI loaded will stuck whole game IDK why
    g_bRenderStarted = true;
    return HOOK_DETOUR_GET_ORIG( hkCSO2UIManager_InitMainUI )( ecx, edx );
}

void OnClientLoaded( const uintptr_t dwClientBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

	HOOK_DETOUR( dwClientBase + 0xAE4610, hkCSO2UIManager_InitMainUI );
}
