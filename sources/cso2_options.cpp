#include "cso2_options.hpp"

#include "utilities.hpp"
#include "utilities/log.hpp"

extern uintptr_t g_dwEngineBase;

namespace cso2
{
//
// this function sets game if the game should connect to its peers through
// direct connections or if its traffic should be passed through the master
// server's relay
//
void SetUseUdpRelay(bool enabled)
{
    Log::Debug("[SetUseUdpRelay] new state: {}\n", enabled);

    if (enabled == true)
    {
        // mov dword ptr [eax], 5
        const std::array<uint8_t, 6> relayPatch = { 0xC7, 0x00, 0x05,
                                                    0x00, 0x00, 0x00 };
        utils::WriteProtectedMemory(relayPatch, g_dwEngineBase + 0x2BE552);
        // mov dword ptr [eax+8], 5
        const std::array<uint8_t, 7> relayPatch2 = { 0xC7, 0x40, 0x08, 0x05,
                                                     0x00, 0x00, 0x00 };
        utils::WriteProtectedMemory(relayPatch2, g_dwEngineBase + 0x2BE56C);
        // mov dword ptr [eax+4], 5
        const std::array<uint8_t, 7> relayPatch3 = { 0xC7, 0x40, 0x04, 0x05,
                                                     0x00, 0x00, 0x00 };
        utils::WriteProtectedMemory(relayPatch3, g_dwEngineBase + 0x2BE587);
    }
    else
    {
        // mov dword ptr [eax], 2
        const std::array<uint8_t, 6> relayPatch = { 0xC7, 0x00, 0x02,
                                                    0x00, 0x00, 0x00 };
        utils::WriteProtectedMemory(relayPatch, g_dwEngineBase + 0x2BE552);
        // mov dword ptr [eax+8], 2
        const std::array<uint8_t, 7> relayPatch2 = { 0xC7, 0x40, 0x08, 0x02,
                                                     0x00, 0x00, 0x00 };
        utils::WriteProtectedMemory(relayPatch2, g_dwEngineBase + 0x2BE56C);
        // mov dword ptr [eax+4], 2
        const std::array<uint8_t, 7> relayPatch3 = { 0xC7, 0x40, 0x04, 0x02,
                                                     0x00, 0x00, 0x00 };
        utils::WriteProtectedMemory(relayPatch3, g_dwEngineBase + 0x2BE587);
    }
}
}  // namespace cso2
