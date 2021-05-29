#include "buffer/view.hpp"

#include <stdexcept>

BufferView::BufferView( uint8_t* data, std::size_t dataSize )
    : m_Data( data ), m_DataSize( dataSize ), m_CurDataOffset( 0 )
{
}

std::string BufferView::ReadString()
{
    std::size_t length = this->Read<std::uint8_t>();

    if ( this->CanReadBytes( length ) == false )
    {
        throw std::length_error(
            "The string's length is larger than the available data" );
    }

    std::string newStr;

    if ( length > 0 )
    {
        auto strStart = reinterpret_cast<const char*>(
            &this->m_Data[this->m_CurDataOffset] );

        newStr.reserve( length + 1 );
        newStr.append( strStart, length );

        this->m_CurDataOffset += length;
    }

    return newStr;
}

std::string BufferView::ReadLongString()
{
    std::size_t length = this->Read<std::uint16_t>();

    if ( this->CanReadBytes( length ) == false )
    {
        throw std::length_error(
            "The string's length is larger than the available data" );
    }

    std::string newStr;

    if ( length > 0 )
    {
        auto strStart = reinterpret_cast<const char*>(
            &this->m_Data[this->m_CurDataOffset] );

        newStr.reserve( length + 1 );
        newStr.append( strStart, length );

        this->m_CurDataOffset += length;
    }

    return newStr;
}

void BufferView::ReadImpl( uint8_t* data, std::size_t dataSize )
{
    if ( this->CanReadBytes( dataSize ) == false )
    {
        throw std::length_error(
            "The desired data's length is larger than the available data" );
    }

    auto dataStart = reinterpret_cast<const std::uint8_t*>(
        &this->m_Data[this->m_CurDataOffset] );

    std::copy( dataStart, dataStart + dataSize, data );
    this->m_CurDataOffset += dataSize;
}
