#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <string_view>

// stores modules base addresses
// if there is no module entry, look it up with GetModuleHandle
// it should be cheaper than calling GetModuleHandle multiple times
// maybe replace std::map with a faster container
class ModuleCache
{
public:
    ModuleCache() = default;
    ~ModuleCache() = default;

    // don't copy
    ModuleCache( const ModuleCache& ) = delete;
    ModuleCache& operator=( const ModuleCache& ) = delete;

    uintptr_t Get( std::string_view szvModuleName );

private:
    uintptr_t GetModuleInCache( std::string_view szvModuleName ) const; 
    uintptr_t LookupModule( std::string_view szvModuleName );

private:
    std::map<size_t, uintptr_t> m_Modules;
};

extern ModuleCache g_ModuleList;
