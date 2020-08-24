#pragma once

#include <fmt/format.h>
#include <string_view>

#define _LogAssert_STR( x ) #x
#define LogAssert( x )                                         \
    if ( !( x ) )                                              \
    {                                                          \
        Log::Error( "Assertion failed ({}) at file {}, {}\n",  \
                    _LogAssert_STR( x ), __FILE__, __LINE__ ); \
        abort();                                               \
    }

class Log
{
public:
    template <typename... Args>
    inline static void Debug( std::string_view format, const Args&... args )
    {
        Log::PrintDebug( format.data(), fmt::make_format_args( args... ) );
    }

    template <typename... Args>
    inline static void Info( std::string_view format, const Args&... args )
    {
        Log::PrintInfo( format.data(), fmt::make_format_args( args... ) );
    }

    template <typename... Args>
    inline static void Warning( std::string_view format, const Args&... args )
    {
        Log::PrintWarning( format.data(), fmt::make_format_args( args... ) );
    }

    template <typename... Args>
    inline static void Error( std::string_view format, const Args&... args )
    {
        Log::PrintError( format.data(), fmt::make_format_args( args... ) );
    }

private:
    static void PrintDebug( const char* format, fmt::format_args args );
    static void PrintInfo( const char* format, fmt::format_args args );
    static void PrintWarning( const char* format, fmt::format_args args );
    static void PrintError( const char* format, fmt::format_args args );

    // disable constructors and copy assignments
    // (move assignments are disabled implicitly)
private:
    Log() = delete;
    ~Log() = delete;
    Log( const Log& ) = delete;
    Log& operator=( const Log& ) = delete;
};
