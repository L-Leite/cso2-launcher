#include "launcher.hpp"

#include <filesystem>
#include <string>
namespace fs = std::filesystem;

#include "platform.hpp"

#include <tier0/cso2/iloadingsplash.hpp>
#include <tier0/dbg.hpp>
#include <tier0/icommandline.hpp>

extern void HookWinapi();

Launcher::Launcher( void* instance, const char* cmdline )
    : m_pInstance( instance ), m_bShowSplash( true )
{
    HookWinapi();

    CommandLine()->CreateCmdLine( cmdline );
    this->ParseCommandLine();

    SpewOutputFunc( LauncherDefaultSpewFunc );
}

int Launcher::Main()
{
    NET_Init();
    Sys_InitPlatformSpecific();

    if ( this->m_bShowSplash == true )
    {
        GetCSO2LoadingSplash()->StartSplashThread( this->m_pInstance );
    }

    extern void SourceMain( void* pInstance );
    SourceMain( this->m_pInstance );

    return 0;
}

void Launcher::Destroy()
{
    Sys_DestroyPlatformSpecific();
    NET_Destroy();
}

void Launcher::ParseCommandLine()
{
    this->m_bShowSplash = CommandLine()->CheckParm( "-nosplash" ) == nullptr;

    bool bShowDebugCon = CommandLine()->CheckParm( "-debugconsole" ) != nullptr;
    bool bTextMode = CommandLine()->CheckParm( "-textmode" ) != nullptr;

    if ( bShowDebugCon == true || bTextMode == true )
    {
        InitTextMode();
    }

    if ( CommandLine()->CheckParm( "-low" ) )
    {
        Proc_SetLowPriority();
    }
    else if ( CommandLine()->CheckParm( "-high" ) )
    {
        Proc_SetHighPriority();
    }
}
