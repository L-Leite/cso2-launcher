
#include "source/appsystemtable.hpp"
#include "source/filesystem_util.hpp"
#include "source/sourcesystem.hpp"

#include "platform.hpp"

#include <engine/engineapi.hpp>
#include <filesystem/filesystem.hpp>
#include <icvar.hpp>

#include <stdexcept>

bool InitRequiredSystems( AppSystemsTable& appSysTable, IFileSystem*& outFs )
{
    auto cvarModule = appSysTable.LoadModuleByName( "vstdlib.dll" );
    appSysTable.AddSystem( cvarModule, CVAR_INTERFACE_VERSION );

    auto fileSystemModule =
        appSysTable.LoadModuleByName( "FileSystem_Stdio.dll" );
    outFs = static_cast<IFileSystem*>( appSysTable.AddSystem(
        fileSystemModule, FILESYSTEM_INTERFACE_VERSION ) );

    if ( outFs == nullptr )
    {
        return false;
    }

    FS_SetBasePaths( outFs );
    FS_SetCustomPaths( outFs );

    return appSysTable.ConnectSystems() == true &&
           appSysTable.InitSystems() == INIT_OK;
}

extern AppSystemsTable* g_pRealAppSysTable;

void SourceMain( void* pInstance )
{
    int engineReturnRes = RUN_OK;

    do
    {
        AppSystemsTable appSystemsTable;
        g_pRealAppSysTable = &appSystemsTable;

        IFileSystem* pFileSystem = nullptr;

        try
        {
            if ( InitRequiredSystems( appSystemsTable, pFileSystem ) == false )
            {
                User_MessageBox( "Failed to init required launcher systems",
                                 "Error" );
                return;
            }
        }
        catch ( const std::exception& e )
        {
            User_MessageBox( e.what(), "Unhandled exception" );
            return;
        }

        CSourceSystem system( appSystemsTable, pInstance, pFileSystem );

        if ( system.Init() == false )
        {
            User_MessageBox( "Failed to init engine systems", "Error" );
            return;
        }

        engineReturnRes = system.MainEntrypont();
    } while ( engineReturnRes == RUN_RESTART );
}