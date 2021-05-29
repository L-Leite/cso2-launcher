#include "hooks/engine/custompacket.hpp"

#include "buffer/view.hpp"
#include "cso2_options.hpp"
#include "utilities/log.hpp"

enum class CustomPacketTypes : std::uint8_t
{
    SetUseRelay = 0
};

CustomPacket::CustomPacket() {}

CustomPacket::~CustomPacket() {}

void CustomPacket::Parse(std::uint8_t* buffer, std::uint32_t bufferLength)
{
    try
    {
        BufferView view(buffer, bufferLength);

        auto type = view.Read<CustomPacketTypes>();

        switch (type)
        {
            case CustomPacketTypes::SetUseRelay:
                this->OnSetUseRelay(view);
                break;
            default:
                Log::Warning(
                    "[CustomPacket::Parse] unknown packet type {} used\n",
                    type);
                break;
        }
    }
    catch (const std::exception& e)
    {
        Log::Warning("[CustomPacket::Parse] threw {}\n", e.what());
    }
}

void CustomPacket::Unknown02() {}

bool CustomPacket::Unknown03()
{
    return true;
}

void CustomPacket::OnSetUseRelay(BufferView& view)
{
    auto enabled = view.Read<uint8_t>() == 1;
    cso2::SetUseUdpRelay(enabled);
}
