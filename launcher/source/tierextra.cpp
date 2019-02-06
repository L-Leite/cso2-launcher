#include "stdafx.hpp"

#include "tierextra.hpp"
#include "tier0/dbg.h"

IVEngineClient* g_pEngineClient = nullptr;

//-----------------------------------------------------------------------------
// Call this to connect to all tier 3 libraries.
// It's up to the caller to check the globals it cares about to see if ones are
// missing
//-----------------------------------------------------------------------------
void ConnectExtraLibraries( CreateInterfaceFn* pFactoryList, int nFactoryCount )
{
    // Don't connect twice..
    Assert( g_pEngineClient == nullptr );

    for ( int i = 0; i < nFactoryCount; ++i )
    {
        if ( !g_pEngineClient )
        {
            g_pEngineClient = reinterpret_cast<IVEngineClient*>(
                pFactoryList[i]( VENGINE_CLIENT_INTERFACE_VERSION, nullptr ) );
        }
    }
}

void DisconnectExtraLibraries()
{
    g_pEngineClient = nullptr;
}
