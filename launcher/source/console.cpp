#include "stdafx.hpp"

#include <d3d9.h>

#include "imgui.h"
#include "imgui_impl/imgui_impl_dx9.h"
#include "imgui_impl/imgui_impl_win32.h"

#include "cdll_int.h"
#include "convar.h"

#include "console.hpp"
#include "git-version.hpp"
#include "hooks.hpp"
#include "tierextra.hpp"
#include "version.hpp"

//
// Global stuffs
//
GameConsole g_GameConsole;

bool g_bRenderStarted = true;

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg,
                                               WPARAM wParam, LPARAM lParam );

extern unsigned int consola_unicode_compressed_size;
extern unsigned int consola_unicode_compressed_data[8574320 / 4];

const unsigned int INVALID_POSITION = static_cast<unsigned int>( ~0 );

GameConsole::GameConsole( void )
{
    m_bShowConsole = false;
    m_bChanged = false;
    m_bDrawExtend = false;

    m_HistoryPos = INVALID_POSITION;
    m_CompletePos = INVALID_POSITION;

    m_bNeedScroll = false;

    ClearInput();
    ClearOutput();
}

// wrapper that adds the consolas fonts to imgui with specific (or not) font
// config and glyphs
static ImFont* ImGuiAddConsolaFont( const ImGuiIO& io, const float size_pixels,
                                    const ImFontConfig* font_cfg = nullptr,
                                    const ImWchar* glyph_ranges = nullptr )
{
    return io.Fonts->AddFontFromMemoryCompressedTTF(
        consola_unicode_compressed_data, consola_unicode_compressed_size,
        size_pixels, font_cfg, glyph_ranges );
}

void GameConsole::Init( LPDIRECT3DDEVICE9 pDevice, HWND hWnd ) const
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable
    // Keyboard Controls

    ImGui::StyleColorsDark();

    // Load unicode fonts

    ImFontConfig font_cfg = ImFontConfig();
    font_cfg.PixelSnapH = true;
    font_cfg.MergeMode = true;

    // Default
    ImGuiAddConsolaFont( io, 20.0f );

    // chinese(t+s)
    ImGuiAddConsolaFont( io, 19.0f, &font_cfg,
                         io.Fonts->GetGlyphRangesChineseFull() );

    // japanese
    ImGuiAddConsolaFont( io, 19.0f, &font_cfg,
                         io.Fonts->GetGlyphRangesJapanese() );

    // korean
    ImGuiAddConsolaFont( io, 19.0f, &font_cfg,
                         io.Fonts->GetGlyphRangesKorean() );

    ImGui_ImplWin32_Init( hWnd );
    ImGui_ImplDX9_Init( pDevice );
}

int InputCallback( ImGuiInputTextCallbackData* data )
{
    return ( (GameConsole*)data->UserData )->ConsoleInputCallBack( data );
}

