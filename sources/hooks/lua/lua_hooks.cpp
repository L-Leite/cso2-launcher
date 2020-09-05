
#include "hooks/lua/lua_hooks.hpp"
#include "source/tierlibs.hpp"
#include "utilities/log.hpp"

#include <filesystem/filesystem.hpp>

std::string GetFixedLuaFilename( std::string_view szOrigFilename )
{
    std::string newFilename( szOrigFilename );
    const auto extPos = szOrigFilename.find( ".lua" );

    if ( extPos != std::string_view::npos )
    {
        newFilename.replace( extPos + 1, 3, "lo" );
    }

    return newFilename;
}

std::pair<bool, std::vector<char>> LoadLuaFileBuffer(
    std::string_view szvFilename )
{
    bool doesLuaExist =
        g_pFullFileSystem->FileExists( szvFilename.data(), "GAME" );

    std::string targetFilename;

    if ( doesLuaExist == false )
    {
        targetFilename = GetFixedLuaFilename( szvFilename );

        bool doesLoExist =
            g_pFullFileSystem->FileExists( targetFilename.c_str(), "GAME" );

        if ( doesLoExist == false )
        {
            return { false, {} };
        }

        Log::Debug( "LoadLuaFileBuffer: using compiled lua file '{}'\n",
                    targetFilename.c_str() );
    }
    else
    {
        targetFilename = szvFilename;
        Log::Debug( "LoadLuaFileBuffer: using source lua file '{}'\n",
                    targetFilename.c_str() );
    }

    FileHandle_t handle =
        g_pFullFileSystem->Open( targetFilename.c_str(), "rb", "GAME" );

    if ( handle == nullptr )
    {
        return { false, {} };
    }

    const auto fileSize = g_pFullFileSystem->Size( handle );
    std::vector<char> buffer( fileSize );

    const auto bytesRead =
        g_pFullFileSystem->Read( buffer.data(), buffer.size(), handle );

    if ( bytesRead != fileSize )
    {
        return { false, {} };
    }

    return { true, std::move( buffer ) };
}