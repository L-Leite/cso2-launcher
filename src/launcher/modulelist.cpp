#include "modulelist.h"
#include <Windows.h>

CModuleList g_ModuleList;

CModuleList::CModuleList()
{	
}

uintptr_t CModuleList::Get(const wchar_t* szModuleName)
{ 
	if (!szModuleName)
		return NULL;

	if (m_Modules.find(szModuleName) == m_Modules.end()
		&& !LookupModule(szModuleName))
		return NULL;

	return m_Modules[szModuleName];
}  

bool CModuleList::LookupModule(const wchar_t* szModuleName)
{ 
	HMODULE hModuleHandle = GetModuleHandleW(szModuleName);

	if (!hModuleHandle)
		return false;

	m_Modules[szModuleName] = (uintptr_t)hModuleHandle;

	return true;
}
