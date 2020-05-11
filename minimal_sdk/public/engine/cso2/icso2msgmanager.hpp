#pragma once

constexpr const char* CSO2_MSGHANDLER_ENGINE_VERSION =
    "CSO2MsgHandlerEngine001";

//
// interface of CSO2MsgHandler
// instance @ engine.dll + 0xAA8190 (rva)
// has 230 virtual methods
//
class ICSO2MsgHandlerEngine
{
public:
    virtual void sub_1028465() = 0;
    virtual void sub_10286160() = 0;
    virtual bool Login( const char* szGameName, const char* szPassword,
                        const char* szNexonName, const char* a5 = nullptr,
                        const char* a6 = nullptr ) = 0;
};

extern ICSO2MsgHandlerEngine* g_pCSO2MsgHandler;
