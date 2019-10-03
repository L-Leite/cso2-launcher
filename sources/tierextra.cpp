#include "stdafx.hpp"

#include "tierextra.hpp"

#include "engine/cso2/icso2msgmanager.h"

IVEngineClient* g_pEngineClient = nullptr;
ICSO2MsgHandlerEngine* g_pCSO2MsgHandler = nullptr;

//
// link additional library interfaces needed by us
//
void ConnectExtraLibraries( CreateInterfaceFn* pFactoryList, int nFactoryCount )
{
    // Don't connect twice..
    assert( g_pEngineClient == nullptr );

    for ( int i = 0; i < nFactoryCount; ++i )
    {
        if ( !g_pEngineClient )
        {
            g_pEngineClient = reinterpret_cast<IVEngineClient*>(
                pFactoryList[i]( VENGINE_CLIENT_INTERFACE_VERSION, nullptr ) );
        }

        if ( !g_pCSO2MsgHandler )
        {
            g_pCSO2MsgHandler = reinterpret_cast<ICSO2MsgHandlerEngine*>(
                pFactoryList[i]( CSO2_MSGHANDLER_ENGINE_VERSION, nullptr ) );
        }
    }
}

void DisconnectExtraLibraries()
{
    g_pEngineClient = nullptr;
    g_pCSO2MsgHandler = nullptr;
}
