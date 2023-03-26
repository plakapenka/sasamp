//
// Created by plaka on 23.02.2023.
//

#ifndef LIVERUSSIA_CENTITYGTA_H
#define LIVERUSSIA_CENTITYGTA_H

#include "CPlaceableGta.h"

#pragma pack(1)
struct CEntityGta : CPlaceableGta
{
    union {
        struct RwObject* m_pRwObject;
        struct RpClump* m_pRpClump;
        struct RpAtomic* m_pRpAtomic;
        uintptr_t pdwRenderWare;    // 24-28	;rwObject
    };

    union {
        uint32_t m_nEntityFlags;
        struct {
            uint32_t m_bUsesCollision : 1;
            uint32_t m_bCollisionProcessed : 1;
            uint32_t m_bIsStatic : 1;
            uint32_t m_bHasContacted : 1;
            uint32_t m_bIsStuck : 1;
            uint32_t m_bIsInSafePosition : 1;
            uint32_t m_bWasPostponed : 1;
            uint32_t m_bIsVisible : 1;

            uint32_t m_bIsBIGBuilding : 1;
            uint32_t m_bRenderDamaged : 1;
            uint32_t m_bStreamingDontDelete : 1;
            uint32_t m_bRemoveFromWorld : 1;
            uint32_t m_bHasHitWall : 1;
            uint32_t m_bImBeingRendered : 1;
            uint32_t m_bDrawLast :1;
            uint32_t m_bDistanceFade : 1;

            uint32_t m_bDontCastShadowsOn : 1;
            uint32_t m_bOffscreen : 1;
            uint32_t m_bIsStaticWaitingForCollision : 1;
            uint32_t m_bDontStream : 1;
            uint32_t m_bUnderwater : 1;
            uint32_t m_bHasPreRenderEffects : 1;
            uint32_t m_bIsTempBuilding : 1;
            uint32_t m_bDontUpdateHierarchy : 1;

            uint32_t m_bHasRoadsignText : 1;
            uint32_t m_bDisplayedSuperLowLOD : 1;
            uint32_t m_bIsProcObject : 1;
            uint32_t m_bBackfaceCulled : 1;
            uint32_t m_bLightObject : 1;
            uint32_t m_bUnimportantStream : 1;
            uint32_t m_bTunnel : 1;
            uint32_t m_bTunnelTransition : 1;
        } nEntityFlags;
    };
    uint32_t fuck;
    uint16_t RandomSeed;
    uint16_t nModelIndex;
    uintptr_t pReferences;
    uintptr_t m_pLastRenderedLink;
    uint16_t m_nScanCode;
    uint8_t m_iplIndex;
    uint8_t m_areaCode;
    uintptr_t m_pLod;
    uint8_t numLodChildren;
    uint8_t numLodChildrenRendered;
    uint8_t m_nType;
    uint8_t m_nStatus;
};

#endif //LIVERUSSIA_CENTITYGTA_H
