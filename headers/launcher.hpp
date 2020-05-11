
#pragma once

class Launcher
{
public:
    Launcher( void* instance, const char* cmdline );

    int Main();
    void Destroy();

private:
    void ParseCommandLine();

private:
    void* m_pInstance;
    bool m_bShowSplash;
};