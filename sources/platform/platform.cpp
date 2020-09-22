#include "platform.hpp"

#include <tier0/icommandline.hpp>

fs::path Game_GetBaseDirectory()
{
    const char* szOverrideDir = CommandLine()->CheckParm( "-basedir" );
    bool bShouldOverrideCurPath = szOverrideDir != nullptr;

    if ( bShouldOverrideCurPath == true )
    {
        fs::path newPath = szOverrideDir;
        std::filesystem::current_path( newPath );
        return newPath;
    }
    else
    {
        return std::filesystem::current_path();
    }
}

CreateInterfaceFn Sys_GetFactory( const char* pModuleName )
{
    return Sys_GetModuleExport<CreateInterfaceFn>(
        Sys_GetModuleBase( pModuleName ), CREATEINTERFACE_PROCNAME );
}
