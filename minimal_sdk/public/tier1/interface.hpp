#pragma once

constexpr const char* CREATEINTERFACE_PROCNAME = "CreateInterface";
using CreateInterfaceFn = void* (*)( const char* pName, int* pReturnCode );

// All interfaces derive from this.
class IBaseInterface
{
public:
    virtual ~IBaseInterface() {}
};

enum Sys_Flags
{
    SYS_NOFLAGS = 0x00,
    SYS_NOLOAD = 0x01  // no loading, no ref-counting, only returns handle if
                       // lib is loaded.
};

// load/unload components
class CSysModule;

// interface return status
enum
{
    IFACE_OK = 0,
    IFACE_FAILED
};