#include "utilities/memorypatterns.hpp"

#include "utilities/PatternSearch.hpp"
#include "utilities/log.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

constexpr const uint8_t MEMORY_PATTTERNS_WILDCARD = '\xCC';

MemoryModuleInfo GetModuleSizeOfCode( std::string_view targetModule )
{
#ifdef _WIN32
    const uintptr_t moduleBase =
        reinterpret_cast<uintptr_t>( GetModuleHandle( targetModule.data() ) );

    if ( moduleBase == 0 )
    {
        return { nullptr, 0 };
    }

    auto pDosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>( moduleBase );
    auto pPEHeader = reinterpret_cast<PIMAGE_NT_HEADERS>(
        moduleBase + pDosHeader->e_lfanew );
    auto pOptionalHeader = &pPEHeader->OptionalHeader;

    return { reinterpret_cast<void*>( moduleBase ),
             pOptionalHeader->SizeOfCode };
#else
#error "Implement GetModuleSizeOfCode for your platform"
#endif
}

std::pair<bool, MemoryModuleInfo> MemoryPatterns::FindModuleInfo(
    std::string_view targetModule )
{
    auto cacheFindRes = this->m_ModuleCache.find( targetModule.data() );

    if ( cacheFindRes != this->m_ModuleCache.end() )
    {
        return { true, cacheFindRes->second };
    }

    auto moduleFound = GetModuleSizeOfCode( targetModule );

    if ( moduleFound.base == nullptr || moduleFound.length == 0 )
    {
        return { false, { 0, 0 } };
    }

    this->m_ModuleCache[targetModule.data()] = moduleFound;
    return { true, moduleFound };
}

bool MemoryPatterns::AddPattern( std::string_view pattern,
                                 std::string_view patternName,
                                 IMemoryPatternsOptions options /*= {}*/ )
{
    const auto [wasModuleFound, moduleInfo] =
        FindModuleInfo( options.targetModule );

    if ( wasModuleFound == false )
    {
        Log::Error( "Failed to get module {} info\n", options.targetModule );
        return false;
    }

    PatternSearch ptrn( pattern.data() );

    std::vector<uintptr_t> lookupResults =
        this->FindPatternInternal( pattern, moduleInfo, options );

    if ( lookupResults.size() == 0 )
    {
        Log::Error( "Unable to find pattern {} in module {}\n", patternName,
                    options.targetModule );
        return false;
    }

    if ( lookupResults.size() > 1 )
    {
        Log::Warning( "Found more than {} results when searching for "
                      "pattern {} in module {}\n",
                      lookupResults.size(), patternName, options.targetModule );
    }

    if ( options.resultIndex >= lookupResults.size() )
    {
        options.resultIndex = lookupResults.size() - 1;
    }

    uintptr_t findRes = lookupResults[options.resultIndex];

    Log::Debug( "Found pattern {} ({}) at {}\n", patternName,
                options.targetModule, reinterpret_cast<void*>( findRes ) );

    this->m_Results[patternName.data()] = findRes;

    return true;
}

std::vector<uintptr_t> MemoryPatterns::FindPatternInternal(
    std::string_view pattern, const MemoryModuleInfo& moduleInfo,
    const IMemoryPatternsOptions& options ) const
{
    PatternSearch ptrn( pattern.data() );

    std::vector<uintptr_t> lookupResults;
    ptrn.Search( MEMORY_PATTTERNS_WILDCARD, moduleInfo.base, moduleInfo.length,
                 lookupResults );

    for ( auto& result : lookupResults )
    {
        uintptr_t baseOffset = 0;

        if ( options.isRelativeJump == true )
        {
            baseOffset = result;
        }

        if ( options.instructionLength != -1 )
        {
            baseOffset += options.instructionLength;
        }

        // from DarthTon's Blackbone

        // Plain pointer sum
        if ( options.functionOffset != -1 )
        {
            result = result - options.functionOffset;
        }
        // Pointer dereference inside instruction
        else if ( options.dataStartOffset != -1 )
        {
            result =
                *reinterpret_cast<int32_t*>( result + options.dataStartOffset );
        }

        result += baseOffset;
    }

    return lookupResults;
}
