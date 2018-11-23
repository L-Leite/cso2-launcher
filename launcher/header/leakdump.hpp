#pragma once

#include "tier0/memalloc.h"

#ifdef _WIN32
#pragma warning( disable : 4073 )
#pragma init_seg( lib )
#endif

class CLeakDump
{
public:
    CLeakDump() : m_bCheckLeaks( false ) {}

    ~CLeakDump()
    {
        if ( m_bCheckLeaks )
        {
#ifndef NO_MALLOC_OVERRIDE
            g_pMemAlloc->DumpStats();
#endif
        }
    }

    bool m_bCheckLeaks;
};