void GameConsole::DrawConsole( void )
{
    if ( !m_bShowConsole )
        return;

    ImVec2 window_pos = ImVec2( 0, 0 );
    ImGui::SetNextWindowPos( window_pos, ImGuiCond_Always );

    ImGui::Begin(
        "ConsoleInput", NULL,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus );

    ImGui::SetKeyboardFocusHere();

    ImGui::PushItemWidth( ImGui::GetIO().DisplaySize.x - 20.0f );
    if ( ImGui::InputText( "", m_szConsoleText, 255,
                           ImGuiInputTextFlags_EnterReturnsTrue |
                               ImGuiInputTextFlags_CallbackCharFilter |
                               ImGuiInputTextFlags_CallbackHistory |
                               ImGuiInputTextFlags_CallbackCompletion,
                           InputCallback, (void*)this ) )
    {
        WriteLine( "] %s", m_szConsoleText );

        // Insert into history. First find match and delete it so it can be
        // pushed to the back. This isn't trying to be smart or optimal.
        m_HistoryPos = INVALID_POSITION;
        m_CompletePos = INVALID_POSITION;
        m_CompleteCandidates.clear();

        for ( int i = m_History.size() - 1; i >= 0; i-- )
        {
            if ( m_History[i] == m_szConsoleText )
            {
                m_History.erase( m_History.begin() + i );
                break;
            }
        }

        m_History.push_back( strdup( m_szConsoleText ) );

        std::string szCommand( m_szConsoleText );

        if ( szCommand.find( "clear" ) == 0 )
        {
            ClearOutput();
            Write( "^5Console cleared.\n" );
        }
        else
        {
            g_pEngineClient->ClientCmd_Unrestricted( szCommand.c_str() );
        }

        ClearInput();
    }

    ImGui::End();

    if ( m_bDrawExtend )
    {
        ImGui::SetNextWindowPos( window_pos, ImGuiCond_Always,
                                 ImVec2( 0, 50.0f ) );

        ImGui::Begin(
            "ConsoleOutput", NULL,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoBringToFrontOnFocus );

        ImGui::Dummy( ImVec2( 0, 25.0f ) );
        ImGui::InputTextMultiline(
            "console_out", m_szOutputText, 0x4000,
            ImVec2( ImGui::GetIO().DisplaySize.x - 20.0f,
                    ImGui::GetIO().DisplaySize.y - 100.0f ),
            ImGuiInputTextFlags_ReadOnly | NeedScrollOutput() );

        ImGui::Text( "^3--- cso2-launcher v" LAUNCHER_VERSION "-" GIT_BRANCH
                     "-" GIT_COMMIT_HASH " ---" );
        ImGui::End();
    }

    DrawCompleteList();
}

void GameConsole::DrawCompleteList( void )
{
    if ( *m_szConsoleText != ' ' && *m_szConsoleText != 0 )
    {
        ImVec2 window_pos = ImVec2( 20.0f, 40.0f );
        ImGui::SetNextWindowPos( window_pos );

        ImGui::Begin(
            "List", NULL,
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                ImGuiWindowFlags_AlwaysAutoResize |
                ImGuiWindowFlags_NoCollapse );

        ImGui::Dummy( ImVec2( ImGui::GetIO().DisplaySize.x / 2, 0 ) );

        // Build a list of candidates
        if ( m_bChanged )
        {
            m_bChanged = false;
            m_HistoryPos = INVALID_POSITION;
            m_CompleteCandidates.clear();

            // Only match word before first space
            int len = strlen( m_szConsoleText );
            for ( int i = 0; i < len; i++ )
                if ( m_szConsoleText[len] == ' ' )
                {
                    len = i;
                    break;
                }

            std::string word = std::string( m_szConsoleText ).substr( 0, len );

            const ConCommandBase* ccommandbase;
            int counter = 0;
            for ( ccommandbase = g_pCVar->GetCommands(); ccommandbase;
                  ccommandbase = ccommandbase->GetNext() )
                if ( strnicmp( ccommandbase->GetName(), word.c_str(),
                               word.length() ) == 0 )
                    m_CompleteCandidates.push_back( ccommandbase->GetName() );
        }

        static ConCommandBase* command = NULL;
        unsigned int counter = 0;
        for ( auto commandname : m_CompleteCandidates )
        {
            counter++;
            if ( m_CompletePos != INVALID_POSITION &&
                 m_CompletePos > counter - 1 )
                continue;

            if ( counter >=
                 ( m_CompletePos == INVALID_POSITION ? 0 : m_CompletePos ) + 7 )
                break;

            command = g_pCVar->FindCommandBase( commandname.data() );

            ImGui::Columns( 2, command->GetName(), false );
            ImGui::Text( "^3%s", command->GetName() );
            ImGui::NextColumn();

            if ( command->IsCommand() )
                ImGui::Text( "^6Command" );
            else
                ImGui::Text(
                    "^2%s",
                    g_pCVar->FindVar( commandname.data() )->GetString() );

            ImGui::NextColumn();
        }

        if ( counter >= 7 )
        {
            ImGui::Columns( 1, "toomuchtext", false );
            ImGui::Text( "^2%i ^6Matches, Too much to show here, Press ^7Shift "
                         "+ Tile ^6 to show all.",
                         m_CompleteCandidates.size() );
            ImGui::NextColumn();
        }

        // If there's only 1 match, show detailed info.
        if ( counter == 1 )  // IDK why,the command will be NULL sometimes, I
                             // dont have any idea about it
        {
            // If this is a cvar, display default value
            if ( !command->IsCommand() )
            {
                ImGui::Columns( 2, command->GetName(), false );
                ImGui::Text( "^6Default:" );
                ImGui::NextColumn();
                ImGui::Text(
                    "^2%s",
                    g_pCVar->FindVar( command->GetName() )->GetDefault() );
                ImGui::NextColumn();
            }

            // Help text
            ImGui::Columns( 1, "helptext", false );
            ImGui::Text( command->GetHelpText() );
            ImGui::NextColumn();

            // Display some flags
            ImGui::Columns( 2, "flags", false );

            ImGui::Text( "^6Flags:" );
            ImGui::NextColumn();

            std::string flaglist = std::string();
            if ( command->IsFlagSet( FCVAR_ARCHIVE ) )
                flaglist.append( " Archive" );
            if ( command->IsFlagSet( FCVAR_REPLICATED ) )
                flaglist.append( " Server" );
            if ( command->IsFlagSet( FCVAR_CLIENTDLL ) )
                flaglist.append( " Client" );
            if ( command->IsFlagSet( FCVAR_PROTECTED ) )
                flaglist.append( " Protected" );
            if ( command->IsFlagSet( FCVAR_CHEAT ) )
                flaglist.append( " Cheat" );

            ImGui::Text( flaglist.c_str() );
            ImGui::NextColumn();
        }

        ImGui::End();
    }
}

