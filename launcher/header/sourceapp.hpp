#pragma once

#include <string>

#include "appframework/AppFramework.h"
#include "tier0/icommandline.h"

class CAppSystemGroup;
class IFileSystem;

//-----------------------------------------------------------------------------
// Inner loop: initialize, shutdown main systems, load steam to
//-----------------------------------------------------------------------------
class CSourceAppSystemGroup : public CSteamAppSystemGroup
{
public:
    CSourceAppSystemGroup( std::string szBaseDir,
                           IFileSystem* pFs = nullptr,
                           CAppSystemGroup* pParent = nullptr );

    // Methods of IApplication
    bool Create() override;
    bool PreInit() override;
    int Main() override;
    void PostShutdown() override;
    void Destroy() override;

private:
    const char* DetermineDefaultMod() const;
    const char* DetermineDefaultGame() const;

    const std::string m_szBaseDir;
    bool m_bEditMode;
};

extern bool g_bTextMode;
