#include "source/appsystemgroup.hpp"

CAppSystemGroup::CAppSystemGroup(
    const std::vector<AppSystemData>& realSystems )
    : m_SystemDict()
{
    for ( auto&& system : realSystems )
    {
        int newIndex = this->m_Systems.AddToTail( system.pSystem );
        this->m_SystemDict.Insert( system.szName.c_str(), newIndex );
    }

    this->m_pParentAppSystem = nullptr;
    m_nErrorStage = NONE;
}