#pragma once

class CByteswap
{
public:
    CByteswap()
    {
        // Default behavior sets the target endian to match the machine native
        // endian (no swap).
        SetTargetBigEndian( IsMachineBigEndian() );
    }

    inline void SetTargetBigEndian( bool bigEndian )
    {
        m_bBigEndian = bigEndian;
        m_bSwapBytes = IsMachineBigEndian() != bigEndian;
    }

    static bool IsMachineBigEndian()
    {
        short nIsBigEndian = 1;

        // if we are big endian, the first byte will be a 0, if little endian,
        // it will be a one.
        return (bool)( 0 == *(char*)&nIsBigEndian );
    }

private:
    unsigned int m_bSwapBytes : 1;
    unsigned int m_bBigEndian : 1;
};
