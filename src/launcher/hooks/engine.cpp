#include <array>	

#include "hooks.h"		   

HOOK_DETOUR_DECLARE( hkSys_SpewFunc );

NOINLINE int hkSys_SpewFunc( int spewType, char* pMsg )
{
	std::cout << pMsg;
	return HOOK_DETOUR_GET_ORIG( hkSys_SpewFunc )(spewType, pMsg);
}

struct IpAddressInfo
{
	std::string szIpAddress;
	uint16_t uPort;
};

HOOK_DETOUR_DECLARE( hkGetServerIpAddressInfo );

NOINLINE void __fastcall hkGetServerIpAddressInfo( IpAddressInfo* pIpAddressInfo )
{
	pIpAddressInfo->szIpAddress = "127.0.0.1";
	pIpAddressInfo->uPort = 30001;
}

void BytePatchEngine( const uintptr_t dwEngineBase )
{
	//
	// don't initialize BugTrap on engine
	//
	// jmp short 0x3C bytes forward
	const std::array<uint8_t, 5> btPatch = { 0xEB, 0x3C };
	WriteProtectedMemory( btPatch, (dwEngineBase + 0x15877B) );

	//
	// skip nexon messenger login	
	//
	// mov al, 1; retn 8
	const std::array<uint8_t, 5> nmPatch = { 0xB0, 0x01, 0xC2, 0x08, 0x00 };
	WriteProtectedMemory( nmPatch, (dwEngineBase + 0x289490) );

	//
	// copy the password instead of a null string
	//
	// push edi; nops
	const std::array<uint8_t, 5> loginNMPatch = { 0x57, 0x90, 0x90, 0x90, 0x90 };
	WriteProtectedMemory( loginNMPatch, (dwEngineBase + 0x284786) );

	//
	// don't null the username string
	//
	const std::array<uint8_t, 20> loginNMPatch2 =
	{
		0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // nops
		0x8B, 0x44, 0x24, 0x64, // mov eax, [esp+64]
		0x8D, 0x4C, 0x24, 0x54, // lea ecx, [esp+54]
		0x90, 0x90, 0x90 // nops
	};
	WriteProtectedMemory( loginNMPatch2, (dwEngineBase + 0x28499D) );

	//
	// don't allow nexon messenger to ovewrite our password 		
	//
	// nops	
	const std::array<uint8_t, 10> loginNMPatch3 = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
	WriteProtectedMemory( loginNMPatch3, (dwEngineBase + 0x284A22) );
	//
	// don't get the nexon username from NM
	//
	WriteProtectedMemory( loginNMPatch3, (dwEngineBase + 0x284A57) );

	//
	// reenable UDP info packet
	//
	const std::array<uint8_t, 6> netPacketPatch = { 0x89, 0xB0, 0x28, 0x01, 0x00, 0x00 }; // mov [eax+128h], esi 
	WriteProtectedMemory( netPacketPatch, (dwEngineBase + 0x283604) );

	//
	// force direct UDP connection instead of relay connection
	//		  
	// mov dword ptr [eax], 2
	const std::array<uint8_t, 6> relayPatch = {0xC7, 0x00, 0x02, 0x00, 0x00, 0x00};
	WriteProtectedMemory( relayPatch, (dwEngineBase + 0x2BE552) );
	// mov dword ptr [eax+8], 2
	const std::array<uint8_t, 7> relayPatch2 = { 0xC7, 0x40, 0x08, 0x02, 0x00, 0x00, 0x00 };
	WriteProtectedMemory( relayPatch2, (dwEngineBase + 0x2BE56C) );
	// mov dword ptr [eax+4], 2
	const std::array<uint8_t, 7> relayPatch3 = { 0xC7, 0x40, 0x04, 0x02, 0x00, 0x00, 0x00 };
	WriteProtectedMemory( relayPatch3, (dwEngineBase + 0x2BE587) );
}

extern DWORD WINAPI ConsoleThread( LPVOID lpArguments );

ON_LOAD_LIB( engine )
{
	const uintptr_t dwEngineBase = GET_LOAD_LIB_MODULE();
	BytePatchEngine( dwEngineBase );

	HOOK_DETOUR( dwEngineBase + 0x155C80, hkSys_SpewFunc );
	HOOK_DETOUR( dwEngineBase + 0x285FE0, hkGetServerIpAddressInfo );

	CloseHandle( CreateThread( nullptr, NULL, ConsoleThread, nullptr, NULL, nullptr ) );
}

