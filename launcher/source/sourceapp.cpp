#include "stdafx.hpp"
#include "sourceapp.hpp"

#ifdef _WIN32
#include <combaseapi.h>
#endif

#include "tier0/icommandline.h"

#include "IHammer.h"
#include "engine_launcher_api.h"

#include "filesystem.h"

#include "filesystem_init.h"

// interfaces
#include "datacache/idatacache.h"
#include "datacache/imdlcache.h"
#include "filesystem/IQueuedLoader.h"
#include "inputsystem/iinputsystem.h"
#include "istudiorender.h"
#include "p4lib/ip4.h"
#include "tier1/tier1.h"
#include "tier2/tier2.h"
#include "tier3/tier3.h"
#include "vgui/ISurface.h"
#include "vgui/ivgui.h"
#include "video/ivideoservices.h"
#include "vphysics_interface.h"
#include "vstdlib/iprocessutils.h"

#include "cso2/iprecommandlineparser.h"

static IEngineAPI* g_pEngineAPI = nullptr;
static IHammer* g_pHammer = nullptr;

constexpr const char* DEFAULT_HL2_GAMEDIR = "cstrike";

bool g_bTextMode = false;

CSourceAppSystemGroup::CSourceAppSystemGroup(
    std::string szBaseDir, IFileSystem* pFs /*= nullptr*/,
    CAppSystemGroup* pParent /*= nullptr*/ )
    : CSteamAppSystemGroup( pFs, pParent ), m_szBaseDir( szBaseDir )
{
}

//-----------------------------------------------------------------------------
// Instantiate all main libraries
//-----------------------------------------------------------------------------
bool CSourceAppSystemGroup::Create()
{
    IFileSystem* pFileSystem =
        static_cast<IFileSystem*>( FindSystem( FILESYSTEM_INTERFACE_VERSION ) );
    pFileSystem->InstallDirtyDiskReportFunc(
        []() {} );  // XB360 specific, is this necessary?

#ifdef _WIN32
    CoInitializeEx( nullptr, COINIT_APARTMENTTHREADED );
#endif

    // Are we running in edit mode?
    m_bEditMode = CommandLine()->CheckParm( "-edit" );

    double st = Plat_FloatTime();

    AppSystemInfo_t appSystems[] = {
        { "engine.dll",
          CVAR_QUERY_INTERFACE_VERSION },  // NOTE: This one must be first!!
        { "inputsystem.dll", INPUTSYSTEM_INTERFACE_VERSION },
        { "materialsystem.dll", MATERIAL_SYSTEM_INTERFACE_VERSION },
        { "datacache.dll", DATACACHE_INTERFACE_VERSION },
        { "datacache.dll", MDLCACHE_INTERFACE_VERSION },
        { "datacache.dll", STUDIO_DATA_CACHE_INTERFACE_VERSION },
        { "studiorender.dll", STUDIO_RENDER_INTERFACE_VERSION },
        { "vphysics.dll", VPHYSICS_INTERFACE_VERSION },
        { "video_services.dll", VIDEO_SERVICES_INTERFACE_VERSION },
        // NOTE: This has to occur before vgui2.dll so it replaces vgui2's
        // surface implementation
        { "vguimatsurface.dll", VGUI_SURFACE_INTERFACE_VERSION },
        { "vgui2.dll", VGUI_IVGUI_INTERFACE_VERSION },
        { "engine.dll", VENGINE_LAUNCHER_API_VERSION },

        { "", "" }  // Required to terminate the list
    };

    if ( !AddSystems( appSystems ) )
    {
        return false;
    }

    // CSO2 doesn't ship with this module
    // TODO: remove this?
    if ( CommandLine()->FindParm( "-stereosxs" ) )
    {
        AddSystem( LoadModule( "headtrack.dll" ), "VHeadTrack001" );
    }

    char pFileSystemDLL[MAX_PATH];
    bool bSteam = false;
    AppModule_t fileSystemModule = APP_MODULE_INVALID;

    if ( FileSystem_GetFileSystemDLLName( pFileSystemDLL, MAX_PATH, bSteam ) ==
         FS_OK )
    {
        fileSystemModule = LoadModule( pFileSystemDLL );

        if ( fileSystemModule != APP_MODULE_INVALID )
            g_pQueuedLoader = static_cast<IQueuedLoader*>(
                AddSystem( fileSystemModule, QUEUEDLOADER_INTERFACE_VERSION ) );
    }

    // Hook in datamodel and p4 control if we're running with -tools
    if ( IsPC() && ( CommandLine()->FindParm( "-tools" ) ||
                     CommandLine()->FindParm( "-p4" ) ) )
    {
        AppModule_t p4libModule = LoadModule( "p4lib.dll" );
        p4 =
            static_cast<IP4*>( AddSystem( p4libModule, P4_INTERFACE_VERSION ) );
        if ( !p4 )
        {
            return false;
        }

        AppModule_t vstdlibModule = LoadModule( "vstdlib.dll" );
        IProcessUtils* processUtils = static_cast<IProcessUtils*>(
            AddSystem( vstdlibModule, PROCESS_UTILS_INTERFACE_VERSION ) );
        if ( !processUtils )
        {
            return false;
        }
    }

    // Connect to interfaces loaded in AddSystems that we need locally
    IMaterialSystem* pMaterialSystem = static_cast<IMaterialSystem*>(
        FindSystem( MATERIAL_SYSTEM_INTERFACE_VERSION ) );
    if ( !pMaterialSystem )
    {
        return false;
    }

    g_pEngineAPI =
        static_cast<IEngineAPI*>( FindSystem( VENGINE_LAUNCHER_API_VERSION ) );

    // Load the hammer DLL if we're in editor mode
    if ( m_bEditMode )
    {
        AppModule_t hammerModule = LoadModule( "hammer_dll.dll" );
        g_pHammer = static_cast<IHammer*>(
            AddSystem( hammerModule, INTERFACEVERSION_HAMMER ) );
        if ( !g_pHammer )
        {
            return false;
        }
    }

    // Load up the appropriate shader DLL
    // This has to be done before connection.
    char const* pDLLName = "shaderapidx9.dll";
    if ( CommandLine()->FindParm( "-noshaderapi" ) )
    {
        pDLLName = "shaderapiempty.dll";
    }
    pMaterialSystem->SetShaderAPI( pDLLName );

    double elapsed = Plat_FloatTime() - st;
    COM_TimestampedLog(
        "LoadAppSystems:  Took %.4f secs to load libraries and get factories.",
        elapsed );

    return true;
}

