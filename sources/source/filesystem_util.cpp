#include "source/filesystem_util.hpp"

#include <filesystem/filesystem.hpp>
#include <tier0/cso2/iprecommandlineparser.hpp>

const char* FS_GetBinDirectory()
{
    return GetCSO2PreCommandLineParser()->GetAnsiBinDirectory();
}

fs::path FS_GetFileSystemLibPath()
{
    fs::path path = FS_GetBinDirectory();
    path /= "filesystem_stdio.dll";
    return path;
}

void FS_SetBasePaths( IFileSystem* pFileSystem )
{
    pFileSystem->RemoveSearchPaths( "EXECUTABLE_PATH" );
    const char* binPath = FS_GetBinDirectory();
    pFileSystem->AddSearchPath( binPath, "EXECUTABLE_PATH" );
}