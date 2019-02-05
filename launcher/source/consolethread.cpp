#include "stdafx.hpp"
#include <iostream>

#include "tierextra.hpp"

void ConsoleThread()
{
    CreateInterfaceFn pEngineFactory = Sys_GetFactory( "engine.dll" );
    ConnectExtraLibraries( &pEngineFactory, 1 );

	if ( g_pEngineClient == nullptr )
    {
        std::cout << "ConsoleThread: g_pEngineClient is nullptr\n";
        return;
	}

	std::string szCommand;

    while ( szCommand.find( "stopconsolethread" ) != 0 )
    {
        std::getline( std::cin, szCommand );
        g_pEngineClient->ClientCmd_Unrestricted( szCommand.c_str() );
    }
}