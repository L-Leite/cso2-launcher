//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose:
//
// Defines the entry point for the application.
//
//===========================================================================//

#ifdef _WIN32
#include <windows.h>

#include <shellapi.h>
#include <shlwapi.h>
#include <winsock2.h>
#endif

#include "basedir.hpp"
#include "leakdump.hpp"
#include "sourceapp.hpp"

#include "appframework/IAppSystem.h"
#include "appframework/IAppSystemGroup.h"
#include "engine_launcher_api.h"
#include "iregistry.h"
#include "materialsystem/imaterialsystem.h"

#include "tier0/dbg.h"
#include "tier0/platform.h"
#include "tier0/vcrmode.h"

#include "tier1/interface.h"

#include <tier0/cso2/iloadingsplash.h>
#include <tier0/cso2/log.h>
#include <tier0/cso2/messagebox.h>

// copied from sys.h
struct FileAssociationInfo
{
    char const* extension;
    char const* command_to_issue;
};

static FileAssociationInfo g_FileAssociations[] = {
    { ".dem", "playdemo" },
    { ".sav", "load" },
    { ".bsp", "map" },
};

CLeakDump g_LeakDump;

//-----------------------------------------------------------------------------
// Spew function!
//-----------------------------------------------------------------------------
SpewRetval_t LauncherDefaultSpewFunc( SpewType_t spewType, char const* pMsg )
{
#ifndef _CERT
    OutputDebugStringA( pMsg );
    switch ( spewType )
    {
        case SPEW_MESSAGE:
        case SPEW_LOG:
            return SPEW_CONTINUE;

        case SPEW_WARNING:
            if ( !stricmp( GetSpewOutputGroup(), "init" ) )
            {
                CSO2MessageBox( nullptr, pMsg, "Warning!",
                                MB_OK | MB_SYSTEMMODAL | MB_ICONERROR );
            }
            return SPEW_CONTINUE;

        case SPEW_ASSERT:
            if ( !ShouldUseNewAssertDialog() )
            {
                CSO2MessageBox( nullptr, pMsg, "Assert!",
                                MB_OK | MB_SYSTEMMODAL | MB_ICONERROR );
            }
            return SPEW_DEBUGGER;

        case SPEW_ERROR:
        default:
            CSO2MessageBox( nullptr, pMsg, "Error!",
                            MB_OK | MB_SYSTEMMODAL | MB_ICONERROR );
            _exit( 1 );
    }
#else
    if ( spewType != SPEW_ERROR )
        return SPEW_CONTINUE;
    _exit( 1 );
#endif
}

//-----------------------------------------------------------------------------
// Implementation of VCRHelpers.
//-----------------------------------------------------------------------------
class CVCRHelpers : public IVCRHelpers
{
public:
    void ErrorMessage( const char* pMsg ) override
    {
        NOVCR( ::MessageBox( nullptr, pMsg, "VCR Error", MB_OK ) );
    }

    void* GetMainWindow() override { return nullptr; }
};

static CVCRHelpers g_VCRHelpers;

BOOL WINAPI MyHandlerRoutine( DWORD dwCtrlType )
{
#if !defined( _X360 )
    TerminateProcess( GetCurrentProcess(), 2 );
#endif
    return TRUE;
}

void InitTextMode()
{
    AllocConsole();

    SetConsoleCtrlHandler( MyHandlerRoutine, TRUE );

    freopen( "CONIN$", "rb",
             stdin );  // reopen stdin handle as console window input
    freopen( "CONOUT$", "wb",
             stdout );  // reopen stout handle as console window output
    freopen( "CONOUT$", "wb",
             stderr );  // reopen stderr handle as console window output
}

//-----------------------------------------------------------------------------
// Allow only one windowed source app to run at a time
//-----------------------------------------------------------------------------
HANDLE g_hMutex = nullptr;
bool GrabSourceMutex()
{
    // don't allow more than one instance to run
    g_hMutex = CreateMutexA( nullptr, FALSE, "hl2_singleton_mutex" );

    DWORD waitResult = WaitForSingleObject( g_hMutex, 0 );

    // Here, we have the mutex
    if ( waitResult == WAIT_OBJECT_0 || waitResult == WAIT_ABANDONED )
    {
        return true;
    }

    // couldn't get the mutex, we must be running another instance
    CloseHandle( g_hMutex );

    return false;
}

