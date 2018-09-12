#include "strtools.h"
#include "hooks.h"
#include "engine/cso2/cso2gamemanager.h"

#include <array>

void ConnectEngineLibraries( const uintptr_t dwEngineBase )
{
	if (!g_pCSO2GameManager)
	{
		g_pCSO2GameManager = reinterpret_cast<CSO2GameManager*>(dwEngineBase + 0xAA8D40);
	}
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

HOOK_DETOUR_DECLARE( hkSys_SpewFunc );

NOINLINE int hkSys_SpewFunc( int spewType, char* pMsg )
{
	printf( pMsg );
	if (spewType == 3)
	{
		assert( false );
	}
	return HOOK_DETOUR_GET_ORIG( hkSys_SpewFunc )(spewType, pMsg);
}

void BytePatchEngine( const uintptr_t dwEngineBase )
{
	// allows the player to connect without Packet_UserInfo
	//uint8_t connectClientPatch[] = { 0xEB }; // jmp
	//WriteProtectedMemory(connectClientPatch, (dwEngineBase + 0x69DD4));

	// disables UDP hole puncher checks
	//uint8_t sendPacketPatch[] = { 0xE9, 0xF7, 0x02, 0x00, 0x00, 0x90 };	// short jmp 0x02F7; nop
	//WriteProtectedMemory(sendPacketPatch, (dwEngineBase + 0x12C172));

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
	const std::array<uint8_t, 18> relayPatch2 = { 0xC7, 0x40, 0x08, 0x02, 0x00, 0x00, 0x00 };
	WriteProtectedMemory( relayPatch2, (dwEngineBase + 0x2BE56C) );
	// mov dword ptr [eax+4], 2
	const std::array<uint8_t, 18> relayPatch3 = { 0xC7, 0x40, 0x04, 0x02, 0x00, 0x00, 0x00 };
	WriteProtectedMemory( relayPatch3, (dwEngineBase + 0x2BE587) );
}

ON_LOAD_LIB( engine )
{
	const uintptr_t dwEngineBase = GET_LOAD_LIB_MODULE();

	HOOK_DETOUR( dwEngineBase + 0x285FE0, hkGetServerIpAddressInfo );
	HOOK_DETOUR( dwEngineBase + 0x155C80, hkSys_SpewFunc );

	ConnectEngineLibraries( dwEngineBase );
	BytePatchEngine( dwEngineBase );
}

