#pragma once

#include <cstdint>
#include <string_view>
#include <unordered_map>
#include <utility>

struct IMemoryPatternsOptions
{
    IMemoryPatternsOptions( intptr_t funOffset = -1, intptr_t dataStartOff = -1,
                            intptr_t insLen = -1, std::string_view module = {},
                            size_t resultIdx = 0, bool isRelJmp = false )
        : functionOffset( funOffset ), dataStartOffset( dataStartOff ),
          instructionLength( insLen ), targetModule( module ),
          resultIndex( resultIdx ), isRelativeJump( isRelJmp )
    {
    }

    intptr_t functionOffset;
    intptr_t dataStartOffset;
    intptr_t instructionLength;
    std::string_view targetModule;
    size_t resultIndex;
    bool isRelativeJump;
};

struct MemoryModuleInfo
{
    void* base;
    uintptr_t length;
};

class MemoryPatterns
{
private:
    std::unordered_map<const char*, uintptr_t> m_Results;
    std::unordered_map<const char*, MemoryModuleInfo> m_ModuleCache;

public:
    bool AddPattern( std::string_view pattern, std::string_view patternName,
                     IMemoryPatternsOptions options = {} );

    [[nodiscard]] inline uintptr_t GetPattern( std::string_view patternName )
    {
        return this->m_Results[patternName.data()];
    }

    inline static MemoryPatterns& Singleton()
    {
        static MemoryPatterns instance;
        return instance;
    }

private:
    MemoryPatterns() = default;
    ~MemoryPatterns() = default;

    std::vector<uintptr_t> FindPatternInternal(
        std::string_view pattern, const MemoryModuleInfo& moduleInfo,
        const IMemoryPatternsOptions& options ) const;
    std::pair<bool, MemoryModuleInfo> FindModuleInfo(
        std::string_view targetModule );

    // disable copy assignments
    // (move assignments are disabld implicitally)
private:
    MemoryPatterns( const MemoryPatterns& ) = delete;
    MemoryPatterns& operator=( const MemoryPatterns& ) = delete;
};
