#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

class lua_State;
using lua_CFunction = int ( * )( lua_State* L );

#define lua_tostring( L, i ) lua_tolstring( L, ( i ), NULL )
#define luaL_loadbuffer( L, s, sz, n ) luaL_loadbufferx( L, s, sz, n, NULL )
#define lua_pop( L, n ) lua_settop( L, -(n)-1 )

/* void lua_pushcclosure( lua_State* L, lua_CFunction fn, int n );
const char* lua_tolstring( lua_State* L, int idx, size_t* len );
int lua_gettop( lua_State* L );
void lua_settop( lua_State* L, int idx );
int luaL_loadbufferx( lua_State* L, const char* buff, size_t size,
                      const char* name, const char* mode );
int lua_pcall( lua_State* L, int nargs, int nresults, int errfunc );
void lua_remove( lua_State* L, int idx );

namespace lua_tinker
{
int on_error( lua_State* L );
void dobuffer( lua_State* L, const char* buff, size_t len );
}  // namespace lua_tinker */

std::pair<bool, std::vector<char>> LoadLuaFileBuffer(
    std::string_view szvFilename );