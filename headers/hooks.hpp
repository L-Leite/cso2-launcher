#pragma once

#include <memory>

#include <headers/CapstoneDisassembler.hpp>
#include <headers/Detour/x86Detour.hpp>
#include <headers/PE/EatHook.hpp>
#include <headers/Virtuals/VTableSwapHook.hpp>

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
