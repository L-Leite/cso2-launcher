#include "stdafx.hpp"
#include "modulelist.hpp"

CModuleList g_ModuleList;

uintptr_t CModuleList::Get( const std::string& szModuleName )
{
    if ( szModuleName.empty() )
    {
        return NULL;
    }

    if ( m_Modules.find( szModuleName ) == m_Modules.end() &&
         !LookupModule( szModuleName ) )
    {
        return NULL;
    }

    return m_Modules[szModuleName];
}

bool CModuleList::LookupModule( const std::string& szModuleName )
{
    HMODULE hModuleHandle = GetModuleHandleA( szModuleName.c_str() );

    if ( !hModuleHandle )
    {
        return false;
    }

    m_Modules[szModuleName] = reinterpret_cast<uintptr_t>( hModuleHandle );

    return true;
}
