#include "hooks.hpp"
#include "hooks/lua/lua_hooks.hpp"
#include "platform.hpp"
#include "source/tierlibs.hpp"
#include "utilities.hpp"
#include "utilities/log.hpp"

#include <filesystem/filesystem.hpp>

#include <fmt/format.h>

// duplicated code with lua_server_hooks.cpp
// the module where the export is looked up must match the caller's module
// (in this case client.dll)
// can this be deduplicated?

#define client_lua_tostring( L, i ) client_lua_tolstring( L, ( i ), NULL )
#define client_luaL_loadbuffer( L, s, sz, n ) \
    client_luaL_loadbufferx( L, s, sz, n, NULL )
#define client_lua_pop( L, n ) client_lua_settop( L, -(n)-1 )

void client_lua_pushcclosure( lua_State* L, lua_CFunction fn, int n )
{
    using fn_t = void ( * )( lua_State*, lua_CFunction, int );
    auto modBase = Sys_GetModuleBase( "client.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "lua_pushcclosure" );
    pFn( L, fn, n );
}

const char* client_lua_tolstring( lua_State* L, int idx, size_t* len )
{
    using fn_t = const char* (*)( lua_State*, int, size_t* );
    auto modBase = Sys_GetModuleBase( "client.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "lua_tolstring" );
    return pFn( L, idx, len );
}

int client_lua_gettop( lua_State* L )
{
    using fn_t = int ( * )( lua_State* );
    auto modBase = Sys_GetModuleBase( "client.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "lua_gettop" );
    return pFn( L );
}

void client_lua_settop( lua_State* L, int idx )
{
    using fn_t = void ( * )( lua_State*, int );
    auto modBase = Sys_GetModuleBase( "client.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "lua_settop" );
    pFn( L, idx );
}

int client_luaL_loadbufferx( lua_State* L, const char* buff, size_t size,
                             const char* name, const char* mode )
{
    using fn_t =
        int ( * )( lua_State*, const char*, size_t, const char*, const char* );
    auto modBase = Sys_GetModuleBase( "client.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "luaL_loadbufferx" );
    return pFn( L, buff, size, name, mode );
}

int client_lua_pcall( lua_State* L, int nargs, int nresults, int errfunc )
{
    using fn_t = int ( * )( lua_State*, int, int, int );
    auto modBase = Sys_GetModuleBase( "client.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "lua_pcall" );
    return pFn( L, nargs, nresults, errfunc );
}

void client_lua_remove( lua_State* L, int idx )
{
    using fn_t = void ( * )( lua_State*, int );
    auto modBase = Sys_GetModuleBase( "client.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "lua_remove" );
    pFn( L, idx );
}

namespace lua_tinker
{
int client_on_error( lua_State* L )
{
    auto szError = client_lua_tostring( L, -1 );
    Log::Error( "(client) lua_tinker::on_error: {}\n", szError );

    return 0;
}

void client_dobuffer( lua_State* L, const char* buff, size_t len )
{
    client_lua_pushcclosure( L, client_on_error, 0 );
    int errfunc = client_lua_gettop( L );

    if ( client_luaL_loadbuffer( L, buff, len, "lua_tinker::dobuffer()" ) == 0 )
    {
        client_lua_pcall( L, 0, 1, errfunc );
    }
    else
    {
        auto szError = client_lua_tostring( L, -1 );
        Log::Error( "(client) lua loadbuffer error: {}\n", szError );
    }

    client_lua_remove( L, errfunc );
    client_lua_pop( L, 1 );
}
}  // namespace lua_tinker

static std::unique_ptr<PLH::x86Detour> g_pLoadLuaFileHook;
static uint64_t g_LoadLuaFileOrig = 0;

NOINLINE bool __fastcall hkClientLoadLuaFile( lua_State* pLuaState,
                                              const std::string& szFilename )
{
    auto [bFileRead, vFileBuffer] = LoadLuaFileBuffer( szFilename );

    if ( bFileRead == false )
    {
        Log::Debug( "(client) LoadLuaFile failed. Could not open '{}'\n",
                    szFilename.c_str() );
        return false;
    }

    lua_tinker::client_dobuffer( pLuaState, vFileBuffer.data(),
                                 vFileBuffer.size() );

    Log::Debug( "(client) LoadLuaFile called. szFilename: '{}'\n",
                szFilename.c_str() );

    return true;
}

static std::unique_ptr<PLH::x86Detour> g_pGamemodeExistHook;
static uint64_t g_GamemodeExistOrig = 0;

//
// this function is optimized to use the __fastcall call convention
// but the caller is cleaning up the stack (like it's a __cdecl call)...
//
NOINLINE bool        /* __fastcall */
hkDoesGamemodeExist( /* int16_t modType,  const char* modName, */
                     const char* mapName, uint8_t level )
{
    int16_t modType;
    const char* modName;

    __asm {
        mov modType, cx;
        mov modName, edx;
    }

    std::string szTargetPath;
    szTargetPath.reserve( 260 );

    if ( modType == 1 )
    {
        szTargetPath = fmt::format( "scripts/mod/{}/{}/{}.lua", modName,
                                    mapName, mapName );
    }
    else if ( modType == 2 )
    {
        szTargetPath = fmt::format( "scripts/mod/{}/{}_lvl{}/{}_lvl{}.lua",
                                    modName, mapName, level, mapName, level );
    }
    else
    {
        szTargetPath =
            fmt::format( "scripts/mod/{}/mod_{}.lua", modName, modName );
    }

    Log::Debug( "DoesGamemodeExist called. szTargetPath: '{}'\n",
                szTargetPath.c_str() );

    bool doesLuaExist =
        g_pFullFileSystem->FileExists( szTargetPath.c_str(), "MOD" );

    if ( doesLuaExist == true )
    {
        Log::Debug( "DoesGamemodeExist: '{}' is a lua source code file\n",
                    szTargetPath.c_str() );
        return true;
    }

    Log::Debug( "DoesGamemodeExist: couldn't find lua source file '{}', trying "
                "to find a compiled lua file...\n",
                szTargetPath.c_str() );

    szTargetPath.replace( szTargetPath.find( ".lua" ), 4, ".lo" );
    bool doesLoExist =
        g_pFullFileSystem->FileExists( szTargetPath.c_str(), "MOD" );

    if ( doesLoExist == false )
    {
        Log::Debug( "DoesGamemodeExist: couldn't find '{}'\n",
                    szTargetPath.c_str() );
        return false;
    }

    Log::Debug( "DoesGamemodeExist: '{}' is a lua compiled file\n",
                szTargetPath.c_str() );

    return true;
}

void ApplyLuaClientHooks( const uintptr_t dwClientBase )
{
    PLH::CapstoneDisassembler dis( PLH::Mode::x86 );

    g_pLoadLuaFileHook =
        SetupDetourHook( dwClientBase + 0xB0B9B0, &hkClientLoadLuaFile,
                         &g_LoadLuaFileOrig, dis );
    g_pLoadLuaFileHook->hook();

    g_pGamemodeExistHook =
        SetupDetourHook( dwClientBase + 0x7C1660, &hkDoesGamemodeExist,
                         &g_GamemodeExistOrig, dis );
    g_pGamemodeExistHook->hook();
}
