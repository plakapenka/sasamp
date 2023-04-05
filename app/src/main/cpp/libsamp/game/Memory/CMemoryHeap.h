//
// Created by plaka on 05.04.2023.
//

#ifndef LIVERUSSIA_CMEMORYHEAP_H
#define LIVERUSSIA_CMEMORYHEAP_H

#include "HeapFreeBlockDesc.h"
#include "CommonSize.h"

#define MEMORY_HEAP_ALIGN 16
#define MEMORY_HEAP_CSIZES 64

#ifdef MoveMemory
#undef MoveMemory
#endif

/**
 * Memory heap definition
 * Used for internal game memory handling on consoles
 */
class CMemoryHeap {
protected:
    HeapBlockDesc*    m_FirstBlock;    // first block
    HeapBlockDesc*    m_LastBlock;     // last block (size=0 used=1)

    HeapFreeBlockDesc m_FreeListBegin; // free blocks list (CommonSize start?)
    HeapFreeBlockDesc m_FreeListEnd;

    CommonSize* m_paCommonSizes;
    bool        m_bMovingRestricted;

    uint32_t      m_nMemUsed;
    int32_t       m_nMemId;

public:
    /**
     * Just a constructor. Call { @link CMemoryHeap::Init } for heap initialization.
     */
    CMemoryHeap();

    /**
     * Initialize memory heap class instance
     *
     * @param mem        memory, allocated for this memory heap
     * @param size       size of allocated memory
     * @param bInitSizes initialize CommonSizes for small memory blocks
     */
    void Init(void* mem, uint32_t size, bool bInitSizes);

    /**
     * Shutdown heap instance
     * NOTE: free() will be called for memory passed to { @link CMemoryHeap::Init } function
     */
    void Shutdown();

    /**
     * Debug stuff
     */

    void IntegrityCheck();
    void PrintMemoryLeaks();
    void ParseHeap();

    static void* GetDebugInfo(void* mem);
    static void  SetDebugInfo(void* mem, void* nDebugInfo);

    uint32_t GetSizeOfHoles();
    void FillInBlockData(HeapBlockDesc* desc, HeapBlockDesc* a2, uint32_t a3);

    void Free(void* mem);
    void* Malloc(uint32_t size);
    void* Realloc(void* mem, uint32_t nSize);

    HeapFreeBlockDesc* _JoinFreeBlocks(HeapFreeBlockDesc* desc);
    bool IsHeapPointer(void* mem) { return mem >= m_FirstBlock && mem < m_LastBlock; }
    uint32_t GetLargestFreeBlock() { return m_FreeListEnd.m_Prev->m_nSize; } // 0x72F370
    uint32_t GetHeapSize() { return (uint8_t*)m_LastBlock - (uint8_t*)m_FirstBlock + sizeof(HeapBlockDesc); }

protected:
    void RegisterFree(HeapFreeBlockDesc* desc);
    void RegisterMalloc(HeapBlockDesc* desc);
    void* MoveHeapBlock(HeapFreeBlockDesc* free, HeapBlockDesc* used);

    HeapFreeBlockDesc* WhereShouldMemoryMove(void* pMemory);
    void* MoveMemory(void* mem);
    void* MoveMemory(void* mem, void**, int32_t iUnk);

public:
    void _DumpHeapStatus(bool bCommonSizes);
    HeapBlockDesc* _FindBlockDesc(void* mem);

    friend class CMemoryMgr;
};



#endif //LIVERUSSIA_CMEMORYHEAP_H
