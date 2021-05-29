#ifndef __BUFFERVIEW_H_
#define __BUFFERVIEW_H_

#include <cstdint>
#include <string>

class BufferView
{
public:
    BufferView( uint8_t* data, std::size_t dataSize );

    template <typename DATA_TYPE>
    inline DATA_TYPE Read()
    {
        DATA_TYPE result;
        this->ReadImpl( reinterpret_cast<std::uint8_t*>( &result ),
                        sizeof( DATA_TYPE ) );
        return result;
    }

    std::string ReadString();
    std::string ReadLongString();

    template <typename T, std::size_t ARRAY_SIZE>
    inline std::array<T, ARRAY_SIZE> ReadArray()
    {
        std::array<T, ARRAY_SIZE> result;
        this->ReadImpl( result );
        return result;
    }

protected:
    inline bool CanReadBytes( std::size_t bytesCount ) const
    {
        return this->m_DataSize >= this->m_CurDataOffset + bytesCount;
    }

    inline std::size_t GetDataViewSize() const { return this->m_DataSize; }

    void ReadImpl( uint8_t* data, std::size_t dataSize );

private:
    // span is only available on c++20
    // const std::span<const std::uint8_t> m_DataView;
    uint8_t* m_Data;
    std::size_t m_DataSize;

    std::size_t m_CurDataOffset;
};

#endif  // __BUFFERVIEW_H_
