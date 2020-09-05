#include "console.hpp"
#include "hooks.hpp"
#include "utilities/log.hpp"
#include "utilities/memorypatterns.hpp"

//
// this is D3DDeviceWrapper::m_pD3DDevice (an IDirect3DDevice9). it's the first
// member variable of the class there is only one instance of D3DDeviceWrapper,
// and it starts in this address
//
LPDIRECT3DDEVICE9 GetD3dDevice()
{
    MemoryPatterns& patterns = MemoryPatterns::Singleton();
    return *reinterpret_cast<LPDIRECT3DDEVICE9*>(
        patterns.GetPattern( "ShaderD3DDevice" ) );
}

static std::unique_ptr<PLH::VTableSwapHook> g_pDeviceHook;
static PLH::VFuncMap g_DeviceOrig;

NOINLINE HRESULT WINAPI hkReset(
    LPDIRECT3DDEVICE9 thisptr, D3DPRESENT_PARAMETERS* pPresentationParameters )
{
    g_GameConsole.OnPreReset();
    const HRESULT result = PLH::FnCast( g_DeviceOrig.at( 16 ), &hkReset )(
        thisptr, pPresentationParameters );
    g_GameConsole.OnPostReset();
    return result;
}

NOINLINE HRESULT WINAPI hkEndScene( LPDIRECT3DDEVICE9 thisptr )
{
    g_GameConsole.OnEndScene();
    return PLH::FnCast( g_DeviceOrig.at( 42 ), &hkEndScene )( thisptr );
}

static std::unique_ptr<PLH::x86Detour> g_pCreateDeviceHook;
static uint64_t g_CreateDeviceOrig = 0;

NOINLINE bool __fastcall hkCreateD3DDevice(
    void* thisptr, void* edx, void* pHWnd, int nAdapter,
    const struct ShaderDeviceInfo_t& info )
{
    const bool res = PLH::FnCast( g_CreateDeviceOrig, &hkCreateD3DDevice )(
        thisptr, edx, pHWnd, nAdapter, info );

    // if the device was created successfully
    if ( res )
    {
        LPDIRECT3DDEVICE9 pDevice = GetD3dDevice();
        g_GameConsole.Init( pDevice, reinterpret_cast<HWND>( pHWnd ) );

        // hook the game's d3d device endscene (its virtual function table index
        // is 42)
        static const PLH::VFuncMap deviceRedirects = {
            { uint16_t( 16 ), reinterpret_cast<uint64_t>( &hkReset ) },
            { uint16_t( 42 ), reinterpret_cast<uint64_t>( &hkEndScene ) }
        };
        g_pDeviceHook = SetupVtableSwap( pDevice, deviceRedirects );
        g_pDeviceHook->hook();
        g_DeviceOrig = g_pDeviceHook->getOriginals();
    }

    return res;
}

bool LookupShaderApiAddresses()
{
    Log::Debug( "Looking up addresses in shaderapidx9.dll...\n" );

    int results = 0;

    MemoryPatterns& patterns = MemoryPatterns::Singleton();

    results += !patterns.AddPattern(
        "\x81\xEC\xCC\xCC\xCC\xCC\x53\x55\x56\x57\x6A\x01",
        "ShaderCreateD3DDevice",
        IMemoryPatternsOptions( -1, -1, -1, "shaderapidx9.dll" ) );

    results += !patterns.AddPattern(
        "\x89\x1D\xCC\xCC\xCC\xCC\xFF\x15", "ShaderD3DDevice",
        IMemoryPatternsOptions( -1, 2, -1, "shaderapidx9.dll" ) );

    const bool foundAllAddresses = results == 0;

    if ( foundAllAddresses == true )
    {
        Log::Debug( "Looked up shaderapidx9.dll addresses successfully\n" );
    }
    else
    {
        Log::Error( "Failed to find {} shaderapidx9.dll addresses\n", results );
    }

    return foundAllAddresses;
}

void OnShaderApiLoaded()
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

    LookupShaderApiAddresses();

    MemoryPatterns& patterns = MemoryPatterns::Singleton();

    PLH::CapstoneDisassembler dis( PLH::Mode::x86 );

    const uintptr_t createDeviceAddr =
        patterns.GetPattern( "ShaderCreateD3DDevice" );

    if ( createDeviceAddr != 0 )
    {
        g_pCreateDeviceHook = SetupDetourHook(
            createDeviceAddr, &hkCreateD3DDevice, &g_CreateDeviceOrig, dis );
        g_pCreateDeviceHook->hook();
    }
}
