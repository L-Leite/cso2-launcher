#pragma once

class IFileSystem;

const char* FS_GetBinDirectory();
void FS_SetBasePaths( IFileSystem* pFileSystem );
void FS_SetCustomPaths( IFileSystem* pFileSystem );