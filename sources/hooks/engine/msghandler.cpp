#include "hooks.hpp"

#include <engine/cso2/socketmanager.hpp>
#include "hooks/engine/custompacket.hpp"

extern uintptr_t g_dwEngineBase;

SocketManager* GetSocketManager()
{
    static std::uintptr_t objAddr = 0;

    if (!objAddr)
    {
        objAddr = g_dwEngineBase + 0x19025BC;
    }

    return *reinterpret_cast<SocketManager**>(objAddr);
}

static std::unique_ptr<PLH::x86Detour> g_pMsgHandlerInitHook;
static uint64_t g_MsgHandlerInitOrig = 0;

NOINLINE void __fastcall hkMsgHandlerInit(void* thisptr, void* edx,
                                          HWND windowHandle)
{
    PLH::FnCast(g_MsgHandlerInitOrig, &hkMsgHandlerInit)(thisptr, edx,
                                                         windowHandle);

    auto socketManager = GetSocketManager();

    socketManager->m_PacketParsers[CUSTOM_PACKET_ID] = new CustomPacket();
}

void ApplyEngineMsgHandlerHooks(const std::uintptr_t dwEngineBase)
{
    auto dis = HookDisassembler();

    g_pMsgHandlerInitHook = SetupDetourHook(
        dwEngineBase + 0x282F00, &hkMsgHandlerInit, &g_MsgHandlerInitOrig, dis);
    g_pMsgHandlerInitHook->hook();
}
