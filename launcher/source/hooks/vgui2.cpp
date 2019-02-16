#include "stdafx.hpp"

#include "hooks.hpp"
#include "tier0/icommandline.h"

//
// Formats a string as "resource/[game prefix]_[language].txt"
//
std::string GetDesiredLangFile( const std::string& szGamePrefix,
                                const char* szLanguage )
{
    std::ostringstream oss;
    oss << "resource/" << szGamePrefix << "_" << szLanguage << ".txt";
    return oss.str();
}

HOOK_DETOUR_DECLARE( hkStrTblAddFile );

//
// Allow the user to specify some language file through command line arguments
// "-lang [desired language]"
// And load the language file from "resource/cso2_[desired language].txt".
//
// Original function: CLocalizedStringTable::AddFile
NOINLINE bool __fastcall hkStrTblAddFile( void* ecx, void* edx,
                                          const char* szFileName,
                                          const char* pPathID,
                                          bool bIncludeFallbackSearchPaths )
{
    const char* szDesiredLang = CommandLine()->ParmValue( "-lang" );

    // Make sure we have a command argument
    if ( !szDesiredLang )
    {
        return HOOK_DETOUR_GET_ORIG( hkStrTblAddFile )(
            ecx, edx, szFileName, pPathID, bIncludeFallbackSearchPaths );
    }

    std::string_view fileNameView = szFileName;
    std::string szDesiredFile;

    // Verify if we want to replace the current language file
    if ( fileNameView == "resource/cso2_koreana.txt" )
    {
        szDesiredFile = GetDesiredLangFile( "cso2", szDesiredLang );
    }
    else if ( fileNameView == "resource/cstrike_korean.txt" )
    {
        szDesiredFile = GetDesiredLangFile( "cstrike", szDesiredLang );
    }
    else if ( fileNameView == "resource/chat_korean.txt" )
    {
        szDesiredFile = GetDesiredLangFile( "chat", szDesiredLang );
    }
    else if ( fileNameView == "resource/valve_korean.txt" )
    {
        szDesiredFile = GetDesiredLangFile( "valve", szDesiredLang );
    }
    else
    {
        // if we don't want to replace the existing file, resume ordinary
        // behavior
        return HOOK_DETOUR_GET_ORIG( hkStrTblAddFile )(
            ecx, edx, szFileName, pPathID, bIncludeFallbackSearchPaths );
    }

    // load our desired language file
    return HOOK_DETOUR_GET_ORIG( hkStrTblAddFile )(
        ecx, edx, szDesiredFile.c_str(), pPathID, true );
}

// the game converts the szInput (as an UTF8 string) to a wide char string
// and converts it back to a single byte string (using the OS's code page)
//
// example: converting string with hangul characters on an OS with a german code
// page would result in gibberish
//
// this fixes that by just copying the utf8 string to the out buffer
int HandleLocalConvertion( const char* szInput, char* szOutBuffer,
                           const std::uint32_t iOutBufferSize )
{
    const std::uint32_t iStrLen = std::strlen( szInput ) + 1;

    // make sure we don't overflow
    if ( iStrLen > iOutBufferSize )
    {
        assert( false );
        return 0;
    }

    std::strncpy( szOutBuffer, szInput, iStrLen );

    return iStrLen;
}

int ConvertWideCharToUtf8( const wchar_t* szInput, char* szOutput,
                           const std::uint32_t iOutBufferSize )
{
    int iLength = WideCharToMultiByte( CP_UTF8, NULL, szInput, -1, szOutput,
                                       iOutBufferSize, nullptr, nullptr );
    szOutput[iOutBufferSize - 1] = L'\0';
    return iLength;
}

int ConvertUtf8ToWideChar( const char* szInput, wchar_t* szOutput,
                           const std::uint32_t iOutBufferSize )
{
    int iLength = MultiByteToWideChar( CP_UTF8, NULL, szInput, -1, szOutput,
                                       iOutBufferSize );
    szOutput[iOutBufferSize - 1] = L'\0';
    return iLength;
}

HOOK_DETOUR_DECLARE( hkLocalToUtf8 );

NOINLINE int __fastcall hkLocalToUtf8( void* thisptr, void*,
                                       const char* szInput, char* szOutBuffer,
                                       std::uint32_t iOutBufferSize )
{
    return HandleLocalConvertion( szInput, szOutBuffer, iOutBufferSize );
}

HOOK_DETOUR_DECLARE( hkUtf8ToLocal );

NOINLINE int __fastcall hkUtf8ToLocal( void* thisptr, void*,
                                       const char* szInput, char* szOutBuffer,
                                       std::uint32_t iOutBufferSize )
{
    return HandleLocalConvertion( szInput, szOutBuffer, iOutBufferSize );
}

HOOK_DETOUR_DECLARE( hkWideCharToUtf8 );

NOINLINE int __fastcall hkWideCharToUtf8( void* thisptr, void*,
                                          const wchar_t* szInput,
                                          char* szOutBuffer,
                                          std::uint32_t iOutBufferSize )
{
    return ConvertWideCharToUtf8( szInput, szOutBuffer, iOutBufferSize );
}

HOOK_DETOUR_DECLARE( hkUtf8ToWideChar );

NOINLINE int __fastcall hkUtf8ToWideChar( void* thisptr, void*,
                                          const char* szInput,
                                          wchar_t* szOutBuffer,
                                          std::uint32_t iOutBufferSize )
{
    return ConvertUtf8ToWideChar( szInput, szOutBuffer, iOutBufferSize );
}

HOOK_DETOUR_DECLARE( hkWideCharToUtf8_2 );

NOINLINE int __fastcall hkWideCharToUtf8_2( void* thisptr, void*,
                                            const wchar_t* szInput,
                                            char* szOutBuffer,
                                            std::uint32_t iOutBufferSize )
{
    return ConvertWideCharToUtf8( szInput, szOutBuffer, iOutBufferSize );
}

void OnVguiLoaded( const uintptr_t dwVguiBase )
{			
	static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

    HOOK_DETOUR( dwVguiBase + 0x8D90, hkStrTblAddFile );
    HOOK_DETOUR( dwVguiBase + 0xB3F0, hkWideCharToUtf8 );
    HOOK_DETOUR( dwVguiBase + 0xB420, hkUtf8ToLocal );
    HOOK_DETOUR( dwVguiBase + 0xB4A0, hkLocalToUtf8 );
    HOOK_DETOUR( dwVguiBase + 0xB520, hkUtf8ToWideChar );
    HOOK_DETOUR( dwVguiBase + 0xB550, hkWideCharToUtf8_2 );
}