bool CSourceAppSystemGroup::PreInit()
{
    CreateInterfaceFn factory = GetFactory();

    ConnectTier1Libraries( &factory, 1 );
    ConVar_Register();
    ConnectTier2Libraries( &factory, 1 );
    ConnectTier3Libraries( &factory, 1 );

    if ( !g_pFullFileSystem || !g_pMaterialSystem )
    {
        return false;
    }

    std::string szDirectoryName = "..\\data\\";
    szDirectoryName += g_pFullFileSystem->GetGameDirectoryName();

    CFSMountContentInfo fsInfo;
    fsInfo.m_pFileSystem = g_pFullFileSystem;
    fsInfo.m_bToolsMode = m_bEditMode;
    fsInfo.m_pDirectoryName = szDirectoryName.c_str();

    if ( FileSystem_MountContent( fsInfo ) != FS_OK )
    {
        return false;
    }

    // Required to run through the editor
    if ( m_bEditMode )
    {
        g_pMaterialSystem->EnableEditorMaterials();
    }

    StartupInfo_t info;
    info.m_pInstance = GetAppInstance();
    info.m_pBaseDirectory = m_szBaseDir.c_str();
    info.m_pInitialMod = DetermineDefaultMod();
    info.m_pInitialGame = DetermineDefaultGame();
    info.m_pParentAppSystemGroup = this;
    info.m_bTextMode = g_bTextMode;

    g_pEngineAPI->SetStartupInfo( info );

    g_pFullFileSystem->AddSearchPath( "cstrike", "GAME" );
    g_pFullFileSystem->AddSearchPath( "cstrike/bin", "GAMEBIN" );

    return true;
}

int CSourceAppSystemGroup::Main()
{
    return g_pEngineAPI->Run();
}

void CSourceAppSystemGroup::PostShutdown()
{
    DisconnectTier3Libraries();
    DisconnectTier2Libraries();
    ConVar_Unregister();
    DisconnectTier1Libraries();
}

void CSourceAppSystemGroup::Destroy()
{
    g_pEngineAPI = nullptr;
    g_pMaterialSystem = nullptr;
    g_pHammer = nullptr;

    CoUninitialize();
}

//-----------------------------------------------------------------------------
// Determines the initial mod to use at load time.
// We eventually (hopefully) will be able to switch mods at runtime
// because the engine/hammer integration really wants this feature.
//-----------------------------------------------------------------------------
const char* CSourceAppSystemGroup::DetermineDefaultMod() const
{
    if ( !m_bEditMode )
    {
        return CommandLine()->ParmValue( "-game", DEFAULT_HL2_GAMEDIR );
    }

    return g_pHammer->GetDefaultMod();
}

const char* CSourceAppSystemGroup::DetermineDefaultGame() const
{
    if ( !m_bEditMode )
    {
        return CommandLine()->ParmValue( "-defaultgamedir",
                                         DEFAULT_HL2_GAMEDIR );
    }

    return g_pHammer->GetDefaultGame();
}
