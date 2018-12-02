#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "tier0/platform.h"
#include "tier1/utlrbtree.h"

// create file to dump out to
class CLogAllFiles
{
public:
    CLogAllFiles();
    void Init( const std::string& szBaseDir );
    void Shutdown();
    void LogFile( const char* fullPathFileName, const char* options );

private:
    static void LogAllFilesFunc( const char* fullPathFileName,
                                 const char* options );
    void LogToAllReslist( char const* line );

    bool m_bActive{ false };

    // persistent across restarts
    std::vector<std::string> m_vLogged;
    std::filesystem::path m_ResListDir;
    std::filesystem::path m_FullGamePath;
    std::string m_szBaseDir;
};

extern CLogAllFiles g_LogFiles;
