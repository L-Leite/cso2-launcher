#pragma once

#include <cstdint>
#include <map>

// stores module base addresses
// if there is no module entry, look it up with GetModuleHandle
// it should be cheaper than calling GetModuleHandle multiple times
// maybe replace std::map with a faster container
class CModuleList
{
public:
	CModuleList() = default;
	~CModuleList() = default;
	CModuleList(const CModuleList&) = delete;
	CModuleList& operator=(const CModuleList&) = delete;

	uintptr_t Get(const std::string& szModuleName);

protected:
	bool LookupModule(const std::string& szModuleName);

private:
	std::map<const std::string, uintptr_t> m_Modules;
};

extern CModuleList g_ModuleList;
