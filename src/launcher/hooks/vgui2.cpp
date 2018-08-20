#include "strtools.h"
#include "hooks.h"
#include "tier0/memdbgon.h"

HOOK_DETOUR_DECLARE(hkVguiFind);

NOINLINE wchar_t* __fastcall hkVguiFind(void* ecx, void* edx, const char* pName)
{
	static const wchar_t* wzNull = L"";
	bool test = false;

	if (test)
	{
		size_t nameLength = strlen(pName);
		wchar_t* newStr = new wchar_t[nameLength + 1];
		MultiByteToWideChar(CP_ACP, NULL, pName, strlen(pName), newStr, nameLength + 1);
		newStr[nameLength] = '\0';
		return newStr;
	}			 
 
	return HOOK_DETOUR_GET_ORIG(hkVguiFind)(ecx, edx, pName);
}

ON_LOAD_LIB(vgui2)
{ 
	//HOOK_DETOUR(dwVguiBase + 0x95F0, hkVguiFind);
}

