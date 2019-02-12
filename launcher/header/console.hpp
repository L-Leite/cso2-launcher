#pragma once
#include <Windows.h>
#include <d3d9.h>
#include <vector>
#include "imgui.h"

typedef LRESULT( __stdcall* WndProc_t )( HWND hWnd, UINT msg, WPARAM wParam,
                                         LPARAM lParam );

class GameConsole
{
public:
    GameConsole( void );
    void Init( HWND hWnd );
    void DrawConsole( void );
	void DrawCompleteList(void);
	void ToogleConsole(bool extend);

	void VWrite( const char* fmt, va_list va );
    void VWriteLine( const char* fmt, va_list va );

    void VWarning( const char* fmt, va_list va );
    void VDevInfo( const char* fmt, va_list va );
    void VError( const char* fmt, va_list va );

    void WriteLine( const char* fmt, ... );
    void Write( const char* fmt, ... );

    void Warning( const char* fmt, ... );
    void DevInfo( const char* fmt, ... );
    void Error( const char* fmt, ... );



    void ClearInput( void );
    void ClearOutput( void );

    int NeedScrollOutput( void );

    int ConsoleInputCallBack( ImGuiInputTextCallbackData* data );

	HWND m_hWnd;
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, WndProc_t orig);
private:
	bool m_bShowConsole;
	bool m_bChanged;
	bool m_bDrawExtend;

    char m_szConsoleText[0x100];
    char m_szOutputText[0x4000];

    std::vector<char*> History;
    int HistoryPos;
    int CompletePos;
    std::vector<const char*> CompleteCandidates;

    bool m_bNeedScroll;
};

extern GameConsole* g_pGameConsole;