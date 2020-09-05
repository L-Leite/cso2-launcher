#pragma once

#include <cstddef>

namespace Scaleform
{
class AllocInfo;
class LimitHandler;
class HeapDesc;
class SysAllocPaged;

class MemoryHeap
{
public:
    virtual ~MemoryHeap() {}

    virtual void CreateArena( size_t arena, SysAllocPaged* sysAlloc ) = 0;
    virtual void DestroyArena( size_t arena ) = 0;
    virtual bool ArenaIsEmpty( size_t arena ) = 0;

    virtual MemoryHeap* CreateHeap( const char* name,
                                    const HeapDesc& desc ) = 0;

    virtual void SetLimitHandler( LimitHandler* handler ) = 0;
    virtual void SetLimit( size_t newLimit ) = 0;

    virtual void AddRef() = 0;

    virtual void Release() = 0;

    virtual void* Alloc( size_t size, const AllocInfo* info = 0 ) = 0;

    template <class T>
    T* Alloc( const AllocInfo* info = 0 )
    {
        return reinterpret_cast<T*>( this->Alloc( sizeof( T ), info ) );
    }

    virtual void* Alloc( size_t size, size_t align,
                         const AllocInfo* info = 0 ) = 0;

    virtual void* Realloc( void* oldPtr, size_t newSize ) = 0;
    virtual void Free( void* ptr ) = 0;
};

}  // namespace Scaleform
