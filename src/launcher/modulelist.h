#pragma once

#include <stdint.h>
#include <map>

class CModuleList
{
public:
	CModuleList();
	~CModuleList() = default;

	uintptr_t Get(const wchar_t* szModuleName);

protected:
	bool LookupModule(const wchar_t* szModuleName);

private:
	std::map<const wchar_t*, uintptr_t> m_Modules;
};

extern CModuleList g_ModuleList;
