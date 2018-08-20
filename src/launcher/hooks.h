#pragma once
#include <headers/CapstoneDisassembler.hpp>
#include <headers/Detour/x86Detour.hpp>
#include <headers/PE/EatHook.hpp>
#include "modulelist.h"
#include "onloadlib.h"

#define HOOK_DETOUR_DECLARE(trampoline) static PLH::x86Detour* _##trampoline = nullptr; \
										static uint64_t _##trampoline##Orig = NULL

#define HOOK_DETOUR(original, trampoline) PLH::CapstoneDisassembler _##trampolineDis(PLH::Mode::x86); \
										_##trampoline = new PLH::x86Detour((char*)(original), (char*)&trampoline, &_##trampoline##Orig, _##trampolineDis); \
										_##trampoline->hook();

#define HOOK_DETOUR_MODULE(mod, original, trampoline) PLH::CapstoneDisassembler _##trampolineDis(PLH::Mode::x86); \
										_##trampoline = new PLH::x86Detour((char*)(g_ModuleList.Get(mod) + (original)), (char*)trampoline, &_##trampoline##Orig, _##trampolineDis); \
										_##trampoline->hook();

#define HOOK_DETOUR_GET_ORIG(trampoline) ((decltype(&trampoline))_##trampoline##Orig)

#define HOOK_EXPORT_DECLARE(trampoline) static PLH::EatHook* _##trampoline = nullptr; \
										static uint64_t _##trampoline##Orig = NULL;

#define HOOK_EXPORT(lib, func, trampoline) _##trampoline \
										= new PLH::EatHook(lib, func, (char*)trampoline, &_##trampoline##Orig); \
									_##trampoline->hook();

#define HOOK_EXPORT_GET_ORIG(trampoline) ((decltype(&trampoline))_##trampoline##Orig)

template<size_t iDataSize>
inline void WriteProtectedMemory(uint8_t (&pData)[iDataSize], uintptr_t pDestination)
{ 
	DWORD dwOldProtect = NULL;
	VirtualProtect((void*)pDestination, iDataSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	memcpy((void*)pDestination, pData, iDataSize);
	VirtualProtect((void*)pDestination, iDataSize, dwOldProtect, &dwOldProtect);
}
