#include "strtools.h"
#include "hooks.h"
#include "tier1/interface.h"
#include "client/cso2/icso2msghandlerclient.h"

void ConnectClientLibraries(uintptr_t dwEngineBase)
{
	CreateInterfaceFn pClientFactory = Sys_GetFactory("client.dll");

	if (!g_pCSO2MsgHandlerClient)
		g_pCSO2MsgHandlerClient = (ICSO2MsgHandlerClient*)pClientFactory(CSO2_MSGHANDLERCLIENT_INTERFACE_VERSION, nullptr);
}

ON_LOAD_LIB(client)
{
	uintptr_t dwClientBase = GET_LOAD_LIB_MODULE();	  
	ConnectClientLibraries(dwClientBase);
}			
