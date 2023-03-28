//
// Created by plaka on 21.02.2023.
//

#include "rpworld.h"
#include "../../main.h"
#include "rwlpcore.h"
#include "rtanim.h"

RpLight* RpLightSetColor(RpLight* light, const RwRGBAReal* color) {
    return ((RpLight*(__cdecl *)(RpLight*, const RwRGBAReal*))(g_libGTASA + 0x00216746 + 1))(light, color);
}

RpLight* RpLightCreate(RwInt32 type) {
    return ((RpLight*(__cdecl *)(RwInt32))(g_libGTASA + 0x00216DB0 + 1))(type);
}

RpWorld* RpWorldRemoveLight(RpWorld* world, RpLight* light) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpLight*))(g_libGTASA + 0x0021E7F4 + 1))(world, light);
}

RpWorld* RpWorldAddLight(RpWorld* world, RpLight* light) {
    return ((RpWorld*(__cdecl *)(RpWorld*, RpLight*))(g_libGTASA + 0x0021E7B0 + 1))(world, light);
}

RwBool RpLightDestroy(RpLight* light) {
    return ((RwBool(__cdecl *)(RpLight*))(g_libGTASA + 0x00216EF4))(light);
}