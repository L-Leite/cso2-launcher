#include "stdafx.hpp"
#include "console.hpp"
#include "assert.h"
#include "cdll_int.h"
#include "convar.h"
#include "git-version.hpp"
#include "hooks.hpp"
#include "imgui.h"
#include "imgui_impl/imgui_impl_dx9.h"
#include "imgui_impl/imgui_impl_win32.h"
#include "tierextra.hpp"

//
// Global stuffs
//

static LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;

GameConsole* g_pGameConsole = nullptr;

bool g_bRenderStarted = false;

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg,
                                               WPARAM wParam, LPARAM lParam );

//
// Utils
//

bool bCompare( const BYTE* pData, const BYTE* bMask, const char* szMask )
{
    for ( ; *szMask; ++szMask, ++pData, ++bMask )
        if ( *szMask == 'x' && *pData != *bMask )
            return false;

    return ( *szMask ) == NULL;
}
DWORD FindPattern( DWORD dwAddress, DWORD dwLen, BYTE* bMask, char* szMask )
{
    for ( DWORD i = 0; i < dwLen; i++ )
        if ( bCompare( (BYTE*)( dwAddress + i ), bMask, szMask ) )
            return ( DWORD )( dwAddress + i );

    return 0;
}

GameConsole::GameConsole( void )
{
    m_hWnd = 0;
    m_bShowConsole = false;
    HistoryPos = -1;
    CompletePos = -1;
}

HOOK_DETOUR_DECLARE( hkEndScene );

HRESULT WINAPI hkEndScene( LPDIRECT3DDEVICE9 pDevice )
{
    if ( g_pd3dDevice == NULL )
    {
        g_pd3dDevice = pDevice;
        // Setup Dear ImGui context IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable
        // Keyboard Controls

        ImGui::StyleColorsDark();

        ImGui_ImplWin32_Init( g_pGameConsole->m_hWnd );
        ImGui_ImplDX9_Init( g_pd3dDevice );

        ImFontConfig chn_config;
        chn_config.MergeMode = true;
        chn_config.PixelSnapH = true;
        ImFont* font = io.Fonts->AddFontFromFileTTF(
            "c:\\Windows\\Fonts\\consola.ttf", 14.0f );

        ImFont* fontchn = io.Fonts->AddFontFromFileTTF(
            "c:\\Windows\\Fonts\\msyh.ttc", 15.0f, &chn_config,
            io.Fonts->GetGlyphRangesChineseSimplifiedCommon() );
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    g_pGameConsole->DrawConsole();

    ImGui::Render();
    ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData() );

    return HOOK_DETOUR_GET_ORIG( hkEndScene )( pDevice );
}

void GameConsole::Init( HWND hWnd )
{
    assert( g_pGameConsole );

    m_hWnd = hWnd;

    uint32_t* vtable = 0;
    uint32_t table =
        FindPattern( (DWORD)GetModuleHandle( "d3d9.dll" ), 0x128000,
                     ( PBYTE ) "\xC7\x06\x00\x00\x00\x00\x89\x86\x00\x00"
                               "\x00\x00\x89\x86",
                     "xx????xx????xx" );
    memcpy( &vtable, (void*)( table + 2 ), 4 );

    HOOK_DETOUR( vtable[42], hkEndScene );
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
    ImVec2 window_pos_pivot = ImVec2( 0, 0 );
    ImGui::SetNextWindowPos( window_pos, ImGuiCond_Always, window_pos_pivot );

    ImGui::Begin(
        "ConsoleInput", NULL,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoCollapse );

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
        HistoryPos = -1;
        CompletePos = -1;
        CompleteCandidates.clear();
        for ( int i = History.size() - 1; i >= 0; i-- )
            if ( stricmp( History[i], m_szConsoleText ) == 0 )
            {
                free( History[i] );
                History.erase( History.begin() + i );
                break;
            }

        History.push_back( strdup( m_szConsoleText ) );

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

    window_pos = ImVec2( 0, 50.0f );
    window_pos_pivot = ImVec2( 0, 50.0f );
    ImGui::SetNextWindowPos( window_pos, ImGuiCond_Always, window_pos_pivot );

    ImGui::Begin(
        "ConsoleOutput", NULL,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoCollapse );

    ImGui::Dummy( ImVec2( 0, 25.0f ) );
    ImGui::InputTextMultiline(
        "console_out", m_szOutputText, 0x4000,
        ImVec2( ImGui::GetIO().DisplaySize.x - 20.0f,
                ImGui::GetIO().DisplaySize.y - 100.0f ),
        ImGuiInputTextFlags_ReadOnly | NeedScrollOutput() );

    ImGui::Text( u8"--- cso2-launcher " GIT_BRANCH " commit: " GIT_COMMIT_HASH " ---" );
    ImGui::End();
}

