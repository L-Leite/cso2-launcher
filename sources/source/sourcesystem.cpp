#include "source/sourcesystem.hpp"

#include "source/appsystemgroup.hpp"
#include "source/appsystemtable.hpp"
#include "source/filesystem_util.hpp"
#include "source/tierlibs.hpp"

#include "platform.hpp"

#include <engine/engineapi.hpp>
#include <filesystem/filesystem.hpp>
#include <tier0/icommandline.hpp>
#include <tier1/convar.hpp>

// interfaces
#include <datacache/idatacache.hpp>
#include <datacache/imdlcache.hpp>
#include <filesystem/iqueuedloader.hpp>
#include <ihammer.hpp>
#include <inputsystem/iinputsystem.hpp>
#include <istudiorender.hpp>
#include <materialsystem/imaterialsystem.hpp>
#include <vgui/isurface.hpp>
#include <vgui/ivgui.hpp>
#include <video/ivideoservices.hpp>
#include <vphysics/iphysics.hpp>

constexpr const char* DEFAULT_ENGINE_GAMEDIR = "cstrike";

CSourceSystem::CSourceSystem( AppSystemsTable& appSysTable, void* instance,
                              IFileSystem* fs )
    : m_AppSysTable( appSysTable ), m_pInstance( instance ), m_pFileSystem( fs )
{
    this->m_szGameDirPath = Game_GetBaseDirectory().generic_string();
    this->m_bTextMode = CommandLine()->CheckParm( "-textmode" ) != nullptr;
    this->m_bHammerMode = CommandLine()->CheckParm( "-edit" ) != nullptr;
}

CSourceSystem::~CSourceSystem()
{
    this->Destroy();
}

bool CSourceSystem::Init()
{
    if ( this->LoadRequiredLibraries() == false )
    {
        User_MessageBox( "Could not load required app systems", "Error" );
        return false;
    }

    if ( this->m_AppSysTable.ConnectSystems() == false )
    {
        User_MessageBox( "Could not connect app systems", "Error" );
        return false;
    }

    this->m_bSystemsConnected = true;

    if ( this->InitTierLibraries() == false )
    {
        User_MessageBox( "Could not init tier libraries", "Error" );
        return false;
    }

    this->SetEngineStartupInfo();

    if ( this->m_AppSysTable.InitSystems() == false )
    {
        User_MessageBox( "Could not init app systems", "Error" );
        return false;
    }

    this->m_bSystemsInitialized = true;

    return true;
}

int CSourceSystem::MainEntrypont() const
{
    return this->m_pEngineApi->Run();
}

void CSourceSystem::Destroy()
{
    if ( this->m_bSystemsInitialized == true )
    {
        this->m_AppSysTable.DisconnectSystems();
    }

    if ( this->m_bSystemsConnected == true )
    {
        this->m_AppSysTable.ShutdownSystems();
    }

    this->m_AppSysTable.UnloadAllModules();
}

bool CSourceSystem::LoadRequiredLibraries()
{
    if ( this->AddRequiredSystems() == false )
    {
        return false;
    }

    this->m_pEngineApi = static_cast<IEngineAPI*>(
        this->m_AppSysTable.FindSystem( VENGINE_LAUNCHER_API_VERSION ) );

    if ( this->m_bHammerMode == true )
    {
        if ( this->InitHammer() == false )
        {
            return false;
        }
    }

    if ( this->InitMaterialSystemAPI() == false )
    {
        return false;
    }

    return true;
}

bool CSourceSystem::InitTierLibraries()
{
    ConnectAllLibraries( this->m_AppSysTable.GetFactory() );

    if ( g_pFullFileSystem == nullptr || g_pMaterialSystem == nullptr )
    {
        return false;
    }

    return true;
}

void CSourceSystem::SetEngineStartupInfo()
{
    if ( this->m_bHammerMode )
    {
        g_pMaterialSystem->EnableEditorMaterials();
    }

    StartupInfo_t info;
    info.m_pInstance = this->m_pInstance;
    info.m_pBaseDirectory = this->m_szGameDirPath.c_str();
    info.m_pInitialMod = this->GetDefaultMod();
    info.m_pInitialGame = this->GetDefaultGame();
    info.m_pParentAppSystemGroup =
        new CAppSystemGroup( this->m_AppSysTable.GetSystems() );
    info.m_bTextMode = this->m_bTextMode;

    this->m_pEngineApi->SetStartupInfo( info );

    this->m_pFileSystem->AddSearchPath( "cstrike", "GAME" );
    this->m_pFileSystem->AddSearchPath( "cstrike/bin", "GAMEBIN" );
}

