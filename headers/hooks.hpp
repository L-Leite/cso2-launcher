#pragma once

#include <memory>

#include <polyhook2/Detour/x86Detour.hpp>
#include <polyhook2/PE/EatHook.hpp>
#include <polyhook2/Virtuals/VFuncSwapHook.hpp>
#include <polyhook2/Virtuals/VTableSwapHook.hpp>
#include <polyhook2/ZydisDisassembler.hpp>

#ifndef NOINLINE
#define NOINLINE __declspec( noinline )
#endif

inline auto HookDisassembler()
{
    PLH::ZydisDisassembler dis( PLH::Mode::x86 );
    return dis;
}

template <typename DetourDestType>
inline auto SetupDetourHook( const uintptr_t fnAddress,
                             DetourDestType* fnCallback, uint64_t* userTrampVar,
                             PLH::ADisassembler& dis )
{
    return std::make_unique<PLH::x86Detour>(
        fnAddress, reinterpret_cast<const uint64_t>( fnCallback ), userTrampVar,
        dis );
}

template <typename DetourDestType>
inline auto SetupExportHook( const std::string& apiName,
                             const std::wstring& moduleName,
                             DetourDestType* fnCallback, uint64_t* userOrigVar )
{
    return std::make_unique<PLH::EatHook>(
        apiName, moduleName, reinterpret_cast<const uint64_t>( fnCallback ),
        userOrigVar );
}

template <typename VtableType>
inline auto SetupVtableSwap( VtableType* Class,
                             const PLH::VFuncMap& redirectMap )
{
    return std::make_unique<PLH::VTableSwapHook>(
        reinterpret_cast<const uint64_t>( Class ), redirectMap );
}

inline auto SetupVfuncSwap( void* vtableAddr, const PLH::VFuncMap& redirectMap,
                            PLH::VFuncMap* origVFuncs )
{
    return std::make_unique<PLH::VFuncSwapHook>(
        reinterpret_cast<const uint64_t>( vtableAddr ), redirectMap,
        origVFuncs );
}
