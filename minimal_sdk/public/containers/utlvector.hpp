

#pragma once

#include "containers/constructor_helpers.hpp"
#include "containers/utlmemory.hpp"

template <typename T>
class CUtlVector
{
    using Allocator = CUtlMemory<T>;

public:
    Allocator m_Memory;
    int m_Size;
    T* m_pElements;

    CUtlVector( int growSize = 0, int initSize = 0 )
        : m_Memory( growSize, initSize ), m_Size( 0 )
    {
        m_pElements = m_Memory.Base();
    }

    void GrowVector( int num = 1 )
    {
        if ( m_Size + num > m_Memory.NumAllocated() )
        {
            m_Memory.Grow( m_Size + num - m_Memory.NumAllocated() );
        }

        m_Size += num;
        m_pElements = m_Memory.Base();
    }

    inline int AddToTail( const T& src ) { return InsertBefore( m_Size, src ); }

    inline T& Element( int i ) { return m_Memory[i]; }
    inline const T& Element( int i ) const { return m_Memory[i]; }

    int InsertBefore( int elem, const T& src )
    {
        GrowVector();
        ShiftElementsRight( elem );
        CopyConstruct( &Element( elem ), src );
        return elem;
    }

    void ShiftElementsRight( int elem, int num = 1 )
    {
        int numToMove = m_Size - elem - num;
        if ( ( numToMove > 0 ) && ( num > 0 ) )
            memmove( &Element( elem + num ), &Element( elem ),
                     numToMove * sizeof( T ) );
    }
};