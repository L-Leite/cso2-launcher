#include "stdafx.hpp"

#include "hooks.hpp"
#include "tier0/ICommandLine.h"

static std::unique_ptr<PLH::x86Detour> g_pInitUIHook;
static uint64_t g_InitUIOrig = NULL;

class ICSO2LoginManager
{
public:
	virtual void sub_1028465() = 0;
	virtual void sub_10286160() = 0;
	virtual bool Login(const char *UserName, const char *Password, const char *a4, const char *a5, const char *a6) = 0;
};

extern ICSO2LoginManager *g_pCSO2LoginManager;

DWORD WINAPI AutoLogin(LPVOID p)
{
	// Delay 1s after ui loaded to avoid crash
	Sleep(1000);
	
	const char* szUsername = CommandLine()->ParmValue("-username");
	const char* szPassword = CommandLine()->ParmValue("-password");

	if (szUsername && szPassword)
	{
		std::string Username(szUsername);
		std::string Password(szPassword);

		while (Username.size() > 16)
			Username.pop_back();

		while (Password.size() > 4)
			Password.pop_back();

		g_pCSO2LoginManager->Login(strdup(Username.c_str()), strdup(Password.c_str()) , "", "", "");
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

	PLH::CapstoneDisassembler dis(PLH::Mode::x86);

	g_pInitUIHook = SetupDetourHook(
		dwClientBase + 0xAE4610, &hkCSO2UIManager_InitMainUI, &g_InitUIOrig, dis);
	g_pInitUIHook->hook();
}