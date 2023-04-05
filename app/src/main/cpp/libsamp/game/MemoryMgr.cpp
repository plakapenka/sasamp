//
// Created by plaka on 05.04.2023.
//

#include "MemoryMgr.h"

void CMemoryMgr::Free(void* memory) {
#ifdef MEMORY_MGR_USE_MEMORY_HEAP
    LOCK_MEMORYHEAP();
    RegisterFree(memory);
    if (g_Heaps[HEAP_SCRATCH]._IsHeapPointer(memory)) {
        g_Heaps[HEAP_SCRATCH].Free(memory);
    } else if (g_Heaps[HEAP_SECONDARY]._IsHeapPointer(memory)) {
        g_Heaps[HEAP_SECONDARY].Free(memory);
    } else {
        g_Heaps[HEAP_PRIMARY].Free(memory);
    }
    UNLOCK_MEMORYHEAP();
#else
   // return plugin::Call<0x72F430, void*>(memory);
    //return ::free(memory);
#endif
}