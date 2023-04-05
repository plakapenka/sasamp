//
// Created by plaka on 21.02.2023.
//

#ifndef LIVERUSSIA_BASEMODELINFO_H
#define LIVERUSSIA_BASEMODELINFO_H


#include <cstdint>
#include <cassert>
#include "main.h"
#include "game/Collision/ColModel.h"

enum ModelInfoType : uint8_t {
    MODEL_INFO_ATOMIC = 1,
    MODEL_INFO_TIME = 3,
    MODEL_INFO_WEAPON = 4,
    MODEL_INFO_CLUMP = 5,
    MODEL_INFO_VEHICLE = 6,
    MODEL_INFO_PED = 7,
    MODEL_INFO_LOD = 8
};

struct CBaseModelInfo {
    uintptr_t 	vtable;
    uint32_t m_nKey;
    char m_modelName[21];
    char skip_1;
    uint16_t m_nRefCount;
    int16_t m_nTxdIndex;
    uint8_t m_nAlpha;
    uint8_t m_n2dfxCount;
    int16_t m_n2dEffectIndex;
    int16_t m_nObjectInfoIndex;
    union {
        unsigned short m_nFlags;
        struct {
            unsigned char m_nFlagsUpperByte;
            unsigned char m_nFlagsLowerByte;
        };
        struct {
            /* https://github.com/multitheftauto/mtasa-blue/blob/master/Client/game_sa/CModelInfoSA.h */
            unsigned char bHasBeenPreRendered: 1; // we use this because we need to apply changes only once
            unsigned char bDrawLast: 1;
            unsigned char bAdditiveRender: 1;
            unsigned char bDontWriteZBuffer: 1;
            unsigned char bDontCastShadowsOn: 1;
            unsigned char bDoWeOwnTheColModel: 1;
            unsigned char bIsBackfaceCulled: 1;
            unsigned char bIsLod: 1;

            union {
                struct { // Atomic flags
                    unsigned char bIsRoad: 1;
                    unsigned char : 1;
                    unsigned char bDontCollideWithFlyer: 1;
                    unsigned char nSpecialType: 4;
                    unsigned char bWetRoadReflection: 1;
                };
                struct { // Vehicle flags
                    unsigned char bUsesVehDummy: 1;
                    unsigned char : 1;
                    unsigned char nCarmodId: 5;
                    unsigned char bUseCommonVehicleDictionary: 1;
                };
                struct { // Clump flags
                    unsigned char bHasAnimBlend: 1;
                    unsigned char bHasComplexHierarchy: 1;
                    unsigned char bAnimSomething: 1;
                    unsigned char bOwnsCollisionModel: 1;
                    unsigned char : 3;
                    unsigned char bTagDisabled: 1;
                };
            };
        };
    };
    uint8_t ski_2[2];
    CColModel *m_pColModel;      // 20
    float m_fDrawDistance;  // 24
    union {
        struct RwObject *m_pRwObject;
        struct RpClump *m_pRwClump;
        struct RpAtomic *m_pRwAtomic;
    };

};
static_assert(sizeof(CBaseModelInfo) == 0x38, "Invalid");
// sizeof=0x38


#endif //LIVERUSSIA_BASEMODELINFO_H
