#pragma once

#include <filesystem>
#include <string>

#include "tier0/ICommandLine.h"

class CBaseDirectory
{
public:
    CBaseDirectory()
    {
        const char* szOverrideDir = CommandLine()->CheckParm( "-basedir" );

        if ( szOverrideDir )
        {
            m_BaseDir = szOverrideDir;
            std::filesystem::current_path( szOverrideDir );
        }
        else
        {
            m_BaseDir = std::filesystem::current_path();
        }
    }

    std::string GetBaseDir() const
    {
        return m_BaseDir.string();
    }

private:
    std::filesystem::path m_BaseDir;
};
