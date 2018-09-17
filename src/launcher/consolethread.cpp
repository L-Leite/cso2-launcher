#include <Windows.h>
#include <iostream>
#include <string>

#include "cdll_int.h"	 

static IVEngineClient* g_pEngineClient;

void ConnectRequiredLibraries()
{
	CreateInterfaceFn pEngineFactory = Sys_GetFactory( "engine.dll" );
	g_pEngineClient = (IVEngineClient*)pEngineFactory( VENGINE_CLIENT_INTERFACE_VERSION, nullptr );
}

bool g_bEnableLocalization = true;
bool g_bPrintMoreDebugInfo = true;

DWORD WINAPI ConsoleThread( LPVOID lpArguments )
{
	ConnectRequiredLibraries();

	while (true)
	{
		std::string szCommand;
		std::getline( std::cin, szCommand );

<<<<<<< HEAD
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
				"setlang\t-\tSet custom language (require restart to make effect)\n"
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
=======
		if (szCommand.find( "lessdebug" ) == 0)
>>>>>>> upstream/master
		{
			g_bPrintMoreDebugInfo = !g_bPrintMoreDebugInfo;
			std::cout << "There will be " << (g_bPrintMoreDebugInfo ? "more" : "less") << " debug info now\n";
		}
		else if (szCommand.find( "togglelocal" ) == 0)
		{
			g_bEnableLocalization = !g_bEnableLocalization;
			std::cout << "Localization " << (g_bEnableLocalization ? "enabled" : "disabled") <<'\n';
		}
<<<<<<< HEAD
		else if (szArguments[0].find("setlang") == 0)
		{
			if (szArguments.size() != 2)
			{
				printf("Usage: setlang [language]\n");
			}
			else 
			{
				HKEY hKey;
				if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Nexon\\cso2\\cso2_launcher", NULL, KEY_WRITE, &hKey) == ERROR_SUCCESS)
				{
					char dwValue[256];
					DWORD dwSize = sizeof(dwValue);

					if (RegSetValueEx(hKey, "language", 0, REG_SZ, (unsigned char *)szArguments[2].c_str(), szArguments[2].length()) == ERROR_SUCCESS)
					{
						printf("Custom language set to %s, restart your game to take effect.", szArguments[2]);
					}
					else
					{
						RegCloseKey(hKey);
						goto failed;
					}

					RegCloseKey(hKey);
				} 
				else
				{
				failed:
					printf("Failed to set custom language. \n", szArguments[2]);
				}
			}
		}
		else if (szArguments[0].find("stopconsolethread") == 0)
=======
		else if (szCommand.find( "stopconsolethread" ) == 0)
>>>>>>> upstream/master
		{
			break;
		}
		else
		{
			g_pEngineClient->ClientCmd_Unrestricted( szCommand.c_str() );
		}

		szCommand.clear();
	}

	return 0;
}