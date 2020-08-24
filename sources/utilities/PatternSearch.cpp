#include "utilities/PatternSearch.hpp"

#include <algorithm>
#include <memory>

#include <climits>

#define REBASE( pRVA, baseOld, baseNew ) \
    ( (size_t)pRVA - (size_t)baseOld + (size_t)baseNew )

PatternSearch::PatternSearch( const std::vector<uint8_t>& pattern )
    : _pattern( pattern )
{
}

PatternSearch::PatternSearch( const std::initializer_list<uint8_t>&& pattern )
    : _pattern( pattern )
{
}

PatternSearch::PatternSearch( const std::string& pattern )
    : _pattern( pattern.begin(), pattern.end() )
{
}

PatternSearch::PatternSearch( const char* pattern, size_t len /*= 0*/ )
    : _pattern( pattern, pattern + ( len ? len : strlen( pattern ) ) )
{
}

PatternSearch::PatternSearch( const uint8_t* pattern, size_t len /*= 0*/ )
    : _pattern( pattern,
                pattern + ( len ? len : strlen( (const char*)pattern ) ) )
{
}

/// <summary>
/// Default pattern matching with wildcards.
/// std::search is approximately 2x faster than naive approach.
/// </summary>
/// <param name="wildcard">Pattern wildcard</param>
/// <param name="scanStart">Starting address</param>
/// <param name="scanSize">Size of region to scan</param>
/// <param name="out">Found results</param>
/// <param name="value_offset">Value that will be added to resulting
/// addresses</param> <returns>Number of found addresses</returns>
size_t PatternSearch::Search( uint8_t wildcard, void* scanStart,
                              size_t scanSize, std::vector<uintptr_t>& out,
                              uintptr_t value_offset /*= 0*/
                              ) const
{
    const uint8_t* cstart = (const uint8_t*)scanStart;
    const uint8_t* cend = cstart + scanSize;

    auto comparer = [&wildcard]( uint8_t val1, uint8_t val2 ) {
        return ( val1 == val2 || val2 == wildcard );
    };

    for ( ;; )
    {
        const uint8_t* res = std::search( cstart, cend, _pattern.begin(),
                                          _pattern.end(), comparer );
        if ( res >= cend )
            break;

        if ( value_offset != 0 )
            out.emplace_back( REBASE( res, scanStart, value_offset ) );
        else
            out.emplace_back( reinterpret_cast<uintptr_t>( res ) );

        cstart = res + _pattern.size();
    }

    return out.size();
}

/// <summary>
/// Full pattern match, no wildcards.
/// Uses Boyer–Moore–Horspool algorithm.
/// </summary>
/// <param name="scanStart">Starting address</param>
/// <param name="scanSize">Size of region to scan</param>
/// <param name="out">Found results</param>
/// <param name="value_offset">Value that will be added to resulting
/// addresses</param> <returns>Number of found addresses</returns>
size_t PatternSearch::Search( void* scanStart, size_t scanSize,
                              std::vector<uintptr_t>& out,
                              uintptr_t value_offset /*= 0*/
                              ) const
{
    size_t bad_char_skip[UCHAR_MAX + 1];

    const uint8_t* haystack = reinterpret_cast<const uint8_t*>( scanStart );
    const uint8_t* needle = &_pattern[0];
    uintptr_t nlen = _pattern.size();
    uintptr_t scan = 0;
    uintptr_t last = nlen - 1;

    //
    // Preprocess
    //
    for ( scan = 0; scan <= UCHAR_MAX; ++scan )
        bad_char_skip[scan] = nlen;

    for ( scan = 0; scan < last; ++scan )
        bad_char_skip[needle[scan]] = last - scan;

    //
    // Search
    //
    while ( scanSize >= static_cast<size_t>( nlen ) )
    {
        for ( scan = last; haystack[scan] == needle[scan]; --scan )
        {
            if ( scan == 0 )
            {
                if ( value_offset != 0 )
                    out.emplace_back(
                        REBASE( haystack, scanStart, value_offset ) );
                else
                    out.emplace_back( reinterpret_cast<uintptr_t>( haystack ) );

                break;
            }
        }

        scanSize -= bad_char_skip[haystack[last]];
        haystack += bad_char_skip[haystack[last]];
    }

    return out.size();
}