bool CSourceSystem::AddRequiredSystems() const
{
    // DO NOT change these systems' order
    // because some systems depend on other systems
    constexpr const std::pair<const char*, const char*> requiredAppSystems[] = {
        { "engine.dll", CVAR_QUERY_INTERFACE_VERSION },
        { "inputsystem.dll", INPUTSYSTEM_INTERFACE_VERSION },
        { "materialsystem.dll", MATERIAL_SYSTEM_INTERFACE_VERSION },
        { "datacache.dll", DATACACHE_INTERFACE_VERSION },
        { "datacache.dll", MDLCACHE_INTERFACE_VERSION },
        { "datacache.dll", STUDIO_DATA_CACHE_INTERFACE_VERSION },
        { "studiorender.dll", STUDIO_RENDER_INTERFACE_VERSION },
        { "vphysics.dll", VPHYSICS_INTERFACE_VERSION },
        { "video_services.dll", VIDEO_SERVICES_INTERFACE_VERSION },
        { "vguimatsurface.dll", VGUI_SURFACE_INTERFACE_VERSION },
        { "vgui2.dll", VGUI_IVGUI_INTERFACE_VERSION },
        { "engine.dll", VENGINE_LAUNCHER_API_VERSION },

        // NOTE: we can load the system here because CSO2 will ALWAYS use
        // 'filesystem_stdio.dll' asthe filesystem library, in regular source
        // games this may not happen
        { "filesystem_stdio.dll", QUEUEDLOADER_INTERFACE_VERSION },
    };

    for ( auto&& system : requiredAppSystems )
    {
        const char* szSystem = system.first;
        const char* szInterface = system.second;

        ASModuleIndex module = this->m_AppSysTable.LoadModuleByName( szSystem );

        if ( module == INVALID_APPSYSTEM_MODULE )
        {
            return false;
        }

        IAppSystem* pSystem =
            this->m_AppSysTable.AddSystem( module, szInterface );

        if ( pSystem == nullptr )
        {
            return false;
        }
    }

    return true;
}

bool CSourceSystem::InitHammer()
{
    ASModuleIndex hammerModule =
        this->m_AppSysTable.LoadModuleByName( "hammer_dll.dll" );

    if ( hammerModule == INVALID_APPSYSTEM_MODULE )
    {
        return false;
    }

    this->m_pHammer = static_cast<IHammer*>( this->m_AppSysTable.AddSystem(
        hammerModule, INTERFACEVERSION_HAMMER ) );

    if ( this->m_pHammer == nullptr )
    {
        return false;
    }

    return true;
}

bool CSourceSystem::InitMaterialSystemAPI()
{
    IMaterialSystem* pMaterialSystem = static_cast<IMaterialSystem*>(
        this->m_AppSysTable.FindSystem( MATERIAL_SYSTEM_INTERFACE_VERSION ) );

    if ( pMaterialSystem == nullptr )
    {
        return false;
    }

    const char* szApiModuleName = "shaderapidx9.dll";

    if ( CommandLine()->FindParm( "-noshaderapi" ) )
    {
        szApiModuleName = "shaderapiempty.dll";
    }

    pMaterialSystem->SetShaderAPI( szApiModuleName );

    return true;
}

const char* CSourceSystem::GetDefaultMod() const
{
    if ( this->m_bHammerMode == true )
    {
        return this->m_pHammer->GetDefaultMod();
    }

    return CommandLine()->ParmValue( "-game", DEFAULT_ENGINE_GAMEDIR );
}

const char* CSourceSystem::GetDefaultGame() const
{
    if ( this->m_bHammerMode == true )
    {
        return this->m_pHammer->GetDefaultGame();
    }

    return CommandLine()->ParmValue( "-defaultgamedir",
                                     DEFAULT_ENGINE_GAMEDIR );
}
