#pragma once

#include <containers/utlmemory.hpp>
#include <tier1/byteswap.hpp>

class CUtlBuffer
{
public:
    enum SeekType_t
    {
        SEEK_HEAD = 0,
        SEEK_CURRENT,
        SEEK_TAIL
    };

    // flags
    enum BufferFlags_t
    {
        TEXT_BUFFER =
            0x1,  // Describes how get + put work (as strings, or binary)
        EXTERNAL_GROWABLE = 0x2,  // This is used w/ external buffers and causes
                                  // the utlbuf to switch to reallocatable
                                  // memory if an overflow happens when Putting.
        CONTAINS_CRLF =
            0x4,  // For text buffers only, does this contain \n or \n\r?
        READ_ONLY = 0x8,  // For external buffers; prevents null termination
                          // from happening.
        AUTO_TABS_DISABLED = 0x10,  // Used to disable/enable push/pop tabs
    };

    // Overflow functions when a get or put overflows
    typedef bool ( CUtlBuffer::*UtlBufferOverflowFunc_t )( int nSize );

    // Constructors for growable + external buffers for
    // serialization/unserialization
    CUtlBuffer( int growSize = 0, int initSize = 0, int nFlags = 0 )
        : m_Memory( growSize, initSize ), m_Get( 0 ), m_Put( 0 ), m_Error( 0 ),
          m_Flags( nFlags ), m_nTab( 0 ), m_nOffset( 0 )
    {
        if ( ( initSize != 0 ) && !IsReadOnly() )
        {
            m_nMaxPut = -1;
            AddNullTermination();
        }
        else
        {
            m_nMaxPut = 0;
        }

        SetOverflowFuncs( &CUtlBuffer::GetOverflow, &CUtlBuffer::PutOverflow );
    }
    virtual ~CUtlBuffer() = default;

    inline bool IsReadOnly() const { return ( m_Flags & READ_ONLY ) != 0; }

    inline bool IsGrowable() const
    {
        return ( m_Flags & EXTERNAL_GROWABLE ) != 0;
    }

    inline void* Base() { return m_Memory.Base(); }
    inline int Size() const { return m_Memory.NumAllocated(); }

protected:
    // error flags
    enum
    {
        PUT_OVERFLOW = 0x1,
        GET_OVERFLOW = 0x2,
        MAX_ERROR_FLAG = GET_OVERFLOW,
    };

    void SetOverflowFuncs( UtlBufferOverflowFunc_t getFunc,
                           UtlBufferOverflowFunc_t putFunc )
    {
        m_GetOverflowFunc = getFunc;
        m_PutOverflowFunc = putFunc;
    }

    // Default overflow funcs
    bool PutOverflow( int nSize )
    {
        /* if ( m_Memory.IsExternallyAllocated() )
        {
            if ( !IsGrowable() )
                return false;

            m_Memory.ConvertToGrowableMemory( 0 );
        } */

        while ( Size() < m_Put - m_nOffset + nSize )
        {
            m_Memory.Grow();
        }

        return true;
    }

    bool GetOverflow( int nSize ) { return false; }

    bool OnPutOverflow( int nSize )
    {
        return ( this->*m_PutOverflowFunc )( nSize );
    }

    bool OnGetOverflow( int nSize )
    {
        return ( this->*m_GetOverflowFunc )( nSize );
    }

    bool CheckPut( int nSize )
    {
        if ( ( m_Error & PUT_OVERFLOW ) || IsReadOnly() )
            return false;

        if ( ( m_Put < m_nOffset ) ||
             ( m_Memory.NumAllocated() < m_Put - m_nOffset + nSize ) )
        {
            if ( !OnPutOverflow( nSize ) )
            {
                m_Error |= PUT_OVERFLOW;
                return false;
            }
        }
        return true;
    }

    void AddNullTermination( void )
    {
        if ( m_Put > m_nMaxPut )
        {
            if ( !IsReadOnly() && ( ( m_Error & PUT_OVERFLOW ) == 0 ) )
            {
                // Add null termination value
                if ( CheckPut( 1 ) )
                {
                    int Index = m_Put - m_nOffset;

                    if ( Index >= 0 )
                    {
                        m_Memory[Index] = 0;
                    }
                }
                else
                {
                    // Restore the overflow state, it was valid before...
                    m_Error &= ~PUT_OVERFLOW;
                }
            }
            m_nMaxPut = m_Put;
        }
    }

    CUtlMemory<unsigned char> m_Memory;
    int m_Get;
    int m_Put;

    unsigned char m_Error;
    unsigned char m_Flags;
    unsigned char m_Reserved;

    int m_nTab;
    int m_nMaxPut;
    int m_nOffset;

    UtlBufferOverflowFunc_t m_GetOverflowFunc;
    UtlBufferOverflowFunc_t m_PutOverflowFunc;

    CByteswap m_Byteswap;
};
