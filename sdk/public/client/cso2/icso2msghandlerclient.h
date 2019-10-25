#pragma once

#define CSO2_MSGHANDLERCLIENT_INTERFACE_VERSION	"CSO2MsgHandlerClient001"

abstract_class ICSO2MsgHandlerClient
{
public:
	virtual void Unknown00(int a2, int a3, int a4) = 0;
	virtual void Unknown01(int a2, int a3, int a4, int a5) = 0;
};

extern ICSO2MsgHandlerClient* g_pCSO2MsgHandlerClient;
