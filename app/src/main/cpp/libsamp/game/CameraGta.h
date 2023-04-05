//
// Created by plaka on 24.03.2023.
//

#ifndef LIVERUSSIA_CAMERAGTA_H
#define LIVERUSSIA_CAMERAGTA_H


#include "CPlaceableGta.h"
#include "game/Enums/eCamMode.h"
#include "Cam.h"
#include "QueuedMode.h"
#include "game/RW/rwcore.h"
#include "CamPathSplines.h"
#include "RW/rwcore.h"
#include "Core/Matrix.h"
#include "main.h"

enum class eFadeFlag : uint16_t {
    FADE_IN,
    FADE_OUT
};

enum class eSwitchType : uint16_t {
    NONE,
    INTERPOLATION,
    JUMPCUT
};

struct CVehicleCamTweak {
    int32_t m_nModelIndex;
    float   m_fDistance;
    float   m_fAltitude;
    float   m_fAngle;
};
static_assert(sizeof(CVehicleCamTweak) == 0x10, "Invalid");

struct CCameraGta : CPlaceableGta{
    bool            m_bAboveGroundTrainNodesLoaded{};
    bool            m_bBelowGroundTrainNodesLoaded{};
    bool            m_bCamDirectlyBehind{};
    bool            m_bCamDirectlyInFront{};
    bool            m_bCameraJustRestored{};
    bool            m_bCutsceneFinished{};
    bool            m_bCullZoneChecksOn{};
    bool            m_bIdleOn{};
    bool            m_bInATunnelAndABigVehicle{};
    bool            m_bInitialNodeFound{};
    bool            m_bInitialNoNodeStaticsSet{};
    bool            m_bIgnoreFadingStuffForMusic{};
    bool            m_bPlayerIsInGarage{};
    bool            m_bPlayerWasOnBike{};
    bool            m_bJustCameOutOfGarage{};
    bool            m_bJustInitialized{true};
    bool            m_bJust_Switched{};
    bool            m_bLookingAtPlayer{true};
    bool            m_bLookingAtVector{};
    bool            m_bMoveCamToAvoidGeom{};
    bool            m_bObbeCinematicPedCamOn{};
    bool            m_bObbeCinematicCarCamOn{};
    bool            m_bRestoreByJumpCut{};
    bool            m_bUseNearClipScript{};
    bool            m_bStartInterScript{};
    bool            m_bStartingSpline{};
    bool            m_bTargetJustBeenOnTrain{};
    bool            m_bTargetJustCameOffTrain{};
    bool            m_bUseSpecialFovTrain{};
    bool            m_bUseTransitionBeta{};
    bool            m_bUseScriptZoomValuePed{};
    bool            m_bUseScriptZoomValueCar{};
    bool            m_bWaitForInterpolToFinish{};
    bool            m_bItsOkToLookJustAtThePlayer{};
    bool            m_bWantsToSwitchWidescreenOff{};
    bool            m_bWideScreenOn{};
    bool            m_b1rstPersonRunCloseToAWall{};
    bool            m_bHeadBob{};
    bool            m_bVehicleSuspenHigh{};
    bool            m_bEnable1rstPersonCamCntrlsScript{};
    bool            m_bAllow1rstPersonWeaponsCamera{};
    bool            m_bCooperativeCamMode{};
    bool            m_bAllowShootingWith2PlayersInCar{true};
    bool            m_bDisableFirstPersonInCar{};
    eCamMode        m_nModeForTwoPlayersSeparateCars{ MODE_TWOPLAYER_SEPARATE_CARS };
    eCamMode        m_nModeForTwoPlayersSameCarShootingAllowed{ MODE_TWOPLAYER_IN_CAR_AND_SHOOTING };
    eCamMode        m_nModeForTwoPlayersSameCarShootingNotAllowed{ MODE_BEHINDCAR };
    eCamMode        m_nModeForTwoPlayersNotBothInCar{ MODE_TWOPLAYER };
    bool            m_bGarageFixedCamPositionSet{};
    bool            m_bDoingSpecialInterp{};
    bool            m_bScriptParametersSetForInterp{};
    bool            m_bFading{};
    bool            m_bMusicFading{};
    bool            m_bMusicFadedOut{};
    bool            m_bFailedCullZoneTestPreviously{};
    bool            m_bFadeTargetIsSplashScreen{};
    bool            m_bWorldViewerBeingUsed{};
    bool            m_bTransitionJUSTStarted{};
    bool            m_bTransitionState{};
    uint8_t         m_nActiveCam{};
    uint32_t        m_nCamShakeStart{};
    uint32_t        m_nFirstPersonCamLastInputTime{};
    uint32_t        m_nLongestTimeInMill{ 5000 };
    uint32_t        m_nNumberOfTrainCamNodes{};
    uint32_t        m_nTimeLastChange{};
    uint32_t        m_nTimeWeLeftIdle_StillNoInput{};
    uint32_t        m_nTimeWeEnteredIdle{};
    uint32_t        m_nTimeTransitionStart{};
    uint32_t        m_nTransitionDuration{};
    uint32_t        m_nTransitionDurationTargetCoors{};
    uint32_t        m_nBlurBlue{};
    uint32_t        m_nBlurGreen{};
    uint32_t        m_nBlurRed{};
    uint32_t        m_nBlurType{};
    uint32_t        m_nWorkOutSpeedThisNumFrames{4};
    uint32_t        m_nNumFramesSoFar{};
    uint32_t        m_nCurrentTrainCamNode{};
    uint32_t        m_nMotionBlur{};
    uint32_t        m_nMotionBlurAddAlpha{};
    uint32_t        m_nCheckCullZoneThisNumFrames{6};
    uint32_t        m_nZoneCullFrameNumWereAt{};
    uint32_t        m_nWhoIsInControlOfTheCamera{};
    uint32_t        m_nCarZoom{2};
    float           m_fCarZoomBase{};
    float           m_fCarZoomTotal{};
    float           m_fCarZoomSmoothed{};
    float           m_fCarZoomValueScript{};
    uint32_t        m_nPedZoom{2};
    float           m_fPedZoomBase{};
    float           m_fPedZoomTotal{};
    float           m_fPedZoomSmoothed{};
    float           m_fPedZoomValueScript{};
    float           m_fCamFrontXNorm{};
    float           m_fCamFrontYNorm{};
    float           m_fDistanceToWater{};
    float           m_fHeightOfNearestWater{};
    float           m_fFOVDuringInter{};
    float           m_fLODDistMultiplier{1.f};
    float           m_fGenerationDistMultiplier{};
    float           m_fAlphaSpeedAtStartInter{};
    float           m_fAlphaWhenInterPol{};
    float           m_fAlphaDuringInterPol{};
    float           m_fBetaDuringInterPol{};
    float           m_fBetaSpeedAtStartInter{};
    float           m_fBetaWhenInterPol{};
    float           m_fFOVWhenInterPol{};
    float           m_fFOVSpeedAtStartInter{};
    float           m_fStartingBetaForInterPol{};
    float           m_fStartingAlphaForInterPol{};
    float           m_fPedOrientForBehindOrInFront{};
    float           m_fCameraAverageSpeed{};
    float           m_fCameraSpeedSoFar{};
    float           m_fCamShakeForce{};
    float           m_fFovForTrain{70.f};
    float           m_fFOV_Wide_Screen{};
    float           m_fNearClipScript{ 0.9f };
    float           m_fOldBetaDiff{};
    float           m_fPositionAlongSpline{};
    float           m_fScreenReductionPercentage{};
    float           m_fScreenReductionSpeed{};
    float           m_fAlphaForPlayerAnim1rstPerson{};
    float           m_fOrientation{};
    float           m_fPlayerExhaustion{1.f};
    float           m_fSoundDistUp{};
    float           m_fSoundDistUpAsRead{};
    float           m_fSoundDistUpAsReadOld{};
    float           m_fAvoidTheGeometryProbsTimer{};
    uint16_t        m_nAvoidTheGeometryProbsDirn{};
    uint16_t        skip8;
    float           m_fWideScreenReductionAmount{};
    float           m_fStartingFOVForInterPol{};
    CCam            m_aCams[3]{};
    uintptr_t*      m_pToGarageWeAreIn{};
    uintptr_t*      m_pToGarageWeAreInForHackAvoidFirstPerson{};
    CQueuedMode     m_PlayerMode{};
    CQueuedMode     m_PlayerWeaponMode{};
    CVector         m_vecPreviousCameraPosition{};
    CVector         m_vecRealPreviousCameraPosition{};
    CVector         m_vecAimingTargetCoors{};
    CVector         m_vecFixedModeVector{};
    CVector         m_vecFixedModeSource{};
    CVector         m_vecFixedModeUpOffSet{};
    CVector         m_vecCutSceneOffset{};
    CVector         m_vecStartingSourceForInterPol{};
    CVector         m_vecStartingTargetForInterPol{};
    CVector         m_vecStartingUpForInterPol{};
    CVector         m_vecSourceSpeedAtStartInter{};
    CVector         m_vecTargetSpeedAtStartInter{};
    CVector         m_vecUpSpeedAtStartInter{};
    CVector         m_vecSourceWhenInterPol{};
    CVector         m_vecTargetWhenInterPol{};
    CVector         m_vecUpWhenInterPol{};
    CVector         m_vecClearGeometryVec{};
    CVector         m_vecGameCamPos{};
    CVector         m_vecSourceDuringInter{};
    CVector         m_vecTargetDuringInter{};
    CVector         m_vecUpDuringInter{};
    CVector         m_vecAttachedCamOffset{};
    CVector         m_vecAttachedCamLookAt{};
    float           m_fAttachedCamAngle{};

