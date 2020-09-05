#pragma once

#include <cstdint>

class CSO2GfxFile
{
public:
    void** vtable;
    uint32_t unk;
    char* m_szName;
    void* m_pHandle;

private:
    CSO2GfxFile() = delete;
};