#include "filesystem.h"
#include <direct.h>
#include <io.h>
#include "convar.h"
#include "hooks.hpp"

std::string GetFullModPath( const char* pFileName )
{
    char buff[FILENAME_MAX];
    _getcwd( buff, FILENAME_MAX );
    std::string path( buff );
    path.append( "\\..\\custom\\" );
    path.append( pFileName );

    return path;
}

NOINLINE FileHandle_t __fastcall hkBFSOpenEx(
    void* ecx, void* edx, const char* pFileName, const char* pOptions,
    unsigned flags, const char* pathID, char** ppszResolvedFilename );

HOOK_DETOUR_DECLARE( BFSOpenExPos1 );
NOINLINE FileHandle_t __fastcall BFSOpenExPos1(
    void* ecx, void* edx, const char* pFileName, const char* pOptions,
    unsigned flags, const char* pathID, char** ppszResolvedFilename )
{
    return hkBFSOpenEx( ecx, edx, pFileName, pOptions, flags, pathID,
                        ppszResolvedFilename );
}

HOOK_DETOUR_DECLARE( BFSOpenExPos2 );
NOINLINE FileHandle_t __fastcall BFSOpenExPos2(
    void* ecx, void* edx, const char* pFileName, const char* pOptions,
    unsigned flags, const char* pathID, char** ppszResolvedFilename )
{
    return hkBFSOpenEx( ecx, edx, pFileName, pOptions, flags, pathID,
                        ppszResolvedFilename );
}

NOINLINE FileHandle_t __fastcall hkBFSOpenEx(
    void* ecx, void* edx, const char* pFileName, const char* pOptions,
    unsigned flags, const char* pathID, char** ppszResolvedFilename )
{
    std::string modPath = GetFullModPath( pFileName );

    if ( _access( modPath.c_str(), 0 ) != -1 )
    {
        printf( "OpenEx Hook: %s to %s\n", pFileName, modPath.c_str() );

        return HOOK_DETOUR_GET_ORIG( BFSOpenExPos2 )( ecx, edx, modPath.c_str(),
                                                      pOptions, flags, pathID,
                                                      ppszResolvedFilename );
    }

    return HOOK_DETOUR_GET_ORIG( BFSOpenExPos2 )(
        ecx, edx, pFileName, pOptions, flags, pathID,
        ppszResolvedFilename );  // the ReadFileEx uses the 2nd OpenEx IDK
}

ON_LOAD_LIB( filesystem_stdio )
{
    if ( _access( GetFullModPath( "." ).c_str(), 0 ) != -1 ) // dont do hook if custom folder is not exists
    {
        uintptr_t dwFileSystemBase = GET_LOAD_LIB_MODULE();

        // there are 2 "CBaseFileSystem::OpenEx"s
        HOOK_DETOUR( dwFileSystemBase + 0x46C60, BFSOpenExPos1 );
        HOOK_DETOUR( dwFileSystemBase + 0x46D90, BFSOpenExPos2 );
    }
}