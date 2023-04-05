//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_HEAPBLOCKDESC_H
#define LIVERUSSIA_HEAPBLOCKDESC_H


#include <cstdint>

class HeapBlockDesc {
public:
    size_t m_nSize;
    bool  m_bInUse;

    uint8_t pad;

    int16_t m_nMemId; // HeapID ?
    union {
        void*        m_pDebugInfo;   // UIntPtr
        uint8_t const* m_upDebugInfo;  // (can be useful, lol)
    };
    HeapBlockDesc* m_PrevBlock; // prev located block ?

public:
    HeapBlockDesc();

    void InsertHeapBlock(HeapBlockDesc* desc);

    HeapBlockDesc* _GetNextLocatedBlock() const;
    void* _GetBlockData() const;
    void _DumpBlockInfo() const;
};

static_assert(sizeof(HeapBlockDesc) == 0x10, "Invalid");


#endif //LIVERUSSIA_HEAPBLOCKDESC_H
