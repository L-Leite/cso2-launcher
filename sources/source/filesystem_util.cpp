#include "source/filesystem_util.hpp"

#include <filesystem/filesystem.hpp>
#include <tier0/cso2/iprecommandlineparser.hpp>

const char* FS_GetBinDirectory()
{
    return GetCSO2PreCommandLineParser()->GetAnsiBinDirectory();
}

void FS_SetBasePaths( IFileSystem* pFileSystem )
{
    pFileSystem->RemoveSearchPaths( "EXECUTABLE_PATH" );
    const char* binPath = FS_GetBinDirectory();
    pFileSystem->AddSearchPath( binPath, "EXECUTABLE_PATH" );
}