void ReleaseSourceMutex()
{
    if ( g_hMutex )
    {
        ReleaseMutex( g_hMutex );
        CloseHandle( g_hMutex );
        g_hMutex = nullptr;
    }
}

// Remove all but the last -game parameter.
// This is for mods based off something other than Half-Life 2 (like HL2MP
// mods). The Steam UI does 'steam -applaunch 320 -game
// c:\steam\steamapps\sourcemods\modname', but applaunch inserts its own -game
// parameter, which would supercede the one we really want if we didn't
// intercede here.
void RemoveSpuriousGameParameters()
{
    // Find the last -game parameter.
    int nGameArgs = 0;
    char lastGameArg[MAX_PATH];
    for ( int i = 0; i < CommandLine()->ParmCount() - 1; i++ )
    {
        if ( Q_stricmp( CommandLine()->GetParm( i ), "-game" ) == 0 )
        {
            Q_snprintf( lastGameArg, sizeof( lastGameArg ), "\"%s\"",
                        CommandLine()->GetParm( i + 1 ) );
            ++nGameArgs;
            ++i;
        }
    }

    // We only care if > 1 was specified.
    if ( nGameArgs > 1 )
    {
        CommandLine()->RemoveParm( "-game" );
        CommandLine()->AppendParm( "-game", lastGameArg );
    }
}

