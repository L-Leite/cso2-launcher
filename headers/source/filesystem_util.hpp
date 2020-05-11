#pragma once

#include <filesystem>
namespace fs = std::filesystem;

class IFileSystem;

const char* FS_GetBinDirectory();
fs::path FS_GetFileSystemLibPath();
void FS_SetBasePaths( IFileSystem* pFileSystem );