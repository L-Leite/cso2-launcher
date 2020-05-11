#pragma once

using AppModule_t = int;

constexpr const AppModule_t APP_MODULE_INVALID = -1;

class IAppSystemGroup
{
public:
    // An installed application creation function, you should tell the group
    // the DLLs and the singleton interfaces you want to instantiate.
    // Return false if there's any problems and the app will abort
    virtual bool Create() = 0;

    // Allow the application to do some work after AppSystems are connected but
    // they are all Initialized.
    // Return false if there's any problems and the app will abort
    virtual bool PreInit() = 0;

    // Main loop implemented by the application
    virtual int Main() = 0;

    // Allow the application to do some work after all AppSystems are shut down
    virtual void PostShutdown() = 0;

    // Call an installed application destroy function, occurring after all
    // modules are unloaded
    virtual void Destroy() = 0;
};
