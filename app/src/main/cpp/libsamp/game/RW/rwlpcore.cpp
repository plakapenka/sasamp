//
// Created by plaka on 23.03.2023.
//

#include "rwlpcore.h"
#include "../../main.h"

RwMatrix* RwMatrixUpdate(RwMatrix* matrix) {
    matrix->flags &= 0xFFFDFFFC;
    return matrix;
}

RwBool RwMatrixDestroy(RwMatrix* mpMat) {
    return ((RwBool(__cdecl *)(RwMatrix*))(g_libGTASA + 0x001E446C + 1))(mpMat);
}

RwBool RwStreamFindChunk(RwStream* stream, RwUInt32 type, RwUInt32* lengthOut, RwUInt32* versionOut) {
    return ((RwBool(__cdecl *)(RwStream*, RwUInt32, RwUInt32*, RwUInt32*))(g_libGTASA + 0x001E1FBA + 1))(stream, type, lengthOut, versionOut);
}

RwStream* RwStreamOpen(RwStreamType type, RwStreamAccessType accessType, const void* data) {
    return ((RwStream*(__cdecl *)(RwStreamType, RwStreamAccessType, const void*))(g_libGTASA + 0x001E59F0 + 1))(type, accessType, data);
}

RwBool RwStreamClose(RwStream* stream, void* data) {
    return ((RwBool(__cdecl *)(RwStream*, void*))(g_libGTASA + 0x001E5958 + 1))(stream, data);
}