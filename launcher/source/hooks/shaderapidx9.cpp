#include "stdafx.hpp"

#include "console.hpp"
#include "hooks.hpp"

//
// this is D3DDeviceWrapper::m_pD3DDevice (an IDirect3DDevice9). it's the first
// member variable of the class there is only one instance of D3DDeviceWrapper,
// and it starts in this address
//
LPDIRECT3DDEVICE9 GetD3dDevice( const uintptr_t base )
{
    return *reinterpret_cast<LPDIRECT3DDEVICE9*>( base + 0x13E184 );
}

using fnConColorPrint_t = HRESULT( WINAPI* )( LPDIRECT3DDEVICE9 );
static std::unique_ptr<PLH::VTableSwapHook> g_pDeviceHook;
static PLH::VFuncMap g_EndSceneOrig;

NOINLINE HRESULT WINAPI hkEndScene( LPDIRECT3DDEVICE9 pDevice )
{
    g_GameConsole.OnEndScene();
    return PLH::FnCast( g_EndSceneOrig.at( 42 ), &hkEndScene )( pDevice );
}

static std::unique_ptr<PLH::x86Detour> g_pCreateDeviceHook;
static uint64_t g_CreateDeviceOrig = NULL;

NOINLINE bool __fastcall hkCreateD3DDevice(
    void* thisptr, void* edx, void* pHWnd, int nAdapter,
    const struct ShaderDeviceInfo_t& info )
{
    const bool res = PLH::FnCast( g_CreateDeviceOrig, &hkCreateD3DDevice )(
        thisptr, edx, pHWnd, nAdapter, info );

    // if the device was created successfully
    if ( res )
    {
        LPDIRECT3DDEVICE9 pDevice =
            GetD3dDevice( utils::GetModuleBase( "shaderapidx9.dll" ) );
        g_GameConsole.Init( pDevice, reinterpret_cast<HWND>( pHWnd ) );

        // hook the game's d3d device endscene (its virtual function table index
        // is 42)
        static const PLH::VFuncMap deviceRedirects = {
            { uint16_t( 42 ), reinterpret_cast<uint64_t>( &hkEndScene ) }
        };
        g_pDeviceHook = std::make_unique<PLH::VTableSwapHook>(
            reinterpret_cast<uint64_t>( pDevice ), deviceRedirects );
        g_pDeviceHook->hook();
        g_EndSceneOrig = g_pDeviceHook->getOriginals();
    }

    return res;
}

void OnShaderApiLoaded( const uintptr_t dwShaderApiBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

    PLH::CapstoneDisassembler hookDisasm( PLH::Mode::x86 );

    g_pCreateDeviceHook =
        SetupDetourHook( dwShaderApiBase + 0x551F0, &hkCreateD3DDevice,
                         &g_CreateDeviceOrig, hookDisasm );
    g_pCreateDeviceHook->hook();
}
