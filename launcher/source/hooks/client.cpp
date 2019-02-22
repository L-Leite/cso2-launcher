#include "stdafx.hpp"

#include "hooks.hpp"
#include "tier0/ICommandLine.h"

// crappy struct
struct LoginParmPointer
{
	uint32_t padding[0x4]; // 0-0Fh
	char **pUsername; // 10h-13h
	uint32_t padding1[0x7]; // 14h-29h
	char **pPassword; // 30h-33h
};

struct LoginParm
{
	uint32_t padding[0x2]; // 0-7
	LoginParmPointer *values; // 8-0Bh
	uint32_t valueSize; // 0Ch-0Fh
};

uintptr_t g_pTryLogin = 0;

static std::unique_ptr<PLH::x86Detour> g_pInitUIHook;
static uint64_t g_InitUIOrig = NULL;

char userName[17] = "";
char passWord[5] = "";

/*
// Test function to make sure the LoginParm is working
void __cdecl Test(volatile DWORD values)
{
	char *username;
	char *password;

	__asm {
		push	eax
		push	ecx
		mov		ecx, [esi + 8]
		mov		eax, [ecx + 10h]
		mov		eax, [eax]
		mov		username, eax
		mov		eax, [ecx + 30h]
		mov		eax, [eax]
		mov		password, eax
		pop		ecx
		pop		eax
	}

	printf("%s %s", username, password);
}
*/

DWORD WINAPI AutoLogin(LPVOID p)
{
	// Delay 1s after ui loaded to avoid crash
	Sleep(1000);

	const char* szUsername = CommandLine()->ParmValue("-username");
	const char* szPassword = CommandLine()->ParmValue("-password");

	if (szUsername && szPassword)
	{
		auto pLoginParm = new LoginParm;
		pLoginParm->values = new LoginParmPointer;
		pLoginParm->valueSize = 2u;

		strncpy(userName, szUsername, 16);
		strncpy(passWord, szPassword, 4);

		char *pUsername = (char *)&userName;
		char *pPassword = (char *)&passWord;

		pLoginParm->values->pUsername = &pUsername;
		pLoginParm->values->pPassword = &pPassword;

		//TestGetValue((DWORD)pLoginParm);
		((void(__cdecl*)(volatile DWORD))g_pTryLogin)((DWORD)pLoginParm);
	}

	return S_OK;
}

NOINLINE bool __fastcall hkCSO2UIManager_InitMainUI(void* ecx, void* edx)
{
	CreateThread(NULL, 0, AutoLogin, 0, 0, 0);
	return PLH::FnCast(g_InitUIOrig, &hkCSO2UIManager_InitMainUI)(ecx, edx);
}

void OnClientLoaded(const uintptr_t dwClientBase)
{
	static bool bHasLoaded = false;

	if (bHasLoaded)
	{
		return;
	}

	bHasLoaded = true;

	g_pTryLogin = dwClientBase + 0xACA490;

	PLH::CapstoneDisassembler dis(PLH::Mode::x86);

	g_pInitUIHook = SetupDetourHook(
		dwClientBase + 0xAE4610, &hkCSO2UIManager_InitMainUI, &g_InitUIOrig, dis);
	g_pInitUIHook->hook();
}