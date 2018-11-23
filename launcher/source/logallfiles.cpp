#include "logallfiles.hpp"

#include "tier0/ICommandLine.h"
#include "tier1/fmtstr.h"
#include "tier1/tier1.h"

#include "filesystem.h"

constexpr const char *ALL_RESLIST_FILE = "all.lst";
constexpr const char *ENGINE_RESLIST_FILE = "engine.lst";

CLogAllFiles g_LogFiles;

void SortResList( char const *pchFileName, char const *pchSearchPath );

static bool AllLogLessFunc( CUtlString const &pLHS, CUtlString const &pRHS )
{
    return CaselessStringLessThan( pLHS.Get(), pRHS.Get() );
}

CLogAllFiles::CLogAllFiles() : m_ResListDir( "reslists" ) {}

void CLogAllFiles::Init( const std::string &szBaseDir )
{
    m_szBaseDir = szBaseDir;

    // Can't do this in edit mode
    if ( CommandLine()->CheckParm( "-edit" ) )
    {
        return;
    }

    if ( !CommandLine()->CheckParm( "-makereslists" ) )
    {
        return;
    }

    m_bActive = true;

    char const *pszDir = nullptr;
    if ( CommandLine()->CheckParm( "-reslistdir", &pszDir ) && pszDir )
    {
        m_ResListDir = pszDir;
    }

    // game directory has not been established yet, must derive ourselves
    m_FullGamePath = m_szBaseDir;
    m_FullGamePath += CommandLine()->ParmValue( "-game", "hl2" );

    // create file to dump out to
    std::filesystem::path fullResPath = m_FullGamePath;
    fullResPath += m_ResListDir;
    g_pFullFileSystem->CreateDirHierarchy( fullResPath.string().c_str(),
                                           "GAME" );

    g_pFullFileSystem->AddLoggingFunc( &LogAllFilesFunc );

    if ( !CommandLine()->FindParm( "-startmap" ) &&
         !CommandLine()->FindParm( "-startstage" ) )
    {
        std::filesystem::path allResFilePath = fullResPath;
        allResFilePath += ALL_RESLIST_FILE;
        g_pFullFileSystem->RemoveFile( allResFilePath.string().c_str(),
                                       "GAME" );
    }
}

void CLogAllFiles::Shutdown()
{
    if ( !m_bActive )
    {
        return;
    }

    m_bActive = false;

    if ( CommandLine()->CheckParm( "-makereslists" ) )
    {
        g_pFullFileSystem->RemoveLoggingFunc( &LogAllFilesFunc );
    }

    std::filesystem::path fullResPath = m_FullGamePath;
    fullResPath += m_ResListDir;
    std::filesystem::path allResFilePath = fullResPath;
    allResFilePath += ALL_RESLIST_FILE;
    std::filesystem::path engineResFilePath = fullResPath;
    engineResFilePath += ENGINE_RESLIST_FILE;

    // Now load and sort all.lst
    SortResList( allResFilePath.string().c_str(), "GAME" );
    // Now load and sort engine.lst
    SortResList( engineResFilePath.string().c_str(), "GAME" );
}

void CLogAllFiles::LogToAllReslist( char const *line )
{
    std::filesystem::path allResFilePath = m_FullGamePath;
    allResFilePath += m_ResListDir;
    allResFilePath += ALL_RESLIST_FILE;

    // Open for append, write data, close.
    FileHandle_t fh = g_pFullFileSystem->Open( allResFilePath.string().c_str(),
                                               "at", "GAME" );
    if ( fh != FILESYSTEM_INVALID_HANDLE )
    {
        std::string szLine = "\"";
        szLine += line;
        szLine += "\"\n";

        g_pFullFileSystem->Write( szLine.c_str(),
                                  static_cast<int>( szLine.length() ), fh );
        g_pFullFileSystem->Close( fh );
    }
}

void CLogAllFiles::LogFile( const char *fullPathFileName, const char *options )
{
    if ( !m_bActive )
    {
        Assert( 0 );
        return;
    }

    // write out to log file
    Assert( fullPathFileName[1] == ':' );

    if ( std::find( m_vLogged.begin(), m_vLogged.end(), fullPathFileName ) ==
         m_vLogged.end() )
    {
        return;
    }

    m_vLogged.emplace_back( fullPathFileName );

    // make it relative to our root directory
    std::string szRelativePath = fullPathFileName;
    szRelativePath += m_szBaseDir;
    LogToAllReslist( szRelativePath.c_str() );
}

//-----------------------------------------------------------------------------
// Purpose: callback function from filesystem
//-----------------------------------------------------------------------------
void CLogAllFiles::LogAllFilesFunc( const char *fullPathFileName,
                                    const char *options )
{
    g_LogFiles.LogFile( fullPathFileName, options );
}