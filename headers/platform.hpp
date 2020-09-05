#pragma once

#include <filesystem>
namespace fs = std::filesystem;

#include <tier0/dbg.hpp>

fs::path Game_GetBaseDirectory();

bool InitTextMode();
SpewRetval_t LauncherDefaultSpewFunc( SpewType_t spewType, const char* pMsg );

bool NET_Init();
bool NET_Destroy();

void Proc_SetHighPriority();
void Proc_SetLowPriority();

void* Sys_LoadLibrary( const char* moduleName );
void Sys_FreeLibrary( void* moduleBase );
void* Sys_GetModuleBase( const char* moduleName );
void* Sys_GetModuleExport( void* moduleBase, const char* exportName );
void Sys_InitPlatformSpecific();
void Sys_DestroyPlatformSpecific();

void User_MessageBox( const char* msg, const char* boxTitle );

template <typename fnType>
inline fnType Sys_GetModuleExport( void* moduleBase, const char* exportName )
{
    return reinterpret_cast<fnType>(
        Sys_GetModuleExport( moduleBase, exportName ) );
}