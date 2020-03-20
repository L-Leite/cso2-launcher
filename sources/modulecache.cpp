#include "modulecache.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

ModuleCache g_ModuleList;

inline uintptr_t GetModuleAddress( std::string_view szvModuleName )
{
    HMODULE hModuleHandle = GetModuleHandleA( szvModuleName.data() );
    return reinterpret_cast<uintptr_t>( hModuleHandle );
}

uintptr_t ModuleCache::Get( std::string_view szvModuleName )
{
    if ( szvModuleName.empty() )
    {
        return 0;
    }

    uintptr_t dwModAddr = this->GetModuleInCache( szvModuleName );

    if ( dwModAddr == 0 )
    {
        dwModAddr = this->LookupModule( szvModuleName );
    }

    return dwModAddr;
}

uintptr_t ModuleCache::GetModuleInCache( std::string_view szvModuleName ) const
{
    const size_t iModuleNameHash =
        std::hash<const char*>{}( szvModuleName.data() );

    const auto resIt = this->m_Modules.find( iModuleNameHash );

    if ( resIt == this->m_Modules.end() )
    {
        return 0;
    }

    return resIt->second;
}

uintptr_t ModuleCache::LookupModule( std::string_view szvModuleName )
{
    uintptr_t dwModuleAddr = GetModuleAddress( szvModuleName );

    if ( dwModuleAddr == 0 )
    {
        return 0;
    }

    const size_t iModuleNameHash =
        std::hash<const char*>{}( szvModuleName.data() );
    m_Modules[iModuleNameHash] = dwModuleAddr;

    return dwModuleAddr;
}
