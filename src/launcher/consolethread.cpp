#include <Windows.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>

#include "cdll_int.h"	 
#include "client/cso2/icso2msghandlerclient.h"
#include "engine/cso2/icso2lobbystatemanager.h"	
#include "engine/cso2/cso2gameroom.h"
#include "engine/cso2/cso2gamemanager.h"

static IVEngineClient* g_pEngineClient;

void ConnectRequiredLibraries()
{
	CreateInterfaceFn pEngineFactory = Sys_GetFactory("engine.dll");
	g_pEngineClient = (IVEngineClient*) pEngineFactory(VENGINE_CLIENT_INTERFACE_VERSION, nullptr);
}

void HostRoom(uint8_t iGamemodeId, uint8_t iMapId)
{
	CSO2ModInfo* pModInfo = g_pCSO2GameManager->GetModInfo();
	CSO2MapInfo* pMapInfo = g_pCSO2GameManager->GetMapInfo();

	std::vector<int>& vGamemodesId = pModInfo->GetId();
	std::vector<int>& vMapsId = pMapInfo->GetId();

	if (std::find(vGamemodesId.begin(), vGamemodesId.end(), iGamemodeId) == vGamemodesId.end())
	{
		printf("Invalid gamemode %i!\n", iGamemodeId);
		return;
	} 

	if (std::find(vMapsId.begin(), vMapsId.end(), iMapId) == vMapsId.end())
	{
		printf("Invalid map %i!\n", iMapId);
		return;
	}

	ptrdiff_t iMapPos = std::distance(vMapsId.begin(), std::find(vMapsId.begin(), vMapsId.end(), iMapId));
	ptrdiff_t iGamemodePos = std::distance(vGamemodesId.begin(), std::find(vGamemodesId.begin(), vGamemodesId.end(), iGamemodeId));

	std::vector<std::string>& vGamemodesName = pModInfo->GetName();
	std::vector<std::string>& vMapsName = pMapInfo->GetName(); 

	std::string szGamemodeName = vGamemodesName[iGamemodePos];
	std::string szMapName = vMapsName[iMapPos];	   	
	std::transform(szGamemodeName.begin(), szGamemodeName.end(), szGamemodeName.begin(), tolower);
	std::transform(szMapName.begin(), szMapName.end(), szMapName.begin(), tolower);		

	printf("Starting a %s match on %s\n", szGamemodeName.c_str(), szMapName.c_str());

	std::shared_ptr<CSO2GameRoom> pRoom = g_pCSO2GameManager->CreateAndJoinRoom(1);

	pRoom->SetRoomName("lobby example");
	pRoom->SetMapId(iMapId);
	pRoom->SetupRoomRules(iGamemodeId, 10, 10);
	pRoom->SetPlayersNum(1);
	pRoom->SetBots(8, 8);

	g_pEngineClient->GetCSO2LobbyStateManager()->SetCurrentState(10);
	g_pCSO2MsgHandlerClient->Unknown00(53, 0, 0);

	std::string szMapCommand = "map " + szMapName;
	g_pEngineClient->ClientCmd_Unrestricted(szMapCommand.c_str());
}

void ListGamemodes()
{
	CSO2ModInfo* pModInfo = g_pCSO2GameManager->GetModInfo();
	std::vector<int>& vGamemodesId = pModInfo->GetId();
	std::vector<std::string>& vGamemodesName = pModInfo->GetName();

	printf("Available gamemodes: (listed as \"[Mode ID] - [Mode name]\"\n");

	for (int i = 0; i < pModInfo->GetCount(); i++)
	{
		printf("%i\t-\t%s\n", vGamemodesId[i], vGamemodesName[i].c_str());
	}
}

void ListMaps()
{	
	CSO2MapInfo* pMapInfo = g_pCSO2GameManager->GetMapInfo();
	std::vector<int>& vMapsId = pMapInfo->GetId();
	std::vector<std::string>& vMapsName = pMapInfo->GetName();

	printf("Available maps: (listed as \"[Map ID] - [Map name]\"\n");

	for (int i = 0; i < pMapInfo->GetCount(); i++)
	{
		printf("%i\t-\t%s\n", vMapsId[i], vMapsName[i].c_str());
	}
}

bool g_bEnableLocalization = true;
bool g_bPrintMoreDebugInfo = true;

DWORD WINAPI ConsoleThread(LPVOID lpArguments)
{
	ConnectRequiredLibraries();

	bool bShouldQuit = false;

	while (!bShouldQuit)
	{
		static std::string szCommand;
		std::getline(std::cin, szCommand);

		std::string szTempArgBuffer;
		std::stringstream ss(szCommand);

		std::vector<std::string> szArguments;

		while (ss >> szTempArgBuffer)
			szArguments.push_back(szTempArgBuffer);

		if (szArguments[0].find("help") == 0)
		{
			printf("Available commands:\n"
				"\n"
				"joinroom [gamemode id] [map id]\t-\tCreates a room and starts the room match\n"
				"listgamemodes\t-\tLists the available game modes\n"
				"listmaps\t-\tLists the available maps\n"
				"lessdebug\t-\tShortens the information printed here\n"
				"togglelocal\t-\tToggles text localization\n"
				"\n"
				"And when you're ingame:\n"
				"jointeam [team number]\t-\tJoins you in a team, where 1 is spectator, 2 is terrorist and 3 is ct\n"
				"disconnect\t-\tDisconnects you from your current match\n"
				"You may also use this console as if it was an ingame console (you can enable 'cl_showfps', for example)\n");
		}
		else if (szArguments[0].find("joinroom") == 0)
		{
			if (szArguments.size() != 3)
				printf("Usage: joinroom [gamemode id] [map id]\n");
			else
				HostRoom(std::stoi(szArguments[1]), std::stoi(szArguments[2]));
		}
		else if (szArguments[0].find("listgamemodes") == 0)
		{
			ListGamemodes();
		}
		else if (szArguments[0].find("listmaps") == 0)
		{
			ListMaps();
		}
		else if (szArguments[0].find("lessdebug") == 0)
		{
			g_bPrintMoreDebugInfo = !g_bPrintMoreDebugInfo;
			printf("There will be %s debug info now\n", g_bPrintMoreDebugInfo ? "more" : "less");
		}
		else if (szArguments[0].find("togglelocal") == 0)
		{
			g_bEnableLocalization = !g_bEnableLocalization;
			printf("Localization %s\n", g_bEnableLocalization ? "enabled" : "disabled");
		}
		else if (szArguments[0].find("stopconsolethread") == 0)
		{
			bShouldQuit = true;
		}
		else
		{
			g_pEngineClient->ClientCmd_Unrestricted(szCommand.c_str());
		}

		szCommand.clear();
	}

	return 0;
}