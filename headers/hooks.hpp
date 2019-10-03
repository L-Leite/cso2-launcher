#pragma once

#include <memory>

#include <headers/CapstoneDisassembler.hpp>
#include <headers/Detour/x86Detour.hpp>
#include <headers/PE/EatHook.hpp>
#include <headers/Virtuals/VTableSwapHook.hpp>

inline auto SetupDetourHook( const uintptr_t fnAddress, const void* fnCallback,
                             uint64_t* userTrampVar, PLH::ADisassembler& dis )
{
    return std::make_unique<PLH::x86Detour>(
        fnAddress, reinterpret_cast<const uint64_t>( fnCallback ), userTrampVar,
        dis );
}

inline auto SetupExportHook( const std::string& apiName,
                             const std::wstring& moduleName,
                             const void* fnCallback, uint64_t* userOrigVar )
{
    return std::make_unique<PLH::EatHook>(
        apiName, moduleName, reinterpret_cast<const uint64_t>( fnCallback ),
        userOrigVar );
}

inline auto SetupVtableSwap( const void* Class,
                             const PLH::VFuncMap& redirectMap )
{
    return std::make_unique<PLH::VTableSwapHook>(
        reinterpret_cast<const uint64_t>( Class ), redirectMap );
}
