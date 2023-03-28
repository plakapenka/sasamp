//
// Created by plaka on 28.03.2023.
//

#include "rwtexdict.h"
#include "../../main.h"

RwTexture* GetFirstTexture(RwTexDictionary* txd) {
    return ((RwTexture*(__cdecl *)(RwTexDictionary*))(g_libGTASA + 0x005D0E1C + 1))(txd);
}

RwTexture* GetFirstTextureCallback(RwTexture* texture, void* data) {
    return ((RwTexture*(__cdecl *)(RwTexture*, void*))(g_libGTASA + 0x005D0E14 + 1))(texture, data);
}

RwTexture* RwTextureGtaStreamRead(RwStream* stream) {
    return ((RwTexture*(__cdecl *)(RwStream*))(g_libGTASA + 0x005D341C + 1))(stream);
}

RwTexDictionary* RwTexDictionaryGtaStreamRead(RwStream* stream) {
    return ((RwTexDictionary*(__cdecl *)(RwStream*))(g_libGTASA + 0x005D3514 + 1))(stream);
}

RwTexDictionary* RwTexDictionaryGtaStreamRead1(RwStream* stream) {
    return ((RwTexDictionary*(__cdecl *)(RwStream*))(g_libGTASA + 0x005D358C + 1))(stream);
}

RwTexDictionary* RwTexDictionaryGtaStreamRead2(RwStream* stream, RwTexDictionary* txd) {
    return ((RwTexDictionary*(__cdecl *)(RwStream*, RwTexDictionary*))(g_libGTASA + 0x005D3644 + 1))(stream, txd);
}

bool RpClumpGtaStreamRead1(RwStream* stream) {
    return ((bool(__cdecl *)(RwStream*))(g_libGTASA + 0x005D07EC + 1))(stream);
}

RpClump* RpClumpGtaStreamRead2(RwStream* stream) {
    return ((RpClump*(__cdecl *)(RwStream*))(g_libGTASA + 0x005D094C + 1))(stream);
}