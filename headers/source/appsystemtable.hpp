#pragma once

#include <cstdint>
#include <limits>
#include <map>
#include <string>
#include <string_view>
#include <vector>

#include <appframework/iappsystem.hpp>
#include <tier1/interface.hpp>

class IAppSystem;
struct AppSystemInfo_t;

using ASModuleIndex = size_t;
constexpr const ASModuleIndex INVALID_APPSYSTEM_MODULE =
    std::numeric_limits<ASModuleIndex>::max();

class AppSystemModule
{
public:
    AppSystemModule() = default;
    AppSystemModule( void* moduleBase, CreateInterfaceFn factory,
                     std::string&& moduleName );

    inline void* GetModuleBase() const { return this->m_pModuleBase; }

    inline const std::string& GetModuleName() const
    {
        return this->m_pModuleName;
    }

    CreateInterfaceFn GetFactory();

private:
    void* m_pModuleBase;
    CreateInterfaceFn m_pFactory;
    std::string m_pModuleName;
};

struct AppSystemData
{
    std::string szName;
    IAppSystem* pSystem;
    bool bInitialized;
};

class AppSystemsTable
{
public:
    AppSystemsTable() = default;
    ~AppSystemsTable() = default;

public:
    ASModuleIndex LoadModuleByName( const char* pDLLName );
    ASModuleIndex LoadModuleByFactory( CreateInterfaceFn factory );
    void UnloadAllModules();

    IAppSystem* AddSystem( ASModuleIndex moduleIndex,
                           const char* pInterfaceName );

    bool ConnectSystems() const;
    InitReturnVal_t InitSystems();

    void ShutdownSystems() const;
    void DisconnectSystems() const;

    void* FindSystem( const char* pSystemName );

    CreateInterfaceFn GetFactory() const;
    inline const auto& GetSystems() const { return this->m_Systems; }

private:
    ASModuleIndex FindIndexByFactory( CreateInterfaceFn targetFactory );
    ASModuleIndex FindIndexByName( std::string_view targetName ) const;

    void* LoadModuleInternal( std::string_view moduleName ) const;
    void FreeModuleInternal( void* moduleBase ) const;

    IAppSystem* FindCachedSystem( std::string_view systemName ) const;

private:
    std::vector<AppSystemModule> m_Modules;
    std::vector<AppSystemData> m_Systems;
};
