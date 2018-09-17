#include <Windows.h>	   
#include <string>
#include <iostream>
#include <sstream>

#include "../githash.h"

enum Sys_Flags
{
	SYS_NOFLAGS = 0x00,
	SYS_NOLOAD = 0x01   // no loading, no ref-counting, only returns handle if lib is loaded. 
};

// same as InternalLoadLibrary
extern "C" __declspec(dllexport) HMODULE _Init(const char *pName, Sys_Flags flags)
{
	HMODULE hModule = nullptr;

	if (flags & SYS_NOLOAD) {
		hModule = GetModuleHandle(pName);
	} else {
		hModule = LoadLibraryExA(pName, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
	}

	return hModule;
}

void CreateDebugConsole()
{
	AllocConsole();

	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);

	SetConsoleTitleA("cso2-launcher commit: " GIT_COMMIT_HASH " -- Debug Console");
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{
	CreateDebugConsole();

	HINSTANCE hLauncher = LoadLibraryExA("launcher.dll", nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);

	if (!hLauncher) {
		char* szError;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&szError), 0, nullptr);

		std::ostringstream outStream;
		outStream << "Failed to load the launcher DLL : \n\n" << szError << '\0';
		std::string szOutErrorMessage = outStream.str();

		std::cout << szOutErrorMessage;
		MessageBoxA(nullptr, szOutErrorMessage.c_str(), "Launcher Error", MB_OK);

		LocalFree(szError);
		return 0;
	}

	using LauncherMain_t = int(*)(HINSTANCE, HINSTANCE, LPSTR, int);
	auto main = reinterpret_cast<LauncherMain_t>(GetProcAddress(hLauncher, "LauncherMain"));
	return main(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
}
