//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_COMMONSIZE_H
#define LIVERUSSIA_COMMONSIZE_H


#include "HeapFreeBlockDesc.h"

class CommonSize {
public:
    HeapFreeBlockDesc m_ListStart;
    HeapFreeBlockDesc m_ListEnd;

    int32_t             m_BlockSize;

    int32_t             m_NumFails;
    int32_t             m_NumFreeBlocks;

public:
    void Init(int32_t blockSize);
    HeapFreeBlockDesc* Malloc();
    void Free(HeapFreeBlockDesc* desc);

    void _DumpInfo();
};

#endif //LIVERUSSIA_COMMONSIZE_H
