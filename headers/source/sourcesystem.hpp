#pragma once

#include <string>

class AppSystemsTable;

class IEngineAPI;
class IFileSystem;
class IHammer;
class IMaterialSystem;

class CSourceSystem
{
public:
    CSourceSystem( AppSystemsTable& appSysTable, void* instance,
                   IFileSystem* fs );
    ~CSourceSystem();

    bool Init();
    int MainEntrypont() const;
    void Destroy();

private:
    bool LoadRequiredLibraries();
    bool InitTierLibraries();
    void SetEngineStartupInfo();

    bool AddRequiredSystems() const;
    bool InitHammer();
    bool InitMaterialSystemAPI();

    const char* GetDefaultMod() const;
    const char* GetDefaultGame() const;

private:
    std::string m_szGameDirPath;

    AppSystemsTable& m_AppSysTable;
    void* m_pInstance;

    IFileSystem* m_pFileSystem;
    IEngineAPI* m_pEngineApi;
    IHammer* m_pHammer;

    bool m_bTextMode;
    bool m_bHammerMode;

    bool m_bSystemsConnected;
    bool m_bSystemsInitialized;
};