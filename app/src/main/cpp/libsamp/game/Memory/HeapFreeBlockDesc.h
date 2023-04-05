//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_HEAPFREEBLOCKDESC_H
#define LIVERUSSIA_HEAPFREEBLOCKDESC_H


#include "HeapBlockDesc.h"

class HeapFreeBlockDesc : public HeapBlockDesc {
public:
    HeapFreeBlockDesc* m_Next;
    HeapFreeBlockDesc* m_Prev;

public:
    HeapFreeBlockDesc();

    HeapFreeBlockDesc* FindSmallestFreeBlock(int32_t size);
    void InsertOrderedHeapFreeBlock(HeapFreeBlockDesc* desc);
    void InsertHeapFreeBlock(HeapFreeBlockDesc* desc);
    void ReplaceHeapFreeBlock(HeapFreeBlockDesc* desc);
    void RemoveHeapFreeBlock();
};

static_assert(sizeof(HeapFreeBlockDesc) == 0x18, "Invalid");
#endif //LIVERUSSIA_HEAPFREEBLOCKDESC_H
