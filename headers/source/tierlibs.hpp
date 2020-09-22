#pragma once

//
// this header holds a minimal list of tier libraries used by the launcher
// NOTE: not all tier libraries are being connected, add them as you use them
//

#include <tier1/interface.hpp>

//
// tier1 libraries
//
extern class ICvar* g_pCVar;

//
// tier2 libraries
//
extern class IFileSystem* g_pFullFileSystem;
extern class IMaterialSystem* g_pMaterialSystem;
extern class IInputSystem* g_pInputSystem;

// tier3 libraries
namespace vgui
{
class ILocalize;
}

extern class vgui::ILocalize* g_pVGuiLocalize;

//
// aditional libraries
//
extern class IVEngineClient* g_pEngineClient;
extern class ICSO2MsgHandlerEngine* g_pCSO2MsgHandler;
extern class IEngineVGui* g_pEngineVGui;

void ConnectAllLibraries( CreateInterfaceFn factory );