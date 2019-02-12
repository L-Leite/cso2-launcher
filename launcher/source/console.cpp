#include "console.hpp"
#include "assert.h"
#include "cdll_int.h"
#include "convar.h"
#include "git-version.hpp"
#include "hooks.hpp"
#include "imgui.h"
#include "imgui_impl/imgui_impl_dx9.h"
#include "imgui_impl/imgui_impl_win32.h"
#include "stdafx.hpp"
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
	m_bDrawExtend = false;
    HistoryPos = -1;
    CompletePos = -1;

	ClearInput();
	ClearOutput();
}

HOOK_DETOUR_DECLARE( hkEndScene );

extern unsigned int consola_unicode_compressed_size;
extern unsigned int consola_unicode_compressed_data[8574320 / 4];
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

		// Load unicode fonts

		// Default
		ImFont* font = io.Fonts->AddFontFromMemoryCompressedTTF(consola_unicode_compressed_data, consola_unicode_compressed_size, 20.0f);

		ImFontConfig font_cfg = ImFontConfig();

		font_cfg.PixelSnapH = true;
		font_cfg.MergeMode = true;

		// chinese(t+s)
		ImFont* cfont = io.Fonts->AddFontFromMemoryCompressedTTF(
			consola_unicode_compressed_data, consola_unicode_compressed_size, 19.0f, &font_cfg, io.Fonts->GetGlyphRangesChineseFull());

		// japanese
		ImFont* jfont = io.Fonts->AddFontFromMemoryCompressedTTF(
			consola_unicode_compressed_data, consola_unicode_compressed_size, 19.0f, &font_cfg, io.Fonts->GetGlyphRangesJapanese());

		// korean
		ImFont* kfont = io.Fonts->AddFontFromMemoryCompressedTTF(
			consola_unicode_compressed_data, consola_unicode_compressed_size, 19.0f, &font_cfg, io.Fonts->GetGlyphRangesKorean());

        ImGui_ImplWin32_Init( g_pGameConsole->m_hWnd );
        ImGui_ImplDX9_Init( g_pd3dDevice );
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

	// Dirty way to hook EndScene, Try to use with VFuncSwapHook in PolyHook
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
    ImGui::SetNextWindowPos( window_pos, ImGuiCond_Always );

    ImGui::Begin(
        "ConsoleInput", NULL,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

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

	if (m_bDrawExtend)
	{
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0, 50.0f));

		ImGui::Begin(
			"ConsoleOutput", NULL,
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBackground |
			ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);

		ImGui::Dummy(ImVec2(0, 25.0f));
		ImGui::InputTextMultiline(
			"console_out", m_szOutputText, 0x4000,
			ImVec2(ImGui::GetIO().DisplaySize.x - 20.0f,
				ImGui::GetIO().DisplaySize.y - 100.0f),
			ImGuiInputTextFlags_ReadOnly | NeedScrollOutput());

		ImGui::Text(u8"^3--- cso2-launcher " GIT_BRANCH " commit: " GIT_COMMIT_HASH
			" ---");
		ImGui::End();
	}

	g_pGameConsole->DrawCompleteList();
}