void GameConsole::ToogleConsole( void )
{
    CreateInterfaceFn pEngineFactory = Sys_GetFactory( "engine.dll" );
    ConnectExtraLibraries( &pEngineFactory, 1 );
    g_pEngineClient->ClientCmd_Unrestricted( "pause" );
    m_bShowConsole = !m_bShowConsole;
}

//
// This supports Call of Duty style colored text:
// ^0 - Black
// ^1 - Red
// ^2 - Green
// ^3 - Yellow
// ^4 - Blue
// ^5 - Light Blue(Cyan)
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
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while ( word_start > data->Buf )
            {
                const char c = word_start[-1];
                if ( c == ' ' || c == '\t' || c == ',' || c == ';' )
                    break;
                word_start--;
            }

            // Build a list of candidates
			if (CompletePos == -1)
			{
                const ConCommandBase* var;
                for ( var = g_pCVar->GetCommands(); var; var = var->GetNext() )
                {
                    if ( strnicmp( var->GetName(), word_start,
                                   (int)( word_end - word_start ) ) == 0 )
                        CompleteCandidates.push_back( var->GetName() );
                }
			}
            

            if ( CompleteCandidates.size() > 0 )
            {
				CompletePos++;

				if ( CompletePos >= CompleteCandidates.size() )
                    CompletePos = 0;

                data->DeleteChars( 0 , strlen(data->Buf) );
                data->InsertChars( data->CursorPos,
                                   CompleteCandidates[CompletePos] );
                data->InsertChars( data->CursorPos, " " );
            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory:
        {
            const int prev_history_pos = HistoryPos;
            if ( data->EventKey == ImGuiKey_UpArrow )
            {
                if ( HistoryPos == -1 )
                    HistoryPos = History.size() - 1;
                else if ( HistoryPos > 0 )
                    HistoryPos--;
            }
            else if ( data->EventKey == ImGuiKey_DownArrow )
            {
                if ( HistoryPos != -1 )
                    if ( ++HistoryPos >= History.size() )
                        HistoryPos = -1;
            }

            // A better implementation would preserve the data on the
            // current input line along with cursor position.
            if ( prev_history_pos != HistoryPos )
            {
                const char* history_str =
                    ( HistoryPos >= 0 ) ? History[HistoryPos] : "";
                data->DeleteChars( 0, data->BufTextLen );
                data->InsertChars( 0, history_str );
            }
            break;
        }
        case ImGuiInputTextFlags_CallbackCharFilter:
		{
            CompletePos = -1;
            CompleteCandidates.clear();

			if ( data->EventChar == '`' || data->EventChar == '~' )
                return 1;

            break;
		}
    }
    return 0;
}

LRESULT WINAPI GameConsole::WndProc( HWND hWnd, UINT msg, WPARAM wParam,
                                     LPARAM lParam, WndProc_t orig )
{
    if ( g_pd3dDevice && g_bRenderStarted )
    {
        if ( msg == WM_KEYDOWN )
            if ( wParam == VK_OEM_3 )
            {
                g_pGameConsole->ToogleConsole();

                return true;
            }

        if ( g_pGameConsole->m_bShowConsole )
        {
            ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam );
            return false;
        }
    }

    return orig( hWnd, msg, wParam, lParam );
}