//
// Created by plaka on 23.02.2023.
//

#ifndef LIVERUSSIA_CPHYSICALGTA_H
#define LIVERUSSIA_CPHYSICALGTA_H

#include "game/Entity/CEntityGta.h"

#pragma pack(1)
struct CPhysicalGta : CEntityGta
{
    float m_fPrevDistFromCam;
    uint32_t m_LastCollisionTime;
    union
    {
        struct
        {
            unsigned int bMakeMassTwiceAsBig : 1;
            unsigned int bApplyGravity : 1;
            unsigned int bDisableCollisionForce : 1;
            unsigned int bCollidable : 1;
            unsigned int bDisableTurnForce : 1;
            unsigned int bDisableMoveForce : 1;
            unsigned int bInfiniteMass : 1;
            unsigned int bDisableZ : 1;

            unsigned int bSubmergedInWater : 1;
            unsigned int bOnSolidSurface : 1;
            unsigned int bBroken : 1;
            unsigned int bProcessCollisionEvenIfStationary : 1; // ref @ 0x6F5CF0
            unsigned int b13 : 1; // only used for peds
            unsigned int bDontApplySpeed : 1;
            unsigned int b15 : 1;
            unsigned int bProcessingShift : 1;

            unsigned int b17 : 1;
            unsigned int bDisableSimpleCollision : 1; // ref @ CPhysical::ProcessCollision
            unsigned int bBulletProof : 1;
            unsigned int bFireProof : 1;
            unsigned int bCollisionProof : 1;
            unsigned int bMeeleProof : 1;
            unsigned int bInvulnerable : 1;
            unsigned int bExplosionProof : 1;

            unsigned int bDontCollideWithFlyers : 1;
            unsigned int bAttachedToEntity : 1;
            unsigned int bAddMovingCollisionSpeed : 1;
            unsigned int bTouchingWater : 1;
            unsigned int bCanBeCollidedWith : 1;
            unsigned int bDestroyed : 1;
            unsigned int b31 : 1;
            unsigned int b32 : 1;
        } physicalFlags;
        unsigned int m_nPhysicalFlags;
    };
    CVector vecMoveSpeed;
    CVector m_vecTurnSpeed;
    CVector m_vecMoveFriction;
    CVector m_vecTurnFriction;
    CVector m_vecAverageMoveSpeed;
    CVector m_vecAverageTurnSpeed;
    float m_fMass;
    float m_fTurnMass;
    float m_fMassMultiplier;
    float m_fAirResistance;
    float m_fElasticity;
    float m_fBuoyancyConstant;
    CVector m_vecCOM; // center of mas
    int m_listEntryInfo;
    uintptr_t m_pMovingListNode;
    uint8_t m_nNoOfStaticFrames;
    uint8_t m_nNoOfCollisionRecords;
    uint8_t m_LastMaterialToHaveBeenStandingOn;
    char field_BB;
    CEntityGta        *m_apCollidedEntities[6];
    float m_fTrueDistanceTravelled;
    float m_fDamageImpulseMagnitude;
    CEntityGta *m_pDamageEntity;
    CVector m_vecDamageNormal;
    CVector m_vecDamagePos;
    uint16_t m_nPieceType;
    uint16_t field_FA;
    CEntityGta *m_pAttachToEntity;
    CVector m_vecAttachPosnOffset;
    CVector m_vecAttachTiltOffset;
    CQuaternion m_AttachQuat;
    CEntityGta *m_pNOCollisionVehicle;
    float m_lightingFromCollision;
    float m_lightingFromPointLights;
    uintptr_t *m_pRealTimeShadow;
};
static_assert(sizeof(CPhysicalGta) == 0x13c, "Invalid");
#endif LIVERUSSIA_CPHYSICALGTA_H