void GameConsole::DrawCompleteList(void)
{
	if (*m_szConsoleText != ' ' && *m_szConsoleText != 0)
	{
		ImVec2 window_pos = ImVec2(20.0f, 40.0f);
		ImGui::SetNextWindowPos(window_pos);

		ImGui::Begin(
			"List", NULL,
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoCollapse);

		ImGui::Dummy(ImVec2(ImGui::GetIO().DisplaySize.x / 2, 0));

		// Build a list of candidates
		if (m_bChanged)
		{
			m_bChanged = false;
			HistoryPos = -1;
			CompleteCandidates.clear();

			// Only match word before first space
			int len = strlen(m_szConsoleText);
			for (int i = 0; i < len; i++)
				if (m_szConsoleText[len] == ' ')
				{
					len = i;
					break;
				}

			std::string word = std::string(m_szConsoleText).substr(0, len);

			const ConCommandBase* ccommandbase;
			ConVar* cvar;
			int counter = 0;
			for (ccommandbase = g_pCVar->GetCommands(); ccommandbase; ccommandbase = ccommandbase->GetNext())
				if (strnicmp(ccommandbase->GetName(), word.c_str(), word.length()) == 0)
					CompleteCandidates.push_back(ccommandbase->GetName());
		}

		static ConCommandBase* command = NULL;
		int counter = 0;
		for (auto commandname : CompleteCandidates)
		{
			counter++;
			if (CompletePos != -1 && CompletePos > counter - 1)
				continue;

			if (counter >= (CompletePos == -1 ? 0 : CompletePos) + 7)
				break;

			command = g_pCVar->FindCommandBase(commandname);

			ImGui::Columns(2, command->GetName(), false);
			ImGui::Text("^3%s", command->GetName());
			ImGui::NextColumn();

			if (command->IsCommand())
				ImGui::Text("^6Command");
			else
				ImGui::Text("^2%s", g_pCVar->FindVar(commandname)->GetString());
				
			ImGui::NextColumn();
		}

		if (counter >= 7)
		{
			ImGui::Columns(1, "toomuchtext", false);
			ImGui::Text("^2%i ^6Matches, Too much to show here, Press ^7Shift + Tile ^6 to show all.", CompleteCandidates.size());
			ImGui::NextColumn();
		}

		// If there's only 1 match, show detailed info.
		if (counter == 1) //IDK why,the command will be NULL sometimes, I dont have any idea about it
		{
			// If this is a cvar, display default value
			if (!command->IsCommand())
			{
				ImGui::Columns(2, command->GetName(), false);
				ImGui::Text("^6Default:");
				ImGui::NextColumn();
				ImGui::Text("^2%s", g_pCVar->FindVar(command->GetName())->GetDefault());
				ImGui::NextColumn();
			}

			// Help text
			ImGui::Columns(1, "helptext", false);
			ImGui::Text(command->GetHelpText());
			ImGui::NextColumn();

			// Display some flags
			ImGui::Columns(2, "flags", false);
			
			ImGui::Text("^6Flags:");
			ImGui::NextColumn();

			std::string flaglist = std::string();
			if (command->IsFlagSet(FCVAR_ARCHIVE))
				flaglist.append(" Archive");
			if (command->IsFlagSet(FCVAR_REPLICATED))
				flaglist.append(" Server");
			if (command->IsFlagSet(FCVAR_CLIENTDLL))
				flaglist.append(" Client");
			if (command->IsFlagSet(FCVAR_PROTECTED))
				flaglist.append(" Protected");
			if (command->IsFlagSet(FCVAR_CHEAT))
				flaglist.append(" Cheat");
			
			ImGui::Text(flaglist.c_str());
			ImGui::NextColumn();
		}

		ImGui::End();
	}
}

void GameConsole::ToogleConsole( bool extend )
{
    if ( g_pEngineClient == nullptr )
    {
        CreateInterfaceFn pEngineFactory = Sys_GetFactory( "engine.dll" );
        ConnectExtraLibraries( &pEngineFactory, 1 );
    }

	if(!m_bDrawExtend && m_bShowConsole)
		ClearInput();

	if (extend && m_bShowConsole && CompleteCandidates.size() > 6)
	{
		WriteLine("---- Matches ----");
		for (auto commandname : CompleteCandidates)
		{
			WriteLine(commandname);
		}

		WriteLine("\n\n\n");
	}

	m_bDrawExtend = extend;
	m_bShowConsole = (extend && m_bShowConsole) ? m_bShowConsole : !m_bShowConsole;
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
            if ( CompleteCandidates.size() > 0 )
            {
                CompletePos++;

                if ( CompletePos >= CompleteCandidates.size() )
                    CompletePos = 0;

                data->DeleteChars( 0, strlen( data->Buf ) );
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
			// filter tile key
			if (data->EventChar == '`' || data->EventChar == '~')
				data->EventChar = 0;

            break;
        }
    }
    return 0;
}

static bool shiftDown = false;

LRESULT WINAPI GameConsole::WndProc( HWND hWnd, UINT msg, WPARAM wParam,
                                     LPARAM lParam, WndProc_t orig )
{
    if ( g_pd3dDevice && g_bRenderStarted )
    {
		if (msg == WM_KEYDOWN)
		{
			if(wParam != VK_TAB)
				m_bChanged = true;

			if (wParam == VK_OEM_3)
				ToogleConsole(shiftDown);

			if (wParam == VK_SHIFT)
				shiftDown = true;
		}

		if (msg == WM_KEYUP && wParam == VK_SHIFT)
			shiftDown = false;

        if ( m_bShowConsole )
        {
            ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam );
            return false;
        }
    }

    return orig( hWnd, msg, wParam, lParam );
}