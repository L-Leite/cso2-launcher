#include "strtools.h"
#include "hooks.hpp"
#include "onloadlib.hpp"

HOOK_DETOUR_DECLARE( hkLoadLibraryExA );

NOINLINE HMODULE WINAPI hkLoadLibraryExA( LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags )
{
	HMODULE hLoadedModule = HOOK_DETOUR_GET_ORIG( hkLoadLibraryExA )(lpLibFileName, hFile, dwFlags);
	CLoadLibCallbacks::OnLoadLibrary( V_GetFileName( lpLibFileName ), reinterpret_cast<uintptr_t>(hLoadedModule) );
	return hLoadedModule;
}

void HookWinapi()
{
	HOOK_DETOUR( LoadLibraryExA, hkLoadLibraryExA );
}
