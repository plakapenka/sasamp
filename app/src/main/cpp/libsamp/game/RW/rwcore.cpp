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

RwBool RwTextureSetFindCallBack(RwTextureCallBackFind callBack) {
    return ((RwBool(__cdecl *)(RwTextureCallBackFind))(g_libGTASA + 0x001DB3A4 + 1))(callBack);
}

RwBool RwTextureSetReadCallBack(RwTextureCallBackRead callBack) {
    return ((RwBool(__cdecl *)(RwTextureCallBackRead))(g_libGTASA + 0x001DB3E0 + 1))(callBack);
}

RwTexDictionary* RwTexDictionaryGetCurrent() {
    return ((RwTexDictionary*(__cdecl *)(void))(g_libGTASA + 0x001DB9E4 + 1))();
}

RwTexDictionary* RwTexDictionarySetCurrent(RwTexDictionary* dict) {
    return ((RwTexDictionary*(__cdecl *)(RwTexDictionary*))(g_libGTASA + 0x001DB9C8 + 1))(dict);
}

RwTexDictionary* RwTexDictionaryCreate() {
    return ((RwTexDictionary*(__cdecl *)(void))(g_libGTASA + 0x001DB500 + 1))();
}

RwBool RwTexDictionaryDestroy(RwTexDictionary* dict) {
    return ((RwBool(__cdecl *)(RwTexDictionary*))(g_libGTASA + 0x001DB588 + 1))(dict);
}

RwTexture* RwTexDictionaryRemoveTexture(RwTexture* texture) {
    return ((RwTexture*(__cdecl *)(RwTexture*))(g_libGTASA + 0x001DB916 + 1))(texture);
}

RwTexture* RwTexDictionaryFindNamedTexture(RwTexDictionary* dict, const RwChar* name) {
    return ((RwTexture*(__cdecl *)(RwTexDictionary*, const RwChar*))(g_libGTASA + 0x001DB930 + 1))(dict, name);
}

const RwTexDictionary* RwTexDictionaryForAllTextures(const RwTexDictionary* dict, RwTextureCallBack fpCallBack, void* data) {
    return ((const RwTexDictionary*(__cdecl *)(const RwTexDictionary*, RwTextureCallBack, void*))(g_libGTASA + 0x001DB6B4 + 1))(dict, fpCallBack, data);
}
