#ifndef _ENGINE_CSO2_SOCKETMANAGER_H_
#define _ENGINE_CSO2_SOCKETMANAGER_H_

#include <cstdint>

#ifdef _WIN32
#include <Windows.h>
#endif

constexpr const std::uint32_t ENGINE_CSO2_MAX_PACKETS = 256;

class Packet;

class SocketManager
{
public:
    virtual ~SocketManager();

    std::uint8_t _Unknown00[0x8];

    HWND m_WindowHandle;
    Packet* m_PacketParsers[ENGINE_CSO2_MAX_PACKETS];

    std::uint8_t _Unknown01[0x34];
};

static_assert( sizeof( SocketManager ) == 0x444 );

#endif  // _ENGINE_CSO2_SOCKETMANAGER_H_
