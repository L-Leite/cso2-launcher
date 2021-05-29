#ifndef _ENGINE_CSO2_PACKET_
#define _ENGINE_CSO2_PACKET_

#include <cstdint>
#include <list>

class Packet
{
public:
    Packet() : _Unknown01(nullptr), _Unknown02(nullptr) {}

    virtual ~Packet() = default;
    virtual void Parse(std::uint8_t* buffer, std::uint32_t bufferLength) = 0;
    virtual void Unknown02() = 0;  // does nothing on most packets
    virtual bool Unknown03() = 0;  // returns true on most packets

    std::list<Packet*> _Unknown00;
    Packet* _Unknown01;
    Packet* _Unknown02;
};

static_assert(sizeof(Packet) == 0x14);

#endif  // _ENGINE_CSO2_PACKET_
