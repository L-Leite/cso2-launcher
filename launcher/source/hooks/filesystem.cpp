#include "stdafx.hpp"

#include <direct.h>
#include <io.h>
#include "filesystem.h"
#include "hooks.hpp"
#include "tier0/ICommandLine.h"

std::filesystem::path GetCustomPath( const char* pFileName )
{
    char buff[FILENAME_MAX];
    _getcwd( buff, FILENAME_MAX );
    std::filesystem::path path( buff );

	while ( *pFileName == '/' || *pFileName == '\\' || *pFileName == '*' ) // Clean path
        pFileName++;

    path /= "..\\custom";
	path /= pFileName;

    return path;
}

void BytePatchFilesystem( const uintptr_t dwFilesystemBase )
{
    //
    // IsFileEncrypted (1006A3B0, rva 6A3B0 @ FileSystem_Stdio.dll)
    // if the requested file's extension contains 'txt', 'csv', 'cfg' or 'db',
    // then it's an encrypted file ('etxt', 'ecsv', 'ecfg' or 'edb') - Question:
    // how about elo? and returns true
    //
    // this patch makes the function return false all the time
    // WARNING: encrypted files no longer work with this
    //
    // only do this if the user passes the '-decryptedfiles' cmd argument
    if ( CommandLine()->CheckParm( "-decryptedfiles", nullptr ) )
    {
        // mov al, 1; retn
        const std::array<uint8_t, 5> encryptPatch = { 0xB0, 0x00, 0xC3 };
        utils::WriteProtectedMemory( encryptPatch,
                                     ( dwFilesystemBase + 0x6A3B0 ) );
    }
}

HOOK_DETOUR_DECLARE( BaseFileSystem_OpenEx );
HOOK_DETOUR_DECLARE( BaseFileSystem_OpenExGfx );

NOINLINE FileHandle_t __fastcall BaseFileSystem_OpenEx(
    void* ecx, void* edx, const char* pFileName, const char* pOptions,
    unsigned flags, const char* pathID, char** ppszResolvedFilename )
{
    auto path = GetCustomPath( pFileName );

	if (std::filesystem::exists(path))
	{
        printf( "Loading from %s.\n", path.string().c_str() );
        return HOOK_DETOUR_GET_ORIG( BaseFileSystem_OpenEx )(
            ecx, edx, path.string().c_str(), pOptions, flags, pathID,
            ppszResolvedFilename );
	}

    return HOOK_DETOUR_GET_ORIG( BaseFileSystem_OpenEx )(
        ecx, edx, pFileName, pOptions, flags, pathID, ppszResolvedFilename );
}

NOINLINE FileHandle_t __fastcall BaseFileSystem_OpenExGfx(
	void* ecx, void* edx, const char* pFileName, const char* pOptions,
	unsigned flags, const char* pathID, char** ppszResolvedFilename)
{
    return BaseFileSystem_OpenEx( ecx, edx, pFileName, pOptions, flags, pathID,
                                  ppszResolvedFilename );
}

const char* path = 0;

HOOK_DETOUR_DECLARE( hkIsFileEncrypted );

NOINLINE bool __fastcall hkIsFileEncrypted( void* ecx, void* edx )
{
	if (std::filesystem::exists(GetCustomPath(path)))
	{
        printf( "Loading decrypted %s ...\n", path );
        return false;
	}

    return HOOK_DETOUR_GET_ORIG( hkIsFileEncrypted )( ecx, edx );
}

// this will be called before call the IsFileEncrypted, and ecx is the pointer of file name.
HOOK_DETOUR_DECLARE( hkGetFileExt );
NOINLINE int __fastcall hkGetFileExt( const char** ecx, void* edx,
                                      const char* file, size_t path_size,
                                      size_t ext_size )
{
    path = *ecx;
    return HOOK_DETOUR_GET_ORIG( hkGetFileExt )( ecx, edx, file, path_size,
                                                 ext_size );
}

void OnFileSystemLoaded( const uintptr_t dwFilesystemBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
        return;

    bHasLoaded = true;

    if ( CommandLine()->CheckParm( "-enablecustom", nullptr ) &&
         std::filesystem::exists( GetCustomPath( "" ) ) )
    {
        HOOK_DETOUR( dwFilesystemBase + 0x46D90, BaseFileSystem_OpenEx );    
		HOOK_DETOUR( dwFilesystemBase + 0x46C60, BaseFileSystem_OpenExGfx );

        HOOK_DETOUR( dwFilesystemBase + 0x6A3B0, hkIsFileEncrypted );
        HOOK_DETOUR( dwFilesystemBase + 0x6A4E0, hkGetFileExt );
    }
    else
    {
        BytePatchFilesystem( dwFilesystemBase );
    }
}
