#pragma once

#include "tier1/ilocalize.hpp"

namespace vgui
{
class ILocalize : public ::ILocalize
{
};  // backwards compatability with vgui::ILocalize declarations
}  // namespace vgui

constexpr const char* VGUI_LOCALIZE_INTERFACE_VERSION = "VGUI_Localize005";
