#pragma once

//
// fake implementation of CAppSystemGroup used by the engine
// this is the only way to pass to the engine the app systems that we setup in
// the launcher
//

#include <vector>

#include <appframework/iappsystemgroup.hpp>
#include <containers/utldict.hpp>
#include <containers/utlvector.hpp>

#include "source/appsystemtable.hpp"

class CAppSystemGroup : public IAppSystemGroup
{
public:
    CAppSystemGroup( const std::vector<AppSystemData>& realSystems );

    virtual bool Create() { return true; }
    virtual bool PreInit() { return true; }
    virtual int Main() { return 0; }
    virtual void PostShutdown() {}
    virtual void Destroy() {}
    virtual int Startup() { return 0; }
    virtual void Shutdown() {}

private:
    virtual CSysModule* LoadModuleDLL( const char* /*pDLLName*/ )
    {
        return nullptr;
    }

    struct Module_t
    {
        CSysModule* m_pModule;
        CreateInterfaceFn m_Factory;
        char* m_pModuleName;
    };

    enum AppSystemGroupStage_t
    {
        CREATION = 0,
        CONNECTION,
        PREINITIALIZATION,
        INITIALIZATION,
        SHUTDOWN,
        POSTSHUTDOWN,
        DISCONNECTION,
        DESTRUCTION,

        NONE,  // This means no error
    };

    CUtlVector<Module_t> m_Modules;
    CUtlVector<IAppSystem*> m_Systems;
    CUtlDict<int, unsigned short> m_SystemDict;
    CAppSystemGroup* m_pParentAppSystem;
    AppSystemGroupStage_t m_nErrorStage;
};