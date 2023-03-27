//
// Created by plaka on 23.03.2023.
//

#include "rwcore.h"
#include "../../main.h"

void* RwIm3DTransform(RwIm3DVertex* pVerts, RwUInt32 numVerts, RwMatrix* ltm, RwUInt32 flags) {
    return ((void*(__cdecl *)(RwIm3DVertex*, RwUInt32, RwMatrix*, RwUInt32))(g_libGTASA + 0x001DCFAC + 1))(pVerts, numVerts, ltm, flags);
}
RwBool RwIm3DRenderLine(RwInt32 vert1, RwInt32 vert2) {
    return ((RwBool(__cdecl *)(RwInt32, RwInt32))(g_libGTASA + 0x001DD3B4 + 1))(vert1, vert2);
}

RwBool RwIm3DEnd() {
    return ((RwBool(__cdecl *)(void))(g_libGTASA + 0x001DD03C + 1))();
}