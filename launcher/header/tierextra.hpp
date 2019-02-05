//
// additional libraries required by our launcher
//

#pragma once

#include "cdll_int.h"

extern IVEngineClient* g_pEngineClient;

void ConnectExtraLibraries( CreateInterfaceFn* pFactoryList,
                            int nFactoryCount );

void DisconnectExtraLibraries();