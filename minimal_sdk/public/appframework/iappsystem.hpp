#pragma once

#include "tier1/interface.hpp"

enum InitReturnVal_t
{
    INIT_FAILED = 0,
    INIT_OK,

    INIT_LAST_VAL,
};

class IAppSystem
{
public:
    // Here's where the app systems get to learn about each other
    virtual bool Connect( CreateInterfaceFn factory ) = 0;
    virtual void Disconnect() = 0;

    // Here's where systems can access other interfaces implemented by this
    // object Returns NULL if it doesn't implement the requested interface
    virtual void* QueryInterface( const char* pInterfaceName ) = 0;

    // Init, shutdown
    virtual InitReturnVal_t Init() = 0;
    virtual void Shutdown() = 0;
};