    RwCamera*       m_pRwCamera{};
    CEntityGta*     m_pTargetEntity{};
    CEntityGta*     m_pAttachedEntity{};
    CCamPathSplines m_aPathArray[4]{};
    bool            m_bMirrorActive{};
    bool            m_bResetOldMatrix{};
    uint8_t         skip0[2];
    float           m_sphereMapRadius;
    CMatrix         m_mCameraMatrix{ CMatrix::Identity() };
    CMatrix         m_mCameraMatrixOld{};
    CMatrix         m_mViewMatrix{};
    CMatrix         m_mMatInverse{};
    CMatrix         m_mMatMirrorInverse{};
    CMatrix         m_mMatMirror{};
    CVector         m_avecFrustumNormals[4]{};
    CVector         m_avecFrustumWorldNormals[4]{};
    CVector         m_avecFrustumWorldNormals_Mirror[4]{};
    float           m_fFrustumPlaneOffsets[4]{};
    float           m_fFrustumPlaneOffsets_Mirror[4]{};
    CVector         m_vecOldSourceForInter{};  //!< unused?
    CVector         m_vecOldFrontForInter{}; //!< unused?
    CVector         m_vecOldUpForInter{};    //!< unused?
    float           m_vecOldFOVForInter{}; //!< unused?
    float           m_fFloatingFade{};
    float           m_fFloatingFadeMusic{};
    float           m_fTimeToFadeOut{};
    float           m_fTimeToFadeMusic{};
    float           m_fTimeToWaitToFadeMusic{};
    float           m_fFractionInterToStopMoving{0.25f};
    float           m_fFractionInterToStopCatchUp{0.75f};
    float           m_fFractionInterToStopMovingTarget{};
    float           m_fFractionInterToStopCatchUpTarget{};
    float           m_fGaitSwayBuffer{0.85f};
    float           m_fScriptPercentageInterToStopMoving{};
    float           m_fScriptPercentageInterToCatchUp{};
    uint32_t        m_nScriptTimeForInterpolation{};
    eFadeFlag       m_iFadingDirection{};
    uint8_t         skip6[2];
    int32_t         *m_nModeObbeCamIsInForCar;
    eCamMode        m_nModeToGoTo{ MODE_FOLLOWPED };
    eFadeFlag       m_nMusicFadingDirection{};
    eSwitchType     m_nTypeOfSwitch{ eSwitchType::INTERPOLATION };
    uint8_t         skip7[2];
    uint32_t        m_nFadeStartTime{};
    uint32_t        m_nFadeTimeStartedMusic{};
    int32_t         m_numExtrasEntitysToIgnore{};
    CEntityGta*     m_pExtrasEntitysToIgnore[2]{};
    float           m_duckZMod{};
    float           m_duckZMod_Aim{};
    float           m_fTrackLinearStartTime{};
    float           m_fTrackLinearEndTime{};
    CVector         m_vectorTrackFrom{};
    CVector         m_vectorTrackTo{};
    int             m_bVectorTrackSmoothEnds{};
    CVector         m_VectorTrackScript{};
    int             m_bVectorTrackScript{};
    float           m_fShakeIntensity{};
    float           m_fStartShakeTime{};
    float           m_fEndShakeTime{};
    int             m_bShakeScript;
    int32_t         m_CurShakeCam{};
    float           m_FOVLerpStartTime;
    float           m_FOVLerpEndTime;
    float           m_FOVLerpStart;
    float           m_FOVLerpEnd;
    bool            m_bFOVLerpSmoothEnds;
    uint8_t         m_bFOVScript[3];
    float           m_fFOVNew{};
    float           m_fMoveLinearStartTime{};
    float           m_fMoveLinearEndTime{};
    CVector         m_vectorMoveFrom{};
    CVector         m_vectorMoveTo{};
    int32_t         m_bVectorMoveSmoothEnds{};
    CVector         m_VectorMoveScript{};
    bool            m_bVectorMoveScript{};
    bool            m_bPersistFOV;
    bool            m_bPersistCamPos;
    bool            m_bPersistCamLookAt;
    int32_t         m_bForceCinemaCam;
    CVehicleCamTweak m_aCamTweak[5]{};
    int32_t         m_bInitedVehicleCamTweaks{};
    float           m_VehicleTweakLenMod;
    float           m_VehicleTweakTargetZMod;
    float           m_VehicleTweakPitchMod;
    int32_t         m_nCurrentTweakModelIndex{};
    float           m_TimeStartFOVLO;
    float           m_TimeEndFOVLO;
    float           m_FOVStartFOVLO;
    CVector         m_StartPositionFOVLO;
    float           m_FOVTargetFOVLO;
    bool            m_bSmoothLerpFOVLO;
    bool            m_bInitLockOnCam[3];
};
static_assert(sizeof(CCameraGta) == 0xd00, "Invalid");

extern CCameraGta TheCamera;

#endif //LIVERUSSIA_CAMERAGTA_H
