#include <array>
#include <filesystem>

#include <direct.h>
#include <io.h>

#include <tier0/icommandline.hpp>

#include "hooks.hpp"
#include "utilities.hpp"
#include "utilities/log.hpp"
#include "utilities/memorypatterns.hpp"

std::filesystem::path GetCustomPath( const char* pFileName )
{
    char buff[FILENAME_MAX];
    _getcwd( buff, FILENAME_MAX );
    std::filesystem::path path( buff );

    while ( *pFileName == '/' || *pFileName == '\\' ||
            *pFileName == '*' )  // Clean path
        pFileName++;

    path /= "..\\custom";
    path /= pFileName;

    return path;
}

static std::unique_ptr<PLH::x86Detour> g_pOpenExHook;
static uint64_t g_OpenExOrig = 0;

NOINLINE void* __fastcall hkFsOpenEx( void* ecx, void* edx,
                                      const char* pFileName,
                                      const char* pOptions, unsigned flags,
                                      const char* pathID,
                                      char** ppszResolvedFilename )
{
    auto path = GetCustomPath( pFileName );

    std::string_view curFile( pFileName );

    if ( std::filesystem::exists( path ) )
    {
        std::cout << "Loading custom file %s" << path << ".\n";
        curFile = path.string();
    }

    return PLH::FnCast( g_OpenExOrig, hkFsOpenEx )( ecx, edx, curFile.data(),
                                                    pOptions, flags, pathID,
                                                    ppszResolvedFilename );
}

static std::unique_ptr<PLH::x86Detour> g_pOpenExGfxHook;
static uint64_t g_OpenExGfxOrig = 0;

NOINLINE void* __fastcall hkFsOpenExGfx( void* ecx, void* edx,
                                         const char* pFileName,
                                         const char* pOptions, unsigned flags,
                                         const char* pathID,
                                         char** ppszResolvedFilename )
{
    return hkFsOpenEx( ecx, edx, pFileName, pOptions, flags, pathID,
                       ppszResolvedFilename );
}

const char* path = 0;

static std::unique_ptr<PLH::x86Detour> g_pFileEncHook;
static uint64_t g_FileEncOrig = 0;

NOINLINE bool __fastcall hkIsFileEncrypted( void* ecx, void* edx )
{
    if ( std::filesystem::exists( GetCustomPath( path ) ) )
    {
        std::cout << "Loading decrypted file %s" << path << ".\n";
        return false;
    }

    return PLH::FnCast( g_FileEncOrig, hkIsFileEncrypted )( ecx, edx );
}

static std::unique_ptr<PLH::x86Detour> g_pGetFileExtHook;
static uint64_t g_GetFileExtOrig = 0;

//
// this will be called before call the IsFileEncrypted, and ecx is the pointer
// of file name.
//
NOINLINE int __fastcall hkGetFileExt( const char** ecx, void* edx,
                                      const char* file, size_t path_size,
                                      size_t ext_size )
{
    path = *ecx;
    return PLH::FnCast( g_GetFileExtOrig, hkGetFileExt )( ecx, edx, file,
                                                          path_size, ext_size );
}

void BytePatchFilesystem()
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
    //
    if ( CommandLine()->CheckParm( "-decryptedfiles", nullptr ) )
    {
        MemoryPatterns& patterns = MemoryPatterns::Singleton();
        const uintptr_t fileEncAddr =
            patterns.GetPattern( "FsIsFileEncrypted" );

        if ( fileEncAddr != 0 )
        {
            // mov al, 0; retn
            const std::array<uint8_t, 5> encryptPatch = { 0xB0, 0x00, 0xC3 };
            utils::WriteProtectedMemory( encryptPatch, fileEncAddr );
        }
    }
}

bool LookupFSAddresses()
{
    Log::Debug( "Looking up addresses in filesystem_stdio.dll...\n" );

    int results = 0;

    MemoryPatterns& patterns = MemoryPatterns::Singleton();

    results += !patterns.AddPattern(
        "\xE8\xCC\xCC\xCC\xCC\x8B\xF0\x89\x74\x24\x18", "FsOpenEx",
        IMemoryPatternsOptions( -1, 1, 5, "filesystem_stdio.dll", 0, true ) );

    results += !patterns.AddPattern(
        "\x81\xEC\xCC\xCC\xCC\xCC\x56\x8B\xB4\x24\xCC\xCC\xCC\xCC\x57\x8B\xF9"
        "\x85\xF6\x75\x0D\x5F\x33\xC0\x5E\x81\xC4\xCC\xCC\xCC\xCC\xC2\x14\x00",
        "FsOpenExGfx",
        IMemoryPatternsOptions( -1, -1, -1, "filesystem_stdio.dll" ) );

    results += !patterns.AddPattern(
        "\x56\x8B\xF1\x57\x83\x7E\x14\x10\x8B\x7E\x10", "FsIsFileEncrypted",
        IMemoryPatternsOptions( -1, -1, -1, "filesystem_stdio.dll" ) );

    results += !patterns.AddPattern(
        "\x51\x56\xFF\x74\x24\x14\x8B\x74\x24\x10\xFF\x74\x24\x14",
        "FsGetFileExt",
        IMemoryPatternsOptions( -1, -1, -1, "filesystem_stdio.dll" ) );

    const bool foundAllAddresses = results == 0;

    if ( foundAllAddresses == true )
    {
        Log::Debug( "Looked up filesystem_stdio.dll addresses successfully\n" );
    }
    else
    {
        Log::Error( "Failed to find {} filesystem_stdio.dll addresses\n",
                    results );
    }

    return foundAllAddresses;
}

void OnFileSystemLoaded()
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
        return;

    bHasLoaded = true;

    LookupFSAddresses();

    MemoryPatterns& patterns = MemoryPatterns::Singleton();
    PLH::CapstoneDisassembler dis( PLH::Mode::x86 );

    if ( CommandLine()->CheckParm( "-enablecustom", nullptr ) &&
         std::filesystem::exists( GetCustomPath( "" ) ) )
    {
        const uintptr_t fsOpenExAddr = patterns.GetPattern( "FsOpenEx" );

        if ( fsOpenExAddr != 0 )
        {
            g_pOpenExHook = SetupDetourHook( fsOpenExAddr, &hkFsOpenEx,
                                             &g_OpenExOrig, dis );
            g_pOpenExHook->hook();
        }

        const uintptr_t fsOpenExGfxAddr = patterns.GetPattern( "FsOpenExGfx" );

        if ( fsOpenExGfxAddr != 0 )
        {
            g_pOpenExGfxHook = SetupDetourHook( fsOpenExGfxAddr, &hkFsOpenExGfx,
                                                &g_OpenExGfxOrig, dis );
            g_pOpenExGfxHook->hook();
        }

        const uintptr_t fileEncAddr =
            patterns.GetPattern( "FsIsFileEncrypted" );

        if ( fileEncAddr != 0 )
        {
            g_pFileEncHook = SetupDetourHook( fileEncAddr, &hkIsFileEncrypted,
                                              &g_FileEncOrig, dis );
            g_pFileEncHook->hook();
        }

        const uintptr_t fileExtAddr = patterns.GetPattern( "FsGetFileExt" );

        if ( fileExtAddr != 0 )
        {
            g_pGetFileExtHook = SetupDetourHook( fileExtAddr, &hkGetFileExt,
                                                 &g_GetFileExtOrig, dis );

            g_pGetFileExtHook->hook();
        }
    }
    else
    {
        BytePatchFilesystem();
    }
}