void GameConsole::ToggleConsole( bool extend )
{
    if ( !m_bDrawExtend && m_bShowConsole )
        ClearInput();

    if ( extend && m_bShowConsole && m_CompleteCandidates.size() > 6 )
    {
        WriteLine( "---- Matches ----" );
        for ( auto commandname : m_CompleteCandidates )
        {
            WriteLine( commandname.data() );
        }

        WriteLine( "\n\n\n" );
    }

    m_bDrawExtend = extend;
    m_bShowConsole =
        ( extend && m_bShowConsole ) ? m_bShowConsole : !m_bShowConsole;
}

//
// This supports Call of Duty style colored text:
// ^0 - Black
// ^1 - Red
// ^2 - Green
// ^3 - Yellow
// ^4 - Blue
// ^5 - Light Blue(Cyan)
// ^6 - Grey
// ^7 - White
//
// Example: if you inputed "^1red ^4blue", the "red" will be Red and the
// "blue" will be Blue
//
void GameConsole::Write( const char* fmt, ... )
{
    va_list arglist;
    va_start( arglist, fmt );
    VWrite( fmt, arglist );
    va_end( arglist );
}

void GameConsole::WriteLine( const char* fmt, ... )
{
    va_list arglist;
    va_start( arglist, fmt );
    VWriteLine( fmt, arglist );
    va_end( arglist );
}

void GameConsole::Warning( const char* fmt, ... )
{
    va_list arglist;
    va_start( arglist, fmt );
    VWarning( fmt, arglist );
    va_end( arglist );
}

void GameConsole::DevInfo( const char* fmt, ... )
{
    va_list arglist;
    va_start( arglist, fmt );
    VDevInfo( fmt, arglist );
    va_end( arglist );
}

void GameConsole::Error( const char* fmt, ... )
{
    va_list arglist;
    va_start( arglist, fmt );
    VError( fmt, arglist );
    va_end( arglist );
}

void GameConsole::VWrite( const char* fmt, va_list va )
{
    char string[0x1000];
    Q_vsnprintf( string, sizeof( string ), fmt, va );
    if ( strlen( m_szOutputText ) + strlen( string ) > 0x3fff )
        ClearOutput();

    strcat( m_szOutputText, string );

    m_bNeedScroll = true;
}

