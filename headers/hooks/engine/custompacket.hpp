#ifndef _HOOKS_ENGINE_CUSTOMPACKET_H_
#define _HOOKS_ENGINE_CUSTOMPACKET_H_

#include <engine/cso2/packet.hpp>

constexpr const std::uint8_t CUSTOM_PACKET_ID = 254;

class BufferView;

class CustomPacket : public Packet
{
public:
    CustomPacket();

    virtual ~CustomPacket();
    virtual void Parse(std::uint8_t* buffer,
                       std::uint32_t bufferLength) override;
    virtual void Unknown02() override;
    virtual bool Unknown03() override;

private:
    void OnSetUseRelay(BufferView& view);
};

#endif  // _HOOKS_ENGINE_CUSTOMPACKET_H_
