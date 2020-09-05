#include "hooks.hpp"
#include "utilities.hpp"
#include "utilities/log.hpp"
#include "utilities/memorypatterns.hpp"

#include "source/tierlibs.hpp"

#include <filesystem/filesystem.hpp>
#include <scaleform/cso2_gfxfile.hpp>
#include <scaleform/sf_memoryheap.hpp>

Scaleform::MemoryHeap* GetGfxMemoryHeap()
{
    return *reinterpret_cast<Scaleform::MemoryHeap**>(
        MemoryPatterns::Singleton().GetPattern( "GfxMemoryHeap" ) );
}

CSO2GfxFile* CreateCSO2GfxFile( const char* szFilename,
                                FileHandle_t fileHandle )
{
    using fn_t =
        CSO2GfxFile*(__thiscall*)( CSO2GfxFile*, const char*, FileHandle_t );

    auto constructor = reinterpret_cast<fn_t>(
        MemoryPatterns::Singleton().GetPattern( "CSO2GfxFileConstructor" ) );
    auto pHeap = GetGfxMemoryHeap();

    return constructor( pHeap->Alloc<CSO2GfxFile>(), szFilename, fileHandle );
}

std::string_view GetFixedGfxPath( const char* purl )
{
    std::string_view urlView = purl;

    if ( urlView.length() > 1 && ( urlView[0] == '/' || urlView[0] == '\\' ) )
    {
        return urlView.substr( 1 );
    }

    return urlView;
}

static std::unique_ptr<PLH::x86Detour> g_pGfxOpenFileHook;
static uint64_t g_GfxOpenFileOrig = 0;

NOINLINE CSO2GfxFile* __fastcall hkCSO2GFxFileOpener_OpenFile(
    void* thisptr, void* edx, const char* purl, int flags, int mode )
{
    Log::Debug(
        "CSO2GFxFileOpener::OpenFile called. purl: '{}' flags: {} mode: {}\n",
        purl, flags, mode );

    auto newFilename = GetFixedGfxPath( purl );
    auto handle = g_pFullFileSystem->Open( newFilename.data(), "rb" );

    if ( handle == nullptr )
    {
        Log::Warning(
            "CSO2GFxFileOpener::OpenFile failed. Could not find file '{}'\n",
            newFilename.data() );
        return nullptr;
    }

    return CreateCSO2GfxFile( newFilename.data(), handle );
}

static std::unique_ptr<PLH::x86Detour> g_pGfxFileModTimeHook;
static uint64_t g_GfxFileModTimeOrig = 0;

NOINLINE int64_t __fastcall hkCSO2GFxFileOpener_GetFileModifyTime(
    void* thisptr, void* edx, const char* purl )
{
    Log::Debug( "CSO2GFxFileOpener::GetFileModifyTime called. purl: '{}'\n",
                purl );

    auto newFilename = GetFixedGfxPath( purl );
    bool fileExists = g_pFullFileSystem->FileExists( newFilename.data() );

    if ( fileExists == false )
    {
        Log::Warning( "CSO2GFxFileOpener::CSO2GFxFileOpener_GetFileModifyTime "
                      "failed. Could not find file {}\n",
                      newFilename.data() );
        return -1;
    }

    return 0;
}

void ApplyScaleformHooks( const uintptr_t dwClientBase )
{
    MemoryPatterns& patterns = MemoryPatterns::Singleton();

    PLH::CapstoneDisassembler dis( PLH::Mode::x86 );

    patterns.SetPattern( "CSO2GfxFileConstructor", dwClientBase + 0x2883E0 );
    patterns.SetPattern( "GfxMemoryHeap", dwClientBase + 0x1CDF0D0 );

    g_pGfxOpenFileHook =
        SetupDetourHook( dwClientBase + 0xA27700, &hkCSO2GFxFileOpener_OpenFile,
                         &g_GfxOpenFileOrig, dis );
    g_pGfxOpenFileHook->hook();

    g_pGfxFileModTimeHook = SetupDetourHook(
        dwClientBase + 0xA27420, &hkCSO2GFxFileOpener_GetFileModifyTime,
        &g_GfxFileModTimeOrig, dis );
    g_pGfxFileModTimeHook->hook();
}
