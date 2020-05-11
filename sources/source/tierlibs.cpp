#include "source/tierlibs.hpp"

#include <stdexcept>

#include "platform.hpp"

#include <engine/cdll_int.hpp>
#include <engine/cso2/icso2msgmanager.hpp>
#include <filesystem/filesystem.hpp>
#include <icvar.hpp>
#include <inputsystem/iinputsystem.hpp>
#include <materialsystem/imaterialsystem.hpp>
#include <tier0/cso2/iloadingsplash.hpp>
#include <tier0/cso2/iprecommandlineparser.hpp>
#include <tier0/dbg.hpp>
#include <tier0/icommandline.hpp>
#include <tier0/platform.hpp>

template <class T>
inline T* FactoryCast( CreateInterfaceFn f, const char* szName, int* retCode )
{
    return reinterpret_cast<T*>( f( szName, retCode ) );
}

//
// tier1 libraries
//
ICvar* g_pCVar = nullptr;

void ConnectTier1Libraries( CreateInterfaceFn factory )
{
    g_pCVar = FactoryCast<ICvar>( factory, CVAR_INTERFACE_VERSION, NULL );
}

//
// tier2 libraries
//
IFileSystem* g_pFullFileSystem = nullptr;
IMaterialSystem* g_pMaterialSystem = nullptr;
IInputSystem* g_pInputSystem = nullptr;

void ConnectTier2Libraries( CreateInterfaceFn factory )
{
    g_pFullFileSystem =
        FactoryCast<IFileSystem>( factory, FILESYSTEM_INTERFACE_VERSION, NULL );

    g_pMaterialSystem = FactoryCast<IMaterialSystem>(
        factory, MATERIAL_SYSTEM_INTERFACE_VERSION, NULL );

    g_pInputSystem = FactoryCast<IInputSystem>(
        factory, INPUTSYSTEM_INTERFACE_VERSION, NULL );
}

//
// aditional libraries
//
IVEngineClient* g_pEngineClient = nullptr;
ICSO2MsgHandlerEngine* g_pCSO2MsgHandler = nullptr;

void ConnectNonTierLibraries( CreateInterfaceFn factory )
{
    g_pEngineClient = FactoryCast<IVEngineClient>(
        factory, VENGINE_CLIENT_INTERFACE_VERSION, NULL );

    g_pCSO2MsgHandler = FactoryCast<ICSO2MsgHandlerEngine>(
        factory, CSO2_MSGHANDLER_ENGINE_VERSION, NULL );
}

void ConnectAllLibraries( CreateInterfaceFn factory )
{
    ConnectTier1Libraries( factory );
    ConnectTier2Libraries( factory );

    // we don't need any tier3 libraries at the moment
    // ConnectTier3Libraries( factory );

    ConnectNonTierLibraries( factory );
}

//
// tier0 imports
//

void* GetTierZeroModule()
{
    static void* zeroModuleBase = nullptr;

    if ( zeroModuleBase == nullptr )
    {
        zeroModuleBase = Sys_LoadLibrary( "tier0.dll" );

        if ( zeroModuleBase == nullptr )
        {
            throw std::runtime_error(
                "Could not find/load tier0.dll.\n"
                "Make sure the launcher is in the correct directory." );
        }
    }

    return zeroModuleBase;
}

template <typename FuncType>
inline FuncType GetTierZeroExport( const char* exportName )
{
    auto res = reinterpret_cast<FuncType>(
        Sys_GetModuleExport( GetTierZeroModule(), exportName ) );

    if ( res == nullptr )
    {
        std::string errMsg = "Could not get tier0 export ";
        errMsg += exportName;
        throw std::runtime_error( errMsg );
    }

    return res;
}

ICommandLine* CommandLine()
{
    using fn_t = ICommandLine* (*)();
    static fn_t pFunc = nullptr;

    if ( pFunc == nullptr )
    {
        pFunc = GetTierZeroExport<fn_t>( "CommandLine_Tier0" );
    }

    return pFunc();
}

ICSO2LoadingSplash* GetCSO2LoadingSplash()
{
    using fn_t = ICSO2LoadingSplash* (*)();
    static fn_t pFunc = nullptr;

    if ( pFunc == nullptr )
    {
        pFunc = GetTierZeroExport<fn_t>( "GetCSO2LoadingSplash" );
    }

    return pFunc();
}

void SpewOutputFunc( SpewOutputFunc_t func )
{
    using fn_t = void ( * )( SpewOutputFunc_t );
    static fn_t pFunc = nullptr;

    if ( pFunc == nullptr )
    {
        pFunc = GetTierZeroExport<fn_t>( "SpewOutputFunc" );
    }

    pFunc( func );
}

ICSO2PreCommandLineParser* GetCSO2PreCommandLineParser()
{
    using fn_t = ICSO2PreCommandLineParser* (*)();
    static fn_t pFunc = nullptr;

    if ( pFunc == nullptr )
    {
        pFunc = GetTierZeroExport<fn_t>( "GetCSO2PreCommandLineParser" );
    }

    return pFunc();
}

const char* GetSpewOutputGroup()
{
    using fn_t = const char* (*)();
    static fn_t pFunc = nullptr;

    if ( pFunc == nullptr )
    {
        pFunc = GetTierZeroExport<fn_t>( "GetSpewOutputGroup" );
    }

    return pFunc();
}

bool ShouldUseNewAssertDialog()
{
    using fn_t = bool ( * )();
    static fn_t pFunc = nullptr;

    if ( pFunc == nullptr )
    {
        pFunc = GetTierZeroExport<fn_t>( "ShouldUseNewAssertDialog" );
    }

    return pFunc();
}

double Plat_FloatTime()
{
    using fn_t = double ( * )();
    static fn_t pFunc = nullptr;

    if ( pFunc == nullptr )
    {
        pFunc = GetTierZeroExport<fn_t>( "Plat_FloatTime" );
    }

    return pFunc();
}