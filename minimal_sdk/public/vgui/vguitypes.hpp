#pragma once

namespace vgui
{
using VPANEL = unsigned int;

using HContext = int;
using HCursor = unsigned long;
using HFont = unsigned long;
using HPanel = unsigned long;
using HScheme = unsigned long;
using HTexture = unsigned long;

constexpr const HContext DEFAULT_VGUI_CONTEXT = -1;
constexpr const HFont INVALID_FONT = 0;
constexpr const HPanel INVALID_PANEL = 0xffffffff;
}  // namespace vgui
