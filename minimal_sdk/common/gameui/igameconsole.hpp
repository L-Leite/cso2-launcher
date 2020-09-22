#pragma once

#include <tier1/interface.hpp>

constexpr const char* GAMECONSOLE_INTERFACE_VERSION = "GameConsoleNexon001";

//-----------------------------------------------------------------------------
// Purpose: interface to game/dev console
//-----------------------------------------------------------------------------
class IGameConsole : public IBaseInterface
{
public:
    // activates the console, makes it visible and brings it to the foreground
    virtual void Activate() = 0;

    virtual void Initialize() = 0;

    // hides the console
    virtual void Hide() = 0;

    // clears the console
    virtual void Clear() = 0;

    // return true if the console has focus
    virtual bool IsConsoleVisible() = 0;

    virtual void SetParent( int parent ) = 0;
};