void GameConsole::VWriteLine( const char* fmt, va_list va )
{
    char string[0x1000];
    Q_vsnprintf( string, sizeof( string ), fmt, va );

    int len = strlen( string );

    if ( string[len - 1] != '\n' )
    {
        string[len] = '\n';
        string[len + 1] = 0;
        len++;
    }

    if ( strlen( m_szOutputText ) + len > 0x3fff )
        ClearOutput();

    strcat( m_szOutputText, string );

    m_bNeedScroll = true;
}

void GameConsole::VWarning( const char* fmt, va_list va )
{
    char string[0x1000];
    Q_vsnprintf( string, sizeof( string ), fmt, va );

    WriteLine( "^3%s", string );
}

void GameConsole::VDevInfo( const char* fmt, va_list va )
{
    char string[0x1000];
    Q_vsnprintf( string, sizeof( string ), fmt, va );

    WriteLine( "^2%s", string );
}

void GameConsole::VError( const char* fmt, va_list va )
{
    char string[0x1000];
    Q_vsnprintf( string, sizeof( string ), fmt, va );

    WriteLine( "^1%s", string );
}

void GameConsole::ClearInput( void )
{
    m_szConsoleText[0] = 0;
}

void GameConsole::ClearOutput( void )
{
    m_szOutputText[0] = 0;
}

int GameConsole::NeedScrollOutput( void )
{
    int ret = ( m_bNeedScroll ? ImGuiInputTextFlags_NeedScrollDown : 0 );
    m_bNeedScroll = false;
    return ret;
}

int GameConsole::ConsoleInputCallBack( ImGuiInputTextCallbackData* data )
{
    switch ( data->EventFlag )
    {
        case ImGuiInputTextFlags_CallbackCompletion:
        {
            // Locate beginning of current word
            if ( m_CompleteCandidates.size() > 0 )
            {
                m_CompletePos++;

                if ( m_CompletePos >= m_CompleteCandidates.size() )
                    m_CompletePos = 0;

                data->DeleteChars( 0, strlen( data->Buf ) );
                data->InsertChars( data->CursorPos,
                                   m_CompleteCandidates[m_CompletePos].data() );
                data->InsertChars( data->CursorPos, " " );
            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory:
        {
            const unsigned int prev_history_pos = m_HistoryPos;
            if ( data->EventKey == ImGuiKey_UpArrow )
            {
                if ( m_HistoryPos == INVALID_POSITION )
                    m_HistoryPos = m_History.size() - 1;
                else if ( m_HistoryPos > 0 )
                    m_HistoryPos--;
            }
            else if ( data->EventKey == ImGuiKey_DownArrow )
            {
                if ( m_HistoryPos != -1 )
                    if ( ++m_HistoryPos >= m_History.size() )
                        m_HistoryPos = INVALID_POSITION;
            }

            // A better implementation would preserve the data on the
            // current input line along with cursor position.
            if ( prev_history_pos != m_HistoryPos )
            {
                std::string_view history_str =
                    ( m_HistoryPos >= 0 ) ? m_History[m_HistoryPos] : "";
                data->DeleteChars( 0, data->BufTextLen );
                data->InsertChars( 0, history_str.data() );
            }
            break;
        }
        case ImGuiInputTextFlags_CallbackCharFilter:
        {
            // filter tile key
            if ( data->EventChar == '`' || data->EventChar == '~' )
                data->EventChar = 0;

            break;
        }
    }
    return 0;
}

// this is called by the game's d3d device's endscene
// draws our own console
void GameConsole::OnEndScene()
{
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    DrawConsole();

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData() );
}

static bool shiftDown = false;

bool GameConsole::OnWindowCallback( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    if ( g_bRenderStarted )
    {
        if ( msg == WM_KEYDOWN )
        {
            if ( wParam != VK_TAB )
                m_bChanged = true;

            if ( wParam == VK_OEM_3 )
                ToggleConsole( shiftDown );

            if ( wParam == VK_SHIFT )
                shiftDown = true;
        }

        if ( msg == WM_KEYUP && wParam == VK_SHIFT )
            shiftDown = false;

        if ( m_bShowConsole )
        {
            ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam );
            return false;
        }
    }

    return true;
}