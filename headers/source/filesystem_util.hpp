#pragma once

class IFileSystem;

const char* FS_GetBinDirectory();
void FS_SetBasePaths( IFileSystem* pFileSystem );