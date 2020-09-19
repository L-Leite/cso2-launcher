#include "hooks.hpp"
#include "hooks/lua/lua_hooks.hpp"
#include "platform.hpp"
#include "utilities.hpp"
#include "utilities/log.hpp"

// duplicated code with lua_server_hooks.cpp
// the module where the export is looked up must match the caller's module
// (in this case server.dll)
// can this be deduplicated?

#define server_lua_tostring( L, i ) server_lua_tolstring( L, ( i ), NULL )
#define server_luaL_loadbuffer( L, s, sz, n ) \
    server_luaL_loadbufferx( L, s, sz, n, NULL )
#define server_lua_pop( L, n ) server_lua_settop( L, -(n)-1 )

void server_lua_pushcclosure( lua_State* L, lua_CFunction fn, int n )
{
    using fn_t = void ( * )( lua_State*, lua_CFunction, int );
    auto modBase = Sys_GetModuleBase( "server.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "lua_pushcclosure" );
    pFn( L, fn, n );
}

const char* server_lua_tolstring( lua_State* L, int idx, size_t* len )
{
    using fn_t = const char* (*)( lua_State*, int, size_t* );
    auto modBase = Sys_GetModuleBase( "server.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "lua_tolstring" );
    return pFn( L, idx, len );
}

int server_lua_gettop( lua_State* L )
{
    using fn_t = int ( * )( lua_State* );
    auto modBase = Sys_GetModuleBase( "server.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "lua_gettop" );
    return pFn( L );
}

void server_lua_settop( lua_State* L, int idx )
{
    using fn_t = void ( * )( lua_State*, int );
    auto modBase = Sys_GetModuleBase( "server.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "lua_settop" );
    pFn( L, idx );
}

int server_luaL_loadbufferx( lua_State* L, const char* buff, size_t size,
                             const char* name, const char* mode )
{
    using fn_t =
        int ( * )( lua_State*, const char*, size_t, const char*, const char* );
    auto modBase = Sys_GetModuleBase( "server.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "luaL_loadbufferx" );
    return pFn( L, buff, size, name, mode );
}

int server_lua_pcall( lua_State* L, int nargs, int nresults, int errfunc )
{
    using fn_t = int ( * )( lua_State*, int, int, int );
    auto modBase = Sys_GetModuleBase( "server.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "lua_pcall" );
    return pFn( L, nargs, nresults, errfunc );
}

void server_lua_remove( lua_State* L, int idx )
{
    using fn_t = void ( * )( lua_State*, int );
    auto modBase = Sys_GetModuleBase( "server.dll" );
    auto pFn = Sys_GetModuleExport<fn_t>( modBase, "lua_remove" );
    pFn( L, idx );
}

namespace lua_tinker
{
int server_on_error( lua_State* L )
{
    auto szError = server_lua_tostring( L, -1 );
    Log::Error( "(server) lua_tinker::on_error: {}\n", szError );

    return 0;
}

void server_dobuffer( lua_State* L, const char* buff, size_t len )
{
    server_lua_pushcclosure( L, server_on_error, 0 );
    int errfunc = server_lua_gettop( L );

    if ( server_luaL_loadbuffer( L, buff, len, "lua_tinker::dobuffer()" ) == 0 )
    {
        server_lua_pcall( L, 0, 1, errfunc );
    }
    else
    {
        auto szError = server_lua_tostring( L, -1 );
        Log::Error( "(server) lua loadbuffer error: {}\n", szError );
    }

    server_lua_remove( L, errfunc );
    server_lua_pop( L, 1 );
}
}  // namespace lua_tinker

static std::unique_ptr<PLH::x86Detour> g_pLoadLuaFileHook;
static uint64_t g_LoadLuaFileOrig = 0;

NOINLINE bool __fastcall hkLoadServerLuaFile( lua_State* pLuaState,
                                              const std::string& szFilename )
{
    auto [bFileRead, vFileBuffer] = LoadLuaFileBuffer( szFilename );

    if ( bFileRead == false )
    {
        Log::Debug( "(server) LoadLuaFile failed. Could not open '{}'\n",
                    szFilename.c_str() );
        return false;
    }

    lua_tinker::server_dobuffer( pLuaState, vFileBuffer.data(),
                                 vFileBuffer.size() );

    Log::Debug( "(server) LoadLuaFile called. szFilename: '{}'\n",
                szFilename.c_str() );

    return true;
}

void ApplyLuaServerHooks( const uintptr_t dwServerBase )
{
    PLH::CapstoneDisassembler dis( PLH::Mode::x86 );

    g_pLoadLuaFileHook =
        SetupDetourHook( dwServerBase + 0x5D8510, &hkLoadServerLuaFile,
                         &g_LoadLuaFileOrig, dis );
    g_pLoadLuaFileHook->hook();
}
