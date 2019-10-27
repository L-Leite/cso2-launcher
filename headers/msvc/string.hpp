#pragma once

#include <cstring>
#include <string>

namespace msvc
{
template <typename value_type>
class basic_string
{
public:
    explicit basic_string( const value_type* r ) { this->AssignFromCstr( r ); }

    explicit basic_string( const std::basic_string<value_type>& r )
    {
        this->AssignFromStdStr( r );
    }

    ~basic_string()
    {
        if ( this->_Mysize > MAX_LOCAL_CAPACITY )
        {
            delete[] this->_Bx._Ptr;
        }
    }

    void operator=( const value_type* r ) { this->AssignFromCstr( r ); }
    void operator=( const std::basic_string<value_type>& r )
    {
        this->AssignFromStdStr( r );
    }

    void AssignFromCstr( const value_type* src )
    {
        const std::size_t iSrcLen = std::strlen( src ) + 1;

        if ( iSrcLen <= MAX_LOCAL_CAPACITY )
        {
            std::memcpy( this->_Bx._Buf, src, iSrcLen );
        }
        else
        {
            value_type* szNewStr = new value_type[iSrcLen];
            std::memcpy( szNewStr, src, iSrcLen );
            this->_Bx._Ptr = szNewStr;
        }

        this->_Mysize = iSrcLen;
        this->_Myres = iSrcLen;
    }

    void AssignFromStdStr( const std::basic_string<value_type>& src )
    {
        if ( src.size() <= MAX_LOCAL_CAPACITY )
        {
            std::memcpy( this->_Bx._Buf, src.data(), src.size() );
        }
        else
        {
            value_type* szNewStr = new value_type[src.max_size()];
            std::memcpy( szNewStr, src.data(), src.size() );
            this->_Bx._Ptr = szNewStr;
        }

        this->_Mysize = src.size();
        this->_Myres = src.max_size();
    }

protected:
    enum
    {
        // length of internal buffer, [1, 16]
        MSVC_BUF_SIZE =
            16 / sizeof( value_type ) < 1 ? 1 : 16 / sizeof( value_type ),
        MAX_LOCAL_CAPACITY = 15 / sizeof( value_type )
    };

    // storage for small buffer or pointer to larger one
    union _Bxty {
        value_type _Buf[MSVC_BUF_SIZE];
        value_type* _Ptr;
    } _Bx;

    std::size_t _Mysize;  // current length of string
    std::size_t _Myres;   // current storage reserved for string
};

typedef basic_string<char> string;
}  // namespace msvc
