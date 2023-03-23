//
// Created by plaka on 23.03.2023.
//

#include "Lines.h"
#include "game/RW/RenderWare.h"
#include "main.h"


void CLines::RenderLineNoClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32_t startColor, uint32_t endColor) {
    RxObjSpace3DVertex vertices[] = {
            { .objVertex = { startX, startY, startZ }, .color = startColor >> 8 | startColor << 24 },
            { .objVertex = { endX,   endY,   endZ   }, .color =   endColor >> 8 | endColor   << 24 }
    };

    //LittleTest();
    if (RwIm3DTransform(vertices, 2u, nullptr, 0)) {
        RwIm3DRenderLine(0, 1);
        RwIm3DEnd();
    }
}

// 0x6FF4F0
void CLines::RenderLineWithClipping(float startX, float startY, float startZ, float endX, float endY, float endZ, uint32_t startColor, uint32_t endColor) {
   // return plugin::Call<0x6FF4F0, float, float, float, float, float, float, uint32, uint32>(startX, startY, startZ, endX, endY, endZ, startColor, endColor);

    return ((void(__cdecl *)(float, float, float, float, float, float, uint32_t, uint32_t))(g_libGTASA + 0x005ADBD8 + 1))(startX, startY, startZ, endX, endY, endZ, startColor, endColor);

//    const CVector start = { startX, startY, startZ };
//    const CVector end   = { endX,   endY,   endZ };
//
//    auto iters = DistanceBetweenPoints(start, end) * 0.4f + 1.0f;
//    auto numVerts = (uint32_t)std::min(iters, 7.0f);
//    if (numVerts > 0) {
//        // todo:
//        for (auto i = 0u; i < numVerts; i++) {
//            // todo:
//        }
//    }
//
//    static RwImVertexIndex indices[] = { // 0x8D503C
//            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11,
//            12, 13, 14, 15, 16, 17, 18, 19, 20, 0
//    };
//    LittleTest();
//    if (RwIm3DTransform(aTempBufferVertices, 2 * numVerts, nullptr, 0)) {
//        RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indices, 2 * numVerts);
//        RwIm3DEnd();
//    }
}

// 0x6FF790
void CLines::ImmediateLine2D(int32_t startX, int32_t startY, int32_t endX, int32_t endY, uint8_t startR, uint8_t startG, uint8_t startB, uint8_t startA, uint8_t endR, uint8_t endG, uint8_t endB, uint8_t endA) {
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(NULL));

    RwIm2DVertex vertices[] = {
            { .x = float(startX), .y = float(startY), .emissiveColor = CRGBA(startR, startG, startB, startA).ToIntARGB() },
            { .x = float(endX),   .y = float(endY),   .emissiveColor = CRGBA(endR, endG, endB, endA).ToIntARGB() }
    };
    RwIm2DRenderLine(vertices, 2, 0, 1);

    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
}
