#include <windows.h>
#include <stdio.h>

#include "../githash.h"

#define DLL_EXPORT extern "C" __declspec(dllexport)

typedef int( *LauncherMain_t )(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow);

enum Sys_Flags
{
	SYS_NOFLAGS = 0x00,
	SYS_NOLOAD = 0x01   // no loading, no ref-counting, only returns handle if lib is loaded. 
};

// aka InternalLoadLibrary
DLL_EXPORT HMODULE _Init( const char *pName, Sys_Flags flags )
{	
	HMODULE hModule = nullptr;

	if ( flags & SYS_NOLOAD )
		hModule = GetModuleHandle( pName );
	else
		hModule = LoadLibraryEx( pName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );

	return hModule;
}

bool CreateDebugConsole()
{
	BOOL result = AllocConsole();

	if (!result)
		return false;

	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	SetConsoleTitleW(L"cso2-launcher commit: " GIT_COMMIT_HASH " -- Debug Console");
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);

	return true;
}

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{			
	CreateDebugConsole();

	HINSTANCE launcher = LoadLibraryEx( "launcher.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
	if ( !launcher )
	{
		char *pszError;
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPTSTR) &pszError, 0, NULL );

		char szBuf[1024];
		_snprintf( szBuf, sizeof( szBuf ), "Failed to load the launcher DLL:\n\n%s", pszError );
		szBuf[sizeof( szBuf ) - 1] = '\0';
		MessageBox( 0, szBuf, "Launcher Error", MB_OK );

		LocalFree( pszError );
		return 0;
	}

	LauncherMain_t main = (LauncherMain_t) GetProcAddress( launcher, "LauncherMain" );
	return main( hInstance, hPrevInstance, lpCmdLine, nCmdShow );
}
