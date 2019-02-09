#pragma once
#include <memory>

#include <headers/CapstoneDisassembler.hpp>
#include <headers/Detour/x86Detour.hpp>
#include <headers/PE/EatHook.hpp>

#define HOOK_DETOUR_DECLARE( trampoline )                           \
    static std::unique_ptr<PLH::x86Detour> _##trampoline = nullptr; \
    static uint64_t _##trampoline##Orig = NULL

#define HOOK_DETOUR( original, trampoline )                            \
    PLH::CapstoneDisassembler _##trampoline##Dis( PLH::Mode::x86 );    \
    _##trampoline = std::make_unique<PLH::x86Detour>(                  \
        (char*)( original ), (char*)&trampoline, &_##trampoline##Orig, \
        _##trampoline##Dis );                                          \
    _##trampoline->hook();

#define HOOK_DETOUR_GET_ORIG( trampoline ) \
    ( (decltype( &trampoline ))_##trampoline##Orig )

#define HOOK_EXPORT_DECLARE( trampoline )                         \
    static std::unique_ptr<PLH::EatHook> _##trampoline = nullptr; \
    static uint64_t _##trampoline##Orig = NULL;

#define HOOK_EXPORT( lib, func, trampoline )                  \
    _##trampoline = std::make_unique<PLH::EatHook>(           \
        lib, func, (char*)trampoline, &_##trampoline##Orig ); \
    _##trampoline->hook();

#define HOOK_EXPORT_GET_ORIG( trampoline ) \
    ( (decltype( &trampoline ))_##trampoline##Orig )
