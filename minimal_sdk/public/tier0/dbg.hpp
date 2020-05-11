#pragma once

enum SpewType_t
{
    SPEW_MESSAGE = 0,
    SPEW_WARNING,
    SPEW_ASSERT,
    SPEW_ERROR,
    SPEW_LOG,

    SPEW_TYPE_COUNT
};

enum SpewRetval_t
{
    SPEW_DEBUGGER = 0,
    SPEW_CONTINUE,
    SPEW_ABORT
};

using SpewOutputFunc_t = SpewRetval_t ( * )( SpewType_t spewType,
                                             const char* pMsg );

void SpewOutputFunc( SpewOutputFunc_t func );
const char* GetSpewOutputGroup();
bool ShouldUseNewAssertDialog();