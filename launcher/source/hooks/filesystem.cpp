#include "stdafx.hpp"

#include "hooks.hpp"

void BytePatchFilesystem( const uintptr_t dwFilesystemBase )
{
    //
    // IsFileEncrypted (1006A3B0, rva 6A3B0 @ FileSystem_Stdio.dll)
	// if the requested file's extension contains 'txt', 'csv', 'cfg' or 'db',
	// then it's an encrypted file ('etxt', 'ecsv', 'ecfg' or 'edb')
	// and returns true
	// 
	// this patch makes the function return false all the time
	// WARNING: encrypted files no longer work with this
    //
    // mov al, 1; retn
    const std::array<uint8_t, 5> encryptPatch = { 0xB0, 0x00, 0xC3 };
    WriteProtectedMemory( encryptPatch, ( dwFilesystemBase + 0x6A3B0 ) );
}

void OnFileSystemLoaded( const uintptr_t dwFilesystemBase )
{
    BytePatchFilesystem( dwFilesystemBase );
}
