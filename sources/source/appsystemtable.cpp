#include "source/appsystemtable.hpp"

#include <filesystem>

#include "platform.hpp"
namespace fs = std::filesystem;

#include <appframework/iappsystem.hpp>

AppSystemModule::AppSystemModule( void* moduleBase, CreateInterfaceFn factory,
                                  std::string&& moduleName )
    : m_pModuleBase( moduleBase ), m_pFactory( factory ),
      m_pModuleName( moduleName )
{
}

CreateInterfaceFn AppSystemModule::GetFactory()
{
    if ( this->m_pFactory != nullptr )
    {
        return this->m_pFactory;
    }

    CreateInterfaceFn newFactory = reinterpret_cast<CreateInterfaceFn>(
        Sys_GetModuleExport( this->m_pModuleBase, CREATEINTERFACE_PROCNAME ) );

    this->m_pFactory = newFactory;
    return newFactory;
}

ASModuleIndex AppSystemsTable::LoadModuleByName( const char* pDLLName )
{
    fs::path modulePath = pDLLName;
    modulePath.replace_extension( "" );
    std::string moduleName = modulePath.generic_string();

    ASModuleIndex cachedRes = this->FindIndexByName( moduleName );

    if ( cachedRes != INVALID_APPSYSTEM_MODULE )
    {
        return cachedRes;
    }

    void* moduleHandle = this->LoadModuleInternal( pDLLName );

    if ( moduleHandle == nullptr )
    {
        return INVALID_APPSYSTEM_MODULE;
    }

    this->m_Modules.emplace_back( moduleHandle, nullptr,
                                  std::move( moduleName ) );
    const ASModuleIndex newEntryIndex = this->m_Modules.size() - 1;

    /*GetCSO2LoadingSplash()->SetLoadingProgress( ( newEntryIndex - 2 )
     * / 2 5 ); GetCSO2LoadingSplash()->Unknown00();*/

    return newEntryIndex;
}

ASModuleIndex AppSystemsTable::LoadModuleByFactory( CreateInterfaceFn factory )
{
    if ( factory == nullptr )
    {
        return INVALID_APPSYSTEM_MODULE;
    }

    ASModuleIndex cachedRes = this->FindIndexByFactory( factory );

    if ( cachedRes != INVALID_APPSYSTEM_MODULE )
    {
        return cachedRes;
    }

    this->m_Modules.emplace_back( nullptr, factory, std::string() );
    const ASModuleIndex newEntryIndex = this->m_Modules.size() - 1;
    return newEntryIndex;
}

void AppSystemsTable::UnloadAllModules()
{
    auto it = this->m_Modules.crbegin();

    for ( ; it != this->m_Modules.crend(); it++ )
    {
        this->FreeModuleInternal( it->GetModuleBase() );
    }
}

IAppSystem* AppSystemsTable::AddSystem( ASModuleIndex moduleIndex,
                                        const char* pInterfaceName )
{
    if ( moduleIndex == INVALID_APPSYSTEM_MODULE )
    {
        return nullptr;
    }

    AppSystemModule& targetModule = this->m_Modules[moduleIndex];
    CreateInterfaceFn pFactory = targetModule.GetFactory();

    int factoryValid;
    IAppSystem* pAppSystem =
        static_cast<IAppSystem*>( pFactory( pInterfaceName, &factoryValid ) );

    if ( factoryValid != IFACE_OK || pAppSystem == nullptr )
    {
        return nullptr;
    }

    this->m_Systems.push_back( { pInterfaceName, pAppSystem, false } );
    return pAppSystem;
}

AppSystemsTable* g_pRealAppSysTable = nullptr;

void* AppSystemCreateInterfaceFn2( const char* pName, int* pReturnCode )
{
    void* pInterface = g_pRealAppSysTable->FindSystem( pName );
    if ( pReturnCode )
    {
        *pReturnCode = pInterface ? IFACE_OK : IFACE_FAILED;
    }
    return pInterface;
}

//-----------------------------------------------------------------------------
// Gets at a class factory for the topmost appsystem group in an appsystem stack
//-----------------------------------------------------------------------------
CreateInterfaceFn AppSystemsTable::GetFactory() const
{
    return AppSystemCreateInterfaceFn2;
}

bool AppSystemsTable::ConnectSystems() const
{
    for ( auto&& sysPair : this->m_Systems )
    {
        IAppSystem* pSystem = sysPair.pSystem;

        if ( pSystem->Connect( this->GetFactory() ) == false )
        {
            return false;
        }
    }

    return true;
}

InitReturnVal_t AppSystemsTable::InitSystems()
{
    for ( auto&& sysPair : this->m_Systems )
    {
        if ( sysPair.bInitialized == true )
        {
            continue;
        }

        IAppSystem* pSystem = sysPair.pSystem;
        InitReturnVal_t nRetVal = pSystem->Init();

        if ( nRetVal != INIT_OK )
        {
            return nRetVal;
        }

        sysPair.bInitialized = true;
    }
    return INIT_OK;
}

void AppSystemsTable::ShutdownSystems() const
{
    auto it = this->m_Systems.crbegin();

    for ( ; it != this->m_Systems.crend(); it++ )
    {
        it->pSystem->Shutdown();
    }
}

void AppSystemsTable::DisconnectSystems() const
{
    auto it = this->m_Systems.crbegin();

    for ( ; it != this->m_Systems.crend(); it++ )
    {
        it->pSystem->Disconnect();
    }
}

void* AppSystemsTable::FindSystem( const char* pSystemName )
{
    IAppSystem* findRes = this->FindCachedSystem( pSystemName );

    if ( findRes != nullptr )
    {
        return findRes;
    }

    for ( auto&& sysPair : this->m_Systems )
    {
        IAppSystem* pSystem = sysPair.pSystem;

        void* pInterface = pSystem->QueryInterface( pSystemName );

        if ( pInterface != nullptr )
        {
            return pInterface;
        }
    }

    /*if ( m_pParentAppSystem )
    {
        void* pInterface = m_pParentAppSystem->FindSystem( pSystemName );
        if ( pInterface )
            return pInterface;
    }*/

    return nullptr;
}

ASModuleIndex AppSystemsTable::FindIndexByFactory(
    CreateInterfaceFn targetFactory )
{
    for ( ASModuleIndex i = 0; i < this->m_Modules.size(); i++ )
    {
        if ( this->m_Modules[i].GetFactory() == targetFactory )
        {
            return i;
        }
    }

    return INVALID_APPSYSTEM_MODULE;
}

ASModuleIndex AppSystemsTable::FindIndexByName(
    std::string_view targetName ) const
{
    for ( ASModuleIndex i = 0; i < this->m_Modules.size(); i++ )
    {
        if ( this->m_Modules[i].GetModuleName() == targetName )
        {
            return i;
        }
    }

    return INVALID_APPSYSTEM_MODULE;
}

void* AppSystemsTable::LoadModuleInternal( std::string_view moduleName ) const
{
    return Sys_LoadLibrary( moduleName.data() );
}

void AppSystemsTable::FreeModuleInternal( void* moduleBase ) const
{
    Sys_FreeLibrary( moduleBase );
}

IAppSystem* AppSystemsTable::FindCachedSystem(
    std::string_view systemName ) const
{
    for ( auto&& system : this->m_Systems )
    {
        if ( system.szName == systemName )
        {
            return system.pSystem;
        }
    }

    return nullptr;
}