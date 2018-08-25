#include "strtools.h"
#include "hooks.h"
#include "engine/cso2/cso2gamemanager.h"

void ConnectEngineLibraries(uintptr_t dwEngineBase)
{
	if (!g_pCSO2GameManager)
		g_pCSO2GameManager = (CSO2GameManager*)(dwEngineBase + 0xAA8D40);
}

HOOK_DETOUR_DECLARE(hkSys_SpewFunc);

NOINLINE int hkSys_SpewFunc(int spewType, char* pMsg)
{
	printf(pMsg);

	if (spewType == 3)
		assert(false);

	return HOOK_DETOUR_GET_ORIG(hkSys_SpewFunc)(spewType, pMsg);
}

typedef struct IpAddressInfo_s
{
	std::string szIpAddress;
	uint16_t uPort;
} IpAddressInfo_t;

HOOK_DETOUR_DECLARE(hkGetServerIpAddressInfo);

NOINLINE void __fastcall hkGetServerIpAddressInfo(IpAddressInfo_t* pIpAddressInfo)
{
	pIpAddressInfo->szIpAddress = "127.0.0.1";
	pIpAddressInfo->uPort = 30001;
}

void BytePatchEngine(uintptr_t dwEngineBase)
{
	// allows the player to connect without Packet_UserInfo
	uint8_t connectClientPatch[] = { 0xEB }; // jmp
	WriteProtectedMemory(connectClientPatch, (dwEngineBase + 0x69DD4));

	// disables UDP hole puncher checks
	uint8_t sendPacketPatch[] = { 0xE9, 0xF7, 0x02, 0x00, 0x00, 0x90 };	// short jmp 0x02F7; nop
	WriteProtectedMemory(sendPacketPatch, (dwEngineBase + 0x12C172));

	// skip nexon messenger login			   	
	uint8_t nmPatch[] = { 0xB0, 0x01, 0xC2, 0x08, 0x00 }; // mov al, 1; retn 8
	WriteProtectedMemory(nmPatch, (dwEngineBase + 0x289490));

	// copy the password instead of a null string
	uint8_t loginNMPatch[] = { 0x57, 0x90, 0x90, 0x90, 0x90 }; // push edi; nops
	WriteProtectedMemory(loginNMPatch, (dwEngineBase + 0x284786));

	// don't null the username string
	uint8_t loginNMPatch2[] =
	{ 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // nops
		0x8B, 0x44, 0x24, 0x64, // mov eax, [esp+64]
		0x8D, 0x4C, 0x24, 0x54, // lea ecx, [esp+54]
		0x90, 0x90, 0x90 // nops
	}; // nops
	WriteProtectedMemory(loginNMPatch2, (dwEngineBase + 0x28499D));

	uint8_t loginNMPatch3[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 }; // nops	
	// don't allow nexon messenger to ovewrite our password 				
	WriteProtectedMemory(loginNMPatch3, (dwEngineBase + 0x284A22));
	// don't get the nexon username from NM
	WriteProtectedMemory(loginNMPatch3, (dwEngineBase + 0x284A57));
}

extern DWORD WINAPI ConsoleThread(LPVOID lpArguments);

ON_LOAD_LIB(engine)
{
	uintptr_t dwEngineBase = GET_LOAD_LIB_MODULE();

	HOOK_DETOUR(dwEngineBase + 0x155C80, hkSys_SpewFunc);
	HOOK_DETOUR(dwEngineBase + 0x285FE0, hkGetServerIpAddressInfo);

	ConnectEngineLibraries(dwEngineBase);
	BytePatchEngine(dwEngineBase);

	CloseHandle(CreateThread(nullptr, NULL, ConsoleThread, nullptr, NULL, nullptr));
}

