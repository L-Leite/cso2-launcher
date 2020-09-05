#pragma once

#include <memory.h>

template <typename T, typename I = int>
class CUtlMemory
{
public:
    T* m_pMemory;
    int m_nAllocationCount;
    int m_nGrowSize;

    //-----------------------------------------------------------------------------
    // constructor, destructor
    //-----------------------------------------------------------------------------

    CUtlMemory( int nGrowSize, int nInitAllocationCount )
        : m_pMemory( 0 ), m_nAllocationCount( nInitAllocationCount ),
          m_nGrowSize( nGrowSize )
    {
        if ( m_nAllocationCount )
        {
            m_pMemory = (T*)malloc( m_nAllocationCount * sizeof( T ) );
        }
    }

    ~CUtlMemory() { Purge(); }

    class Iterator_t
    {
    public:
        Iterator_t( I i ) : index( i ) {}
        I index;

        bool operator==( const Iterator_t it ) const
        {
            return index == it.index;
        }
        bool operator!=( const Iterator_t it ) const
        {
            return index != it.index;
        }
    };
    Iterator_t First() const
    {
        return Iterator_t( IsIdxValid( 0 ) ? 0 : InvalidIndex() );
    }
    Iterator_t Next( const Iterator_t& it ) const
    {
        return Iterator_t( IsIdxValid( it.index + 1 ) ? it.index + 1 :
                                                        InvalidIndex() );
    }
    I GetIndex( const Iterator_t& it ) const { return it.index; }
    bool IsIdxAfter( I i, const Iterator_t& it ) const { return i > it.index; }
    bool IsValidIterator( const Iterator_t& it ) const
    {
        return IsIdxValid( it.index );
    }
    Iterator_t InvalidIterator() const { return Iterator_t( InvalidIndex() ); }

    // Specify the invalid ('null') index that we'll only return on failure
    static const I INVALID_INDEX = (I)-1;  // For use with COMPILE_TIME_ASSERT
    static I InvalidIndex() { return INVALID_INDEX; }

    inline T* Base() { return m_pMemory; }
    inline const T* Base() const { return m_pMemory; }

    inline bool IsIdxValid( I i ) const
    {
        // If we always cast 'i' and 'm_nAllocationCount' to unsigned then we
        // can do our range checking with a single comparison instead of two.
        // This gives a modest speedup in debug builds.
        return (uint32_t)i < (uint32_t)m_nAllocationCount;
    }

    //-----------------------------------------------------------------------------
    // element access
    //-----------------------------------------------------------------------------
    inline T& operator[]( I i ) { return m_pMemory[(uint32_t)i]; }
    inline const T& operator[]( I i ) const { return m_pMemory[(uint32_t)i]; }

    void Grow( int num = 1 )
    {
        // Make sure we have at least numallocated + num allocations.
        // Use the grow rules specified for this memory (in m_nGrowSize)
        int nAllocationRequested = m_nAllocationCount + num;

        auto caclNewAllocCount = []( int nAllocationCount, int nGrowSize,
                                     int nNewSize, int nBytesItem ) {
            if ( nGrowSize )
            {
                nAllocationCount =
                    ( ( 1 + ( ( nNewSize - 1 ) / nGrowSize ) ) * nGrowSize );
            }
            else
            {
                if ( !nAllocationCount )
                {
                    // Compute an allocation which is at least as big as a
                    // cache line...
                    nAllocationCount = ( 31 + nBytesItem ) / nBytesItem;
                }

                while ( nAllocationCount < nNewSize )
                {
                    nAllocationCount *= 2;
                }
            }

            return nAllocationCount;
        };

        int nNewAllocationCount =
            caclNewAllocCount( m_nAllocationCount, m_nGrowSize,
                               nAllocationRequested, sizeof( T ) );

        // if m_nAllocationRequested wraps index type I, recalculate
        if ( (int)(I)nNewAllocationCount < nAllocationRequested )
        {
            if ( (int)(I)nNewAllocationCount == 0 &&
                 (int)( I )( nNewAllocationCount - 1 ) >= nAllocationRequested )
            {
                --nNewAllocationCount;  // deal w/ the common case of
                                        // m_nAllocationCount == MAX_USHORT + 1
            }
            else
            {
                if ( (int)(I)nAllocationRequested != nAllocationRequested )
                {
                    // we've been asked to grow memory to a size s.t. the index
                    // type can't address the requested amount of memory
                    return;
                }
                while ( (int)(I)nNewAllocationCount < nAllocationRequested )
                {
                    nNewAllocationCount =
                        ( nNewAllocationCount + nAllocationRequested ) / 2;
                }
            }
        }

        m_nAllocationCount = nNewAllocationCount;

        if ( m_pMemory )
        {
            m_pMemory =
                (T*)realloc( m_pMemory, m_nAllocationCount * sizeof( T ) );
        }
        else
        {
            m_pMemory = (T*)malloc( m_nAllocationCount * sizeof( T ) );
        }
    }

    //-----------------------------------------------------------------------------
    // Memory deallocation
    //-----------------------------------------------------------------------------
    void Purge()
    {
        if ( m_pMemory )
        {
            free( (void*)m_pMemory );
            m_pMemory = 0;
        }
        m_nAllocationCount = 0;
    }

    inline int NumAllocated() const { return m_nAllocationCount; }

    inline bool IsExternallyAllocated() const { return m_nGrowSize < 0; }
};