/*
============
va

does a varargs printf into a temp buffer, so I don't need to have
varargs versions of all text functions.
============
*/
static char* va( char* format, ... )
{
    va_list argptr;
    static char string[8][512];
    static int curstring = 0;

    curstring = ( curstring + 1 ) % 8;

    va_start( argptr, format );
    Q_vsnprintf( string[curstring], sizeof( string[curstring] ), format,
                 argptr );
    va_end( argptr );

    return string[curstring];
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *param -
// Output : static char const
//-----------------------------------------------------------------------------
static char const* Cmd_TranslateFileAssociation( char const* param )
{
    static char sz[512];
    char* retval = nullptr;

    char temp[512];
    Q_strncpy( temp, param, sizeof( temp ) );
    Q_FixSlashes( temp );
    Q_strlower( temp );

    const char* extension = V_GetFileExtension( temp );
    // must have an extension to map
    if ( !extension )
    {
        return retval;
    }
    extension--;  // back up so we have the . in the extension

    int c = ARRAYSIZE( g_FileAssociations );
    for ( int i = 0; i < c; i++ )
    {
        FileAssociationInfo& info = g_FileAssociations[i];

        if ( !Q_strcmp( extension, info.extension ) &&
             !CommandLine()->FindParm( va( "+%s", info.command_to_issue ) ) )
        {
            // Translate if haven't already got one of these commands
            Q_strncpy( sz, temp, sizeof( sz ) );
            Q_FileBase( sz, temp, sizeof( sz ) );

            Q_snprintf( sz, sizeof( sz ), "%s %s", info.command_to_issue,
                        temp );
            retval = sz;
            break;
        }
    }

    // return null if no translation, otherwise return commands
    return retval;
}

//-----------------------------------------------------------------------------
// Purpose: Converts all the convar args into a convar command
// Input  : none
// Output : const char * series of convars
//-----------------------------------------------------------------------------
static std::string BuildCommand()
{
    std::string cmdStr;

    // arg[0] is the executable name
    for ( int i = 1; i < CommandLine()->ParmCount(); i++ )
    {
        const char* szParm = CommandLine()->GetParm( i );
        if ( !szParm )
        {
            continue;
        }

        if ( szParm[0] == '-' )
        {
            // skip -XXX options and eat their args
            const char* szValue = CommandLine()->ParmValue( szParm );
            if ( szValue )
            {
                i++;
            }
            continue;
        }
        if ( szParm[0] == '+' )
        {
            // convert +XXX options and stuff them into the build buffer
            const char* szValue = CommandLine()->ParmValue( szParm );
            if ( szValue )
            {
                cmdStr += va( "%s %s;", szParm + 1, szValue );
                i++;
            }
            else
            {
                cmdStr += ( szParm + 1 );
                cmdStr += ';';
            }
        }
        else
        {
            // singleton values, convert to command
            char const* translated =
                Cmd_TranslateFileAssociation( CommandLine()->GetParm( i ) );
            if ( translated )
            {
                cmdStr += translated;
                cmdStr += ';';
            }
        }
    }

    return cmdStr;
}

bool CreateDebugConsole()
{
    BOOL bCreated = AllocConsole();

    if ( bCreated == FALSE )
    {
        return false;
    }

    FILE* conIn;
    FILE* conOut;
    FILE* conErr;

    freopen_s( &conIn, "CONIN$", "r", stdin );
    freopen_s( &conOut, "CONOUT$", "w", stdout );
    freopen_s( &conErr, "CONOUT$", "w", stderr );

    SetConsoleTitleA( "cso2-launcher -- Debug Console" );
    SetConsoleCP( CP_UTF8 );
    SetConsoleOutputCP( CP_UTF8 );

    return true;
}

extern void HookTier0();
extern void HookWinapi();

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine, int nShowCmd )
{
    SetAppInstance( hInstance );

    HookTier0();
    HookWinapi();

    g_CSO2DocLog.Create();

    // Hook the debug output stuff.
    SpewOutputFunc( LauncherDefaultSpewFunc );

    // Quickly check the hardware key, essentially a warning shot.
    if ( !Plat_VerifyHardwareKeyPrompt() )
    {
        return -1;
    }

    const char* filename;
    CommandLine()->CreateCmdLine( IsPC() ? VCRHook_GetCommandLine() :
                                           lpCmdLine );

    if ( CommandLine()->FindParm( "-debugconsole" ) != 0 )
    {
        bool bConCreated = CreateDebugConsole();

        if ( bConCreated == false )
        {
            Warning( "Could not create a debug console" );
        }
    }

    // show the splash screen
    GetCSO2LoadingSplash()->StartLoadingScreenThread( hInstance );

    // Figure out the directory the executable is running from
    // and make that be the current working directory
    CBaseDirectory baseDir;

    // Start VCR mode?
    if ( CommandLine()->CheckParm( "-vcrrecord", &filename ) )
    {
        if ( !VCRStart( filename, true, &g_VCRHelpers ) )
        {
            Error( "-vcrrecord: can't open '%s' for writing.\n", filename );
            return -1;
        }
    }
    else if ( CommandLine()->CheckParm( "-vcrplayback", &filename ) )
    {
        if ( !VCRStart( filename, false, &g_VCRHelpers ) )
        {
            Error( "-vcrplayback: can't open '%s' for reading.\n", filename );
            return -1;
        }
    }

    // See the function for why we do this.
    RemoveSpuriousGameParameters();

    if ( IsPC() )
    {
        // initialize winsock
        WSAData wsaData;
        int nError = ::WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
        if ( nError )
        {
            Msg( "Warning! Failed to start Winsock via WSAStartup = 0x%x.\n",
                 nError );
        }
    }

    // Run in text mode? (No graphics or sound).
    if ( CommandLine()->CheckParm( "-textmode" ) )
    {
        g_bTextMode = true;
        InitTextMode();
    }
    else
    {
#ifdef NO_MULTIPLE_CLIENTS
        int retval = -1;
        // Can only run one windowed source app at a time
        if ( !GrabSourceMutex() )
        {
            // We're going to hijack the existing session and load a new
            // savegame into it. This will mainly occur when users click on
            // links in Bugzilla that will automatically copy saves and load
            // them directly from the web browser. The -hijack command prevents
            // the launcher from objecting that there is already an instance of
            // the game.
            if ( CommandLine()->CheckParm( "-hijack" ) )
            {
                HWND hwndEngine = FindWindow( "Valve001", NULL );

                // Can't find the engine
                if ( hwndEngine == NULL )
                {
                    ::MessageBox( NULL,
                                  "The modified entity keyvalues could not be "
                                  "sent to the Source Engine because the "
                                  "engine does not appear to be running.",
                                  "Source Engine Not Running",
                                  MB_OK | MB_ICONEXCLAMATION );
                }
                else
                {
                    std::string szCommand = BuildCommand();

                    //
                    // Fill out the data structure to send to the engine.
                    //
                    COPYDATASTRUCT copyData;
                    copyData.cbData = szCommand.length() + 1;
                    copyData.dwData = 0;
                    copyData.lpData = (void*)szCommand.c_str();

                    if ( !::SendMessage( hwndEngine, WM_COPYDATA, 0,
                                         (LPARAM)&copyData ) )
                    {
                        ::MessageBox(
                            NULL,
                            "The Source Engine was found running, but did not "
                            "accept the request to load a savegame. It may be "
                            "an old version of the engine that does not "
                            "support this functionality.",
                            "Source Engine Declined Request",
                            MB_OK | MB_ICONEXCLAMATION );
                    }
                    else
                    {
                        retval = 0;
                    }
                }
            }
            else
            {
                ::MessageBox(
                    NULL,
                    "Only one instance of the game can be running at one time.",
                    "Source - Warning", MB_ICONINFORMATION | MB_OK );
            }

            return retval;
        }
#endif
    }

    // Make low priority?
    if ( CommandLine()->CheckParm( "-low" ) )
    {
        SetPriorityClass( GetCurrentProcess(), IDLE_PRIORITY_CLASS );
    }
    else if ( CommandLine()->CheckParm( "-high" ) )
    {
        SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );
    }

    // If game is not run from Steam then add -insecure in order to avoid client
    // timeout message
    if ( nullptr == CommandLine()->CheckParm( "-steam" ) )
    {
        CommandLine()->AppendParm( "-insecure", nullptr );
    }

    g_LeakDump.m_bCheckLeaks =
        CommandLine()->CheckParm( "-leakcheck" ) != nullptr;

    bool bRestart = true;
    while ( bRestart )
    {
        bRestart = false;

        CSourceAppSystemGroup sourceSystems( baseDir.GetBaseDir() );
        CSteamApplication steamApplication( &sourceSystems );
        int nRetval = steamApplication.Run();

        if ( steamApplication.GetErrorStage() ==
             CSourceAppSystemGroup::INITIALIZATION )
        {
            bRestart = ( nRetval == INIT_RESTART );
        }
        else if ( nRetval == RUN_RESTART )
        {
            bRestart = true;
        }
    }

    if ( IsPC() )
    {
        // shutdown winsock
        int nError = ::WSACleanup();
        if ( nError )
        {
            Msg( "Warning! Failed to complete WSACleanup = 0x%x.\n", nError );
        }
    }

    // Allow other source apps to run
    ReleaseSourceMutex();

#ifndef _X360
    // Now that the mutex has been released, check
    // HKEY_CURRENT_USER\Software\Valve\Source\Relaunch URL. If there is a URL
    // here, exec it. This supports the capability of immediately re-launching
    // the the game via Steam in a different audio language
    HKEY hKey;
    if ( RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\Valve\\Source", NULL,
                       KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS )
    {
        char szValue[MAX_PATH];
        DWORD dwValueLen = MAX_PATH;

        if ( RegQueryValueEx( hKey, "Relaunch URL", nullptr, nullptr,
                              (unsigned char*)szValue,
                              &dwValueLen ) == ERROR_SUCCESS )
        {
            ShellExecuteA( nullptr, "open", szValue, nullptr, nullptr,
                           SW_SHOW );
            RegDeleteValue( hKey, "Relaunch URL" );
        }

        RegCloseKey( hKey );
    }

#endif

    return 0;
}
