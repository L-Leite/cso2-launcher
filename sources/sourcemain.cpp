
#include "source/appsystemtable.hpp"
#include "source/filesystem_util.hpp"
#include "source/sourcesystem.hpp"

#include "platform.hpp"

#include <engine/engineapi.hpp>
#include <filesystem/filesystem.hpp>
#include <icvar.hpp>

bool InitRequiredSystems( AppSystemsTable& appSysTable, IFileSystem*& outFs )
{
    fs::path fsPath = FS_GetFileSystemLibPath();
    std::string szFsPath = fsPath.generic_string();

    auto cvarModule = appSysTable.LoadModuleByName( "vstdlib.dll" );
    appSysTable.AddSystem( cvarModule, CVAR_INTERFACE_VERSION );

    auto fileSystemModule = appSysTable.LoadModuleByName( szFsPath.c_str() );
    outFs = static_cast<IFileSystem*>( appSysTable.AddSystem(
        fileSystemModule, FILESYSTEM_INTERFACE_VERSION ) );

    if ( outFs == nullptr )
    {
        return false;
    }

    FS_SetBasePaths( outFs );

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

        if ( InitRequiredSystems( appSystemsTable, pFileSystem ) == false )
        {
            User_MessageBox( "Failed to init required launcher systems",
                             "Error" );
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