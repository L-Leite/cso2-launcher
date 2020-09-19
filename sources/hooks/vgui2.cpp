#include <sstream>
#include <string_view>

#include <tier0/icommandline.hpp>
#include <vgui/ilocalize.hpp>

#include "hooks.hpp"
#include "platform.hpp"
#include "source/tierlibs.hpp"

//
// Formats a string as "resource/[game prefix]_[language].txt"
//
std::string GetDesiredLangFile( std::string_view szGamePrefix,
                                std::string_view szLanguage )
{
    std::ostringstream oss;
    oss << "resource/" << szGamePrefix << "_" << szLanguage << ".txt";
    return oss.str();
}

static std::unique_ptr<PLH::x86Detour> g_pStrTblAddHook;
static uint64_t g_StrTblAddOrig = NULL;

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
        return PLH::FnCast( g_StrTblAddOrig, hkStrTblAddFile )(
            ecx, edx, szFileName, pPathID, bIncludeFallbackSearchPaths );
    }

    std::string_view fileNameView = szFileName;
    std::string szDesiredFile;

    // Verify if we want to replace the current language file
    if ( fileNameView == "resource/cso2_koreana.txt" ||
         fileNameView == "resource/cso2_china.txt" )
    {
        szDesiredFile = GetDesiredLangFile( "cso2", szDesiredLang );
    }
    else if ( fileNameView == "resource/cstrike_korean.txt" ||
              fileNameView == "resource/cstrike_schinese.txt" )
    {
        szDesiredFile = GetDesiredLangFile( "cstrike", szDesiredLang );
    }
    else if ( fileNameView == "resource/chat_korean.txt" ||
              fileNameView == "resource/chat_schinese.txt" )
    {
        szDesiredFile = GetDesiredLangFile( "chat", szDesiredLang );
    }
    else if ( fileNameView == "resource/valve_korean.txt" ||
              fileNameView == "resource/valve_schinese.txt" )
    {
        szDesiredFile = GetDesiredLangFile( "valve", szDesiredLang );
    }
    else
    {
        // if we don't want to replace the existing file, resume ordinary
        // behavior
        return PLH::FnCast( g_StrTblAddOrig, hkStrTblAddFile )(
            ecx, edx, szFileName, pPathID, bIncludeFallbackSearchPaths );
    }

    // load our desired language file
    return PLH::FnCast( g_StrTblAddOrig, hkStrTblAddFile )(
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

    if ( strncpy_s( szOutBuffer, iOutBufferSize, szInput, iStrLen ) != 0 )
    {
        assert( false );
        return 0;
    }

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

static std::unique_ptr<PLH::VTableSwapHook> g_pStrTblHook;
static PLH::VFuncMap g_StrTblOrig;

NOINLINE int __fastcall hkLocalToUtf8( void*, void*, const char* szInput,
                                       char* szOutBuffer,
                                       std::uint32_t iOutBufferSize )
{
    return HandleLocalConvertion( szInput, szOutBuffer, iOutBufferSize );
}

NOINLINE int __fastcall hkUtf8ToLocal( void*, void*, const char* szInput,
                                       char* szOutBuffer,
                                       std::uint32_t iOutBufferSize )
{
    return HandleLocalConvertion( szInput, szOutBuffer, iOutBufferSize );
}

NOINLINE int __fastcall hkWideCharToUtf8( void*, void*, const wchar_t* szInput,
                                          char* szOutBuffer,
                                          std::uint32_t iOutBufferSize )
{
    return ConvertWideCharToUtf8( szInput, szOutBuffer, iOutBufferSize );
}

NOINLINE int __fastcall hkUtf8ToWideChar( void*, void*, const char* szInput,
                                          wchar_t* szOutBuffer,
                                          std::uint32_t iOutBufferSize )
{
    return ConvertUtf8ToWideChar( szInput, szOutBuffer, iOutBufferSize );
}

NOINLINE int __fastcall hkWideCharToUtf8_2( void*, void*,
                                            const wchar_t* szInput,
                                            char* szOutBuffer,
                                            std::uint32_t iOutBufferSize )
{
    return ConvertWideCharToUtf8( szInput, szOutBuffer, iOutBufferSize );
}

// we need this earlier than we link the tier libraries, so get it here
vgui::ILocalize* GetVGuiLocalize()
{
    auto factory = reinterpret_cast<CreateInterfaceFn>( Sys_GetModuleExport(
        Sys_GetModuleBase( "vgui2.dll" ), "CreateInterface" ) );

    if ( factory == nullptr )
    {
        return nullptr;
    }

    return reinterpret_cast<vgui::ILocalize*>(
        factory( VGUI_LOCALIZE_INTERFACE_VERSION, NULL ) );
}

void OnVguiLoaded( const uintptr_t dwVguiBase )
{
    static bool bHasLoaded = false;

    if ( bHasLoaded )
    {
        return;
    }

    bHasLoaded = true;

    PLH::CapstoneDisassembler dis( PLH::Mode::x86 );

    g_pStrTblAddHook = SetupDetourHook( dwVguiBase + 0x8D90, &hkStrTblAddFile,
                                        &g_StrTblAddOrig, dis );
    g_pStrTblAddHook->hook();

    // does multiple hooks in CLocalizedStringTable
    static const PLH::VFuncMap deviceRedirects = {
        { uint16_t( 20 ), reinterpret_cast<uint64_t>( &hkWideCharToUtf8 ) },
        { uint16_t( 21 ), reinterpret_cast<uint64_t>( &hkUtf8ToLocal ) },
        { uint16_t( 22 ), reinterpret_cast<uint64_t>( &hkLocalToUtf8 ) },
        { uint16_t( 23 ), reinterpret_cast<uint64_t>( &hkUtf8ToWideChar ) },
        { uint16_t( 24 ), reinterpret_cast<uint64_t>( &hkWideCharToUtf8_2 ) },
    };

    g_pStrTblHook = SetupVtableSwap( GetVGuiLocalize(), deviceRedirects );
    g_pStrTblHook->hook();
    g_StrTblOrig = g_pStrTblHook->getOriginals();
}
