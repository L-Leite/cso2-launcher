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

		if (szCommand.find( "lessdebug" ) == 0)
		{
			g_bPrintMoreDebugInfo = !g_bPrintMoreDebugInfo;
			std::cout << "There will be " << (g_bPrintMoreDebugInfo ? "more" : "less") << " debug info now\n";
		}
		else if (szCommand.find( "togglelocal" ) == 0)
		{
			g_bEnableLocalization = !g_bEnableLocalization;
			std::cout << "Localization " << (g_bEnableLocalization ? "enabled" : "disabled") <<'\n';
		}
		else if (szCommand.find( "stopconsolethread" ) == 0)
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