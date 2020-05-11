#pragma once

template <class T>
inline T* Construct( T* pMemory )
{
    return ::new ( pMemory ) T;
}

template <class T>
inline T* CopyConstruct( T* pMemory, T const& src )
{
    return ::new ( pMemory ) T( src );
}

template <class T>
inline void Destruct( T* pMemory )
{
    pMemory->~T();
}