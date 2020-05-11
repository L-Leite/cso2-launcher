#pragma once

#include "containers/utlmap.hpp"

inline bool CaselessStringLessThan( const char* const& lhs,
                                    const char* const& rhs )
{
    if ( !lhs )
        return false;
    if ( !rhs )
        return true;
    return ( stricmp( lhs, rhs ) < 0 );
}

template <class T, class I = int>
class CUtlDict
{
public:
    using DictElementMap_t = CUtlMap<const char*, T, I>;
    DictElementMap_t m_Elements;

    CUtlDict( int compareType = 0, int growSize = 0, int initSize = 0 )
        : m_Elements( growSize, initSize )
    {
        m_Elements.SetLessFunc( CaselessStringLessThan );
    }

    ~CUtlDict() { Purge(); }

    void Purge() { RemoveAll(); }

    void RemoveAll()
    {
        typename DictElementMap_t::IndexType_t index =
            m_Elements.FirstInorder();
        while ( index != m_Elements.InvalidIndex() )
        {
            free( (void*)m_Elements.Key( index ) );
            index = m_Elements.NextInorder( index );
        }

        m_Elements.RemoveAll();
    }

    I Insert( const char* pName, const T& element )
    {
        return m_Elements.Insert( strdup( pName ), element );
    }
};