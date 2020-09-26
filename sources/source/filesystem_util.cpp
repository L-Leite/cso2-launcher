#include "source/filesystem_util.hpp"
#include "utilities/log.hpp"

#include <filesystem/filesystem.hpp>
#include <tier0/cso2/iprecommandlineparser.hpp>

#include <array>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>
namespace fs = std::filesystem;
using namespace std::string_view_literals;

constexpr const char CUSTOM_DIRECTORY[] = "cstrike/custom";
constexpr const std::array<std::string_view, 7> CUSTOM_BLACKLISTED_DIRS = {
    "materials"sv, "maps"sv,    "models"sv, "resource"sv,
    "sound"sv,     "scripts"sv, "ui_gfx"sv
};
constexpr const char CUSTOM_DIRECTORY_BLACKLIST_MSG[] =
    "Tried to add {} as a search path.\n"
    "\n"
    "This is probably not what you intended.\n"
    "\n"
    "Create a new directory and place your files under it.\n"
    "Example:\n"
    "- cstrike/custom/my_custom_stuff/ (This subfolder will be added as "
    "a search path)\n"
    "- cstrike/custom/my_custom_stuff/models/custom_model.mdl\n"
    "- cstrike/custom/my_custom_stuff/materials/custom_material.vmt\n"
    "- cstrike/custom/my_custom_stuff/ui_gfx/custom_ui_file.res\n";

const char* FS_GetBinDirectory()
{
    return GetCSO2PreCommandLineParser()->GetAnsiBinDirectory();
}

const char* FS_GetDataDirectory()
{
    return GetCSO2PreCommandLineParser()->GetAnsiDataDirectory();
}

void FS_SetBasePaths( IFileSystem* pFileSystem )
{
    pFileSystem->RemoveSearchPaths( "EXECUTABLE_PATH" );
    const char* binPath = FS_GetBinDirectory();
    pFileSystem->AddSearchPath( binPath, "EXECUTABLE_PATH" );
}

inline bool IsBlacklistedDirectory( const std::string& dirStr )
{
    for ( auto&& dir : CUSTOM_BLACKLISTED_DIRS )
    {
        if ( dirStr.find( dir.data(), dir.size() ) != std::string::npos )
        {
            return true;
        }
    }

    return false;
}

void FS_SetCustomPaths( IFileSystem* pFileSystem )
{
    fs::path customPath = FS_GetDataDirectory();
    customPath /= CUSTOM_DIRECTORY;

    fs::create_directories( customPath );

    for ( const auto& curDir : fs::directory_iterator( customPath ) )
    {
        const fs::path& curPath = curDir.path();
        std::string curPathStr = curPath.generic_string();

        if ( IsBlacklistedDirectory( curPathStr ) == true )
        {
            std::string errMsg = fmt::format( CUSTOM_DIRECTORY_BLACKLIST_MSG,
                                              curPathStr.data() );
            throw std::runtime_error( errMsg );
        }

        Log::Info( "Using directory {} for custom content\n", curPathStr );

        pFileSystem->AddSearchPath( curPathStr.data(), "GAME" );
        pFileSystem->AddSearchPath( curPathStr.data(), "MOD" );
    }
}