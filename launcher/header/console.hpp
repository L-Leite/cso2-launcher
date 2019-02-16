#pragma once

#ifdef _WIN32
#include <Windows.h>
#include <d3d9.h>
#else
#error Implement me
#endif

struct ImGuiInputTextCallbackData;

class GameConsole
{
public:
    GameConsole( void );
    ~GameConsole() = default;
    void Init( LPDIRECT3DDEVICE9 pDevice, HWND hWnd ) const;

    void DrawConsole( void );
    void DrawCompleteList( void );
    void ToggleConsole( bool extend );

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

    void OnEndScene();

    bool OnWindowCallback( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

private:
    bool m_bShowConsole;
    bool m_bChanged;
    bool m_bDrawExtend;

    char m_szConsoleText[0x100];
    char m_szOutputText[0x4000];

    std::vector<std::string> m_History;
    unsigned int m_HistoryPos;
    unsigned int m_CompletePos;
    std::vector<std::string_view> m_CompleteCandidates;

    bool m_bNeedScroll;
};

extern GameConsole g_GameConsole;