//
// additional libraries required by our launcher
//

#pragma once

#include "cdll_int.h"

class ICSO2MsgHandlerEngine;

extern IVEngineClient* g_pEngineClient;


void ConnectExtraLibraries( CreateInterfaceFn* pFactoryList,
                            int nFactoryCount );

void DisconnectExtraLibraries();