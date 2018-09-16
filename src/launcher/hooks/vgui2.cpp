#include "strtools.h"
#include "hooks.h"
#include "tier0/memdbgon.h"

extern bool g_bEnableLocalization;

HOOK_DETOUR_DECLARE( hkStrTblFind );

// CLocalizedStringTable::Find
NOINLINE wchar_t* __fastcall hkStrTblFind( void* ecx, void* edx, const char* pName )
{
	static wchar_t szBuffer[1024];

	if (!g_bEnableLocalization)
	{
		size_t nameLength = strlen( pName );
		MultiByteToWideChar( CP_ACP, NULL, pName, strlen( pName ), szBuffer, nameLength + 1 );
		szBuffer[nameLength] = '\0';
		return szBuffer;
	}

	return HOOK_DETOUR_GET_ORIG( hkStrTblFind )(ecx, edx, pName);
}

ON_LOAD_LIB( vgui2 )
{
	uintptr_t dwVguiBase = GET_LOAD_LIB_MODULE();
	HOOK_DETOUR( dwVguiBase + 0xAC80, hkStrTblFind );
}
