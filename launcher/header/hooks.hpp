#pragma once
#include <memory>

#include <headers/CapstoneDisassembler.hpp>
#include <headers/Detour/x86Detour.hpp>
#include <headers/PE/EatHook.hpp>
#include "modulelist.hpp"
#include "onloadlib.hpp"

#define HOOK_DETOUR_DECLARE( trampoline )                           \
    static std::unique_ptr<PLH::x86Detour> _##trampoline = nullptr; \
    static uint64_t _##trampoline##Orig = NULL

#define HOOK_DETOUR( original, trampoline )                            \
    PLH::CapstoneDisassembler _##trampoline##Dis( PLH::Mode::x86 );    \
    _##trampoline = std::make_unique<PLH::x86Detour>(                  \
        (char*)( original ), (char*)&trampoline, &_##trampoline##Orig, \
        _##trampoline##Dis );                                          \
    _##trampoline->hook();

#define HOOK_DETOUR_MODULE( mod, original, trampoline )                       \
    PLH::CapstoneDisassembler _##trampoline##Dis( PLH::Mode::x86 );           \
    _##trampoline = std::make_unique<PLH::x86Detour>(                         \
        (char*)( g_ModuleList.Get( mod ) + ( original ) ), (char*)trampoline, \
        &_##trampoline##Orig, _##trampoline##Dis );                           \
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

template <size_t iDataSize>
inline void WriteProtectedMemory( const std::array<uint8_t, iDataSize>& data,
                                  uintptr_t pDestination )
{
    PLH::MemoryProtector mp( pDestination, iDataSize,
                             PLH::R | PLH::W | PLH::X );
    std::copy( data.begin(), data.end(),
               reinterpret_cast<uint8_t*>( pDestination ) );
}
