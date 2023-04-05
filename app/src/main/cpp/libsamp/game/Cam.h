//
// Created by plaka on 28.03.2023.
//

#ifndef LIVERUSSIA_CAM_H
#define LIVERUSSIA_CAM_H


#include "CVehicleGta.h"
#include "Core/Vector.h"
#include "Enums/eCamMode.h"

struct CCam {
public:
    uint8_t skip[0x210];
//    bool m_bBelowMinDist;
//    bool m_bBehindPlayerDesired;
//    bool m_bCamLookingAtVector;
//    bool m_bCollisionChecksOn;
//    bool m_bFixingBeta;
//    bool m_bTheHeightFixerVehicleIsATrain;
//    bool m_bLookBehindCamWasInFront;
//    bool m_bLookingBehind;
//    bool m_bLookingLeft;
//    bool m_bLookingRight;
//    bool m_bResetStatics;
//    bool m_bResetKeyboardStatics;
//    bool m_bRotating;
//    eCamMode m_nMode;
//    uint32_t m_nFinishTime;
//    uint32_t m_nDoCollisionChecksOnFrameNum;
//    uint32_t m_nDoCollisionCheckEveryNumOfFrames;
//    uint32_t m_nFrameNumWereAt;
//    uint32_t m_nRunningVectorArrayPos;
//    uint32_t m_nRunningVectorCounter;
//    uint32_t m_nDirectionWasLooking;
//    float m_fMaxRoleAngle;
//    float m_fRoll;
//    float m_fRollSpeed;
//    float m_fSyphonModeTargetZOffSet;
//    float m_fAmountFractionObscured;
//    float m_fAlphaSpeedOverOneFrame;
//    float m_fBetaSpeedOverOneFrame;
//    float m_fBufferedTargetBeta;
//    float m_fBufferedTargetOrientation;
//    float m_fBufferedTargetOrientationSpeed;
//    float m_fCamBufferedHeight;
//    float m_fCamBufferedHeightSpeed;
//    float m_fCloseInPedHeightOffset;
//    float m_fCloseInPedHeightOffsetSpeed;
//    float m_fCloseInCarHeightOffset;
//    float m_fCloseInCarHeightOffsetSpeed;
//    float m_fDimensionOfHighestNearCar;
//    float m_fDistanceBeforeChanges;
//    float m_fFovSpeedOverOneFrame;
//    float m_fMinDistAwayFromCamWhenInterPolating;
//    float m_fPedBetweenCameraHeightOffset;
//    float m_fPlayerInFrontSyphonAngleOffSet;
//    float m_fRadiusForDead;
//    float m_fRealGroundDist;
//    float m_fTargetBeta;
//    float m_fTimeElapsedFloat;
//    float m_fTilt;
//    float m_fTiltSpeed;
//    float m_fTransitionBeta;
//    float m_fTrueBeta;
//    float m_fTrueAlpha;
//    float m_fInitialPlayerOrientation;
//    float m_fVerticalAngle;  ///< The radian angle ([-pi/2, pi/2]) relative to the entity the camera is locked on (the player usually)
//    float m_fAlphaSpeed;
//    float m_fFOV;
//    float m_fFOVSpeed;
//    float m_fHorizontalAngle;
//    float m_fBetaSpeed;
//    float m_fDistance;
//    float m_fDistanceSpeed;
//    float m_fCaMinDistance;
//    float m_fCaMaxDistance;
//    float m_fSpeedVar;
//    float m_fCameraHeightMultiplier;
//    float m_fTargetZoomGroundOne;
//    float m_fTargetZoomGroundTwo;
//    float m_fTargetZoomGroundThree;
//    float m_fTargetZoomOneZExtra;
//    float m_fTargetZoomTwoZExtra;
//    float m_fTargetZoomTwoInteriorZExtra;
//    float m_fTargetZoomThreeZExtra;
//    float m_fTargetZoomZCloseIn;
//    float m_fMinRealGroundDist;
//    float m_fTargetCloseInDist;
//    float m_fBeta_Targeting;
//    float m_fX_Targetting;
//    float m_fY_Targetting;
//    CVehicleGta *m_pCarWeAreFocussingOn;
//    CVehicleGta *m_pCarWeAreFocussingOnI;
//    float m_fCamBumpedHorz;
//    float m_fCamBumpedVert;
//    uint32_t m_nCamBumpedTime;
//    CVector m_vecSourceSpeedOverOneFrame;
//    CVector m_vecTargetSpeedOverOneFrame;
//    CVector m_vecUpOverOneFrame;
//    CVector m_vecTargetCoorsForFudgeInter;
//    CVector m_vecCamFixedModeVector;
//    CVector m_vecCamFixedModeSource;
//    CVector m_vecCamFixedModeUpOffSet;
//    CVector m_vecLastAboveWaterCamPosition;
//    CVector m_vecBufferedPlayerBodyOffset;
//    CVector m_vecFront;
//    CVector m_vecSource;
//    CVector m_vecSourceBeforeLookBehind;
//    CVector m_vecUp;
//    CVector m_avecPreviousVectors[2];
//    CVector m_avecTargetHistoryPos[4];
//    uint32_t m_anTargetHistoryTime[4];
//    uint32_t m_nCurrentHistoryPoints;
//    CEntityGta *m_pCamTargetEntity;
//    float m_fCameraDistance;
//    float m_fIdealAlpha;
//    float m_fPlayerVelocity;
//    CVehicleGta *m_pLastCarEntered;
//    CPedGta *m_pLastPedLookedAt;
//    bool m_bFirstPersonRunAboutActive;
};
static_assert(sizeof(CCam) == 0x210, "Invalid");

#endif //LIVERUSSIA_CAM_H
