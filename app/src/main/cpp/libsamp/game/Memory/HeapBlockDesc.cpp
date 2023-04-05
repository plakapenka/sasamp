//
// Created by plaka on 05.04.2023.
//

#include <cstring>
#include <cstdio>
#include "HeapBlockDesc.h"
#include "HeapFreeBlockDesc.h"

HeapBlockDesc::HeapBlockDesc() {
    m_nSize      = 0;
    m_bInUse     = false;
    m_nMemId     = 0;
    m_pDebugInfo = nullptr;
    m_PrevBlock  = nullptr;

}

void HeapBlockDesc::InsertHeapBlock(HeapBlockDesc* desc) {
    // missing
}

HeapBlockDesc* HeapBlockDesc::_GetNextLocatedBlock() const {
    return (HeapBlockDesc*)((uint8_t*)this + sizeof(HeapBlockDesc) + m_nSize);
}

void* HeapBlockDesc::_GetBlockData() const {
    return (uint8_t*)this + sizeof(HeapBlockDesc);
}

void HeapBlockDesc::_DumpBlockInfo() const {
    auto const* self = reinterpret_cast<HeapFreeBlockDesc const*>(this);

    const char* info = "";

    char szMem[64] = { 0 };
    for (int32_t i = 0; i < 16; ++i) { // todo: magic number (related to m_idStack size?)
        sprintf(szMem + strlen(szMem), "%02x", ((uint8_t*)self->_GetBlockData())[i]);
        if (i + 1 != 16) {
            strcat(szMem, " ");
        }
    }

    // DEV_LOG("  {:6s} [{:03d}]: 0x{:08x} {:11d} bytes   ^ 0x{:08x}   < {} >   {}\n", type, p->m_nMemId & 0xFF, p, p->m_nSize, p->m_PrevBlock, szMem, info);
}