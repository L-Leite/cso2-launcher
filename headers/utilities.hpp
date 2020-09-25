#pragma once

#include <algorithm>
#include <array>
#include <string>

namespace utils
{
#ifdef _WIN32
class COverrideMemProtection
{
public:
    COverrideMemProtection( void* pAddress, size_t iMemSize, DWORD dwNewProt )
    {
        m_pAddress = pAddress;
        m_iMemSize = iMemSize;
        VirtualProtect( pAddress, iMemSize, dwNewProt, &m_dwOriginalProt );
    }

    ~COverrideMemProtection()
    {
        VirtualProtect( m_pAddress, m_iMemSize, m_dwOriginalProt,
                        &m_dwOriginalProt );
    }

private:
    void* m_pAddress;
    size_t m_iMemSize;
    DWORD m_dwOriginalProt;
};
#endif

//
// write memory to a write protected address
//
template <size_t iDataSize>
inline void WriteProtectedMemory( const std::array<uint8_t, iDataSize>& data,
                                  uintptr_t pDestination )
{
    uint8_t* pTarget = reinterpret_cast<uint8_t*>( pDestination );
#ifdef _WIN32
    COverrideMemProtection p( pTarget, iDataSize, PAGE_READWRITE );
#else
#error WriteProtectedMemory - Implement me in your target platform
#endif
    std::copy( data.begin(), data.end(), pTarget );
}

inline void ToLower( std::string& str )
{
    std::transform( str.begin(), str.end(), str.begin(), []( unsigned char c ) {
        return static_cast<char>( std::tolower( c ) );
    } );
}
}  // namespace utils
