#include "strtools.h"
#include "hooks.h"

#include <atlbase.h>
#include "tier0/icommandline.h"

#include "tier0/memdbgon.h"

extern bool g_bEnableLocalization;

HOOK_DETOUR_DECLARE(hkStrTblFind);

// CLocalizedStringTable::Find
NOINLINE wchar_t* __fastcall hkStrTblFind(void* ecx, void* edx, const char* pName)
{
	static wchar_t szBuffer[1024];

	if (!g_bEnableLocalization)
	{
		size_t nameLength = strlen(pName);
		MultiByteToWideChar(CP_ACP, NULL, pName, strlen(pName), szBuffer, nameLength + 1);
		szBuffer[nameLength] = '\0';
		return szBuffer;
	}			 
 
	return HOOK_DETOUR_GET_ORIG(hkStrTblFind)(ecx, edx, pName);
}

HOOK_DETOUR_DECLARE(hkStrTblAddFile);

//CLocalizedStringTable::AddFile
NOINLINE bool __fastcall hkStrTblAddFile(void* ecx, void* edx, const char *szFileName, const char *pPathID, bool bIncludeFallbackSearchPaths)
{
	if (strcmp(szFileName, "resource/cso2_koreana.txt") == 0) 
	{
		char szFilePath[MAX_PATH];

		const char *szLang = CommandLine()->ParmValue("-lang");

		HKEY hKey;

		if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Nexon\\cso2\\cso2_launcher", NULL, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS)
		{
			if (RegCreateKey(HKEY_CURRENT_USER, "Software\\Nexon\\cso2\\cso2_launcher", &hKey) != ERROR_SUCCESS)
			{
				snprintf(szFilePath, sizeof(szFilePath), "resource/cso2_%s.txt", szLang ? szLang : "koreana");
				goto final;
			}

		}

		if (szLang) 
		{
			snprintf(szFilePath, sizeof(szFilePath), "resource/cso2_%s.txt", szLang);
			RegSetValueEx(hKey, "language", 0, REG_SZ, (unsigned char *)szLang, strlen(szLang));
		}
		else
		{
			char dwValue[256];
			DWORD dwSize = sizeof(dwValue);

			if (RegQueryValueEx(hKey, "language", NULL, NULL, (LPBYTE)&dwValue, &dwSize) == ERROR_SUCCESS)
			{
				snprintf(szFilePath, sizeof(szFilePath), "resource/cso2_%s.txt", dwValue);
			}
			else
			{
				snprintf(szFilePath, sizeof(szFilePath), "resource/cso2_%s.txt", "koreana");
				RegSetValueEx(hKey, "language", 0, REG_SZ, (unsigned char *)"koreana", 7);
			}
		}

		RegCloseKey(hKey);

	final:
		printf("Adding fallback(resource/cso2_koreana_fallback.txt) localized strings... \n");
		HOOK_DETOUR_GET_ORIG(hkStrTblAddFile)(ecx, edx, "resource/cso2_koreana_fallback.txt", pPathID, true);

		printf("Loading custom(%s) localized strings... \n", szFilePath);
		return HOOK_DETOUR_GET_ORIG(hkStrTblAddFile)(ecx, edx, szFilePath, pPathID, true);
	}

	return HOOK_DETOUR_GET_ORIG(hkStrTblAddFile)(ecx, edx, szFileName, pPathID, bIncludeFallbackSearchPaths);
}

ON_LOAD_LIB(vgui2)
{ 
	uintptr_t dwVguiBase = GET_LOAD_LIB_MODULE();
	HOOK_DETOUR(dwVguiBase + 0xAC80, hkStrTblFind);
	HOOK_DETOUR(dwVguiBase + 0x8D90, hkStrTblAddFile);
}

