//
// Created by plaka on 23.02.2023.
//

#ifndef LIVERUSSIA_CVEHICLEGTA_H
#define LIVERUSSIA_CVEHICLEGTA_H

#include "CPhysicalGta.h"
#include "tHandlingData.h"
#include "CPedGta.h"

struct CVehicleGta : CPhysicalGta
{
    uint8_t m_VehicleAudioEntity[588];		// 0184-900
    tHandlingData* pHandling;	// 900-904
    uintptr_t* pFlyingHandling;
    union{
        eVehicleHandlingFlags  m_nHandlingFlagsIntValue;
        struct {
            uint32_t b1gBoost : 1;
            uint32_t b2gBoost : 1;
            uint32_t bNpcAntiRoll : 1;
            uint32_t bNpcNeutralHandl : 1;
            uint32_t bNoHandbrake : 1;
            uint32_t bSteerRearwheels : 1;
            uint32_t bHbRearwheelSteer : 1;
            uint32_t bAltSteerOpt : 1;
            uint32_t bWheelFNarrow2 : 1;
            uint32_t bWheelFNarrow : 1;
            uint32_t bWheelFWide : 1;
            uint32_t bWheelFWide2 : 1;
            uint32_t bWheelRNarrow2 : 1;
            uint32_t bWheelRNarrow : 1;
            uint32_t bWheelRWide : 1;
            uint32_t bWheelRWide2 : 1;
            uint32_t bHydraulicGeom : 1;
            uint32_t bHydraulicInst : 1;
            uint32_t bHydraulicNone : 1;
            uint32_t bNosInst : 1;
            uint32_t bOffroadAbility : 1;
            uint32_t bOffroadAbility2 : 1;
            uint32_t bHalogenLights : 1;
            uint32_t bProcRearwheelFirst : 1;
            uint32_t bUseMaxspLimit : 1;
            uint32_t bLowRider : 1;
            uint32_t bStreetRacer : 1;
            uint32_t bSwingingChassis : 1;
        } handlingFlags;
    };
    uint8_t AutoPilot[152];
    union
    {
        struct
        {
            uint32_t m_nVehicleUpperFlags;
            uint32_t m_nVehicleLowerFlags;
        };
        struct {
            uint32_t bIsLawEnforcer : 1; // Is this guy chasing the player at the moment
            uint32_t bIsAmbulanceOnDuty : 1; // Ambulance trying to get to an accident
            uint32_t bIsFireTruckOnDuty : 1; // Firetruck trying to get to a fire
            uint32_t bIsLocked : 1; // Is this guy locked by the script (cannot be removed)
            uint32_t bEngineOn : 1; // For sound purposes. Parked cars have their engines switched off (so do destroyed cars)
            uint32_t bIsHandbrakeOn : 1; // How's the handbrake doing ?
            uint32_t bLightsOn : 1; // Are the lights switched on ?
            uint32_t bFreebies : 1; // Any freebies left in this vehicle ?

            uint32_t bIsVan : 1; // Is this vehicle a van (doors at back of vehicle)
            uint32_t bIsBus : 1; // Is this vehicle a bus
            uint32_t bIsBig : 1; // Is this vehicle a bus
            uint32_t bLowVehicle : 1; // Need this for sporty type cars to use low getting-in/out anims
            uint32_t bComedyControls : 1; // Will make the car hard to control (hopefully in a funny way)
            uint32_t bWarnedPeds : 1; // Has scan and warn peds of danger been processed?
            uint32_t bCraneMessageDone : 1; // A crane message has been printed for this car allready
            uint32_t bTakeLessDamage : 1; // This vehicle is stronger (takes about 1/4 of damage)

            uint32_t bIsDamaged : 1; // This vehicle has been damaged and is displaying all its components
            uint32_t bHasBeenOwnedByPlayer : 1;// To work out whether stealing it is a crime
            uint32_t bFadeOut : 1; // Fade vehicle out
            uint32_t bIsBeingCarJacked : 1; // Fade vehicle out
            uint32_t bCreateRoadBlockPeds : 1;// If this vehicle gets close enough we will create peds (coppers or gang members) round it
            uint32_t bCanBeDamaged : 1; // Set to FALSE during cut scenes to avoid explosions
            uint32_t bOccupantsHaveBeenGenerated : 1; // Is true if the occupants have already been generated. (Shouldn't happen again)
            uint32_t bGunSwitchedOff : 1; // Level designers can use this to switch off guns on boats

            uint32_t bVehicleColProcessed : 1;// Has ProcessEntityCollision been processed for this car?
            uint32_t bIsCarParkVehicle : 1; // Car has been created using the special CAR_PARK script command
            uint32_t bHasAlreadyBeenRecorded : 1; // Used for replays
            uint32_t bPartOfConvoy : 1;
            uint32_t bHeliMinimumTilt : 1; // This heli should have almost no tilt really
            uint32_t bAudioChangingGear : 1; // sounds like vehicle is changing gear
            uint32_t bIsDrowning : 1; // is vehicle occupants taking damage in water (i.e. vehicle is dead in water)
            uint32_t bTyresDontBurst : 1; // If this is set the tyres are invincible

            uint32_t bCreatedAsPoliceVehicle : 1;// True if this guy was created as a police vehicle (enforcer, policecar, miamivice car etc)
            uint32_t bRestingOnPhysical : 1; // Dont go static cause car is sitting on a physical object that might get removed
            uint32_t bParking : 1;
            uint32_t bCanPark : 1;
            uint32_t bFireGun : 1; // Does the ai of this vehicle want to fire it's gun?
            uint32_t bDriverLastFrame : 1; // Was there a driver present last frame ?
            uint32_t bNeverUseSmallerRemovalRange : 1;// Some vehicles (like planes) we don't want to remove just behind the camera.
            uint32_t bIsRCVehicle : 1; // Is this a remote controlled (small) vehicle. True whether the player or AI controls it.

            uint32_t bAlwaysSkidMarks : 1; // This vehicle leaves skidmarks regardless of the wheels' states.
            uint32_t bEngineBroken : 1; // Engine doesn't work. Player can get in but the vehicle won't drive
            uint32_t bVehicleCanBeTargetted : 1;// The ped driving this vehicle can be targetted, (for Torenos plane mission)
            uint32_t bPartOfAttackWave : 1; // This car is used in an attack during a gang war
            uint32_t bWinchCanPickMeUp : 1; // This car cannot be picked up by any ropes.
            uint32_t bImpounded : 1; // Has this vehicle been in a police impounding garage
            uint32_t bVehicleCanBeTargettedByHS : 1;// Heat seeking missiles will not target this vehicle.
            uint32_t bSirenOrAlarm : 1; // Set to TRUE if siren or alarm active, else FALSE

            uint32_t bHasGangLeaningOn : 1;
            uint32_t bGangMembersForRoadBlock : 1;// Will generate gang members if NumPedsForRoadBlock > 0
            uint32_t bDoesProvideCover : 1; // If this is false this particular vehicle can not be used to take cover behind.
            uint32_t bMadDriver : 1; // This vehicle is driving like a lunatic
            uint32_t bUpgradedStereo : 1; // This vehicle has an upgraded stereo
            uint32_t bConsideredByPlayer : 1; // This vehicle is considered by the player to enter
            uint32_t bPetrolTankIsWeakPoint : 1;// If false shootong the petrol tank will NOT Blow up the car
            uint32_t bDisableParticles : 1; // Disable particles from this car. Used in garage.

            uint32_t bHasBeenResprayed : 1; // Has been resprayed in a respray garage. Reset after it has been checked.
            uint32_t bUseCarCheats : 1; // If this is true will set the car cheat stuff up in ProcessControl()
            uint32_t bDontSetColourWhenRemapping : 1;// If the texture gets remapped we don't want to change the colour with it.
            uint32_t bUsedForReplay : 1; // This car is controlled by replay and should be removed when replay is done.
        } m_nVehicleFlags;
    };

    uint32_t m_nCreationTime;
    uint8_t m_nPrimaryColor;			// 1076-1077	;byteColor1
    uint8_t m_nSecondaryColor;			// 1077-1078	;byteColor2
    uint8_t m_colour3;
    uint8_t m_colour4;
    uint8_t m_comp1;
    uint8_t m_comp2;
    short  m_anUpgrades[15];
    float m_wheelScale;
    uint16_t m_nAlarmState;
    int16_t m_nForcedRandomRouteSeed; // if this is non-zero the random wander gets deterministic
    CPedGta* pDriver;			// 1120-1124	;driver
    CPedGta* pPassengers[8];	// 1124-1152	;pPassengers
    unsigned char  m_nNumPassengers;
    unsigned char  m_nNumGettingIn;
    unsigned char  m_nGettingInFlags;
    unsigned char  m_nGettingOutFlags;
    unsigned char  m_nMaxPassengers;
    unsigned char  m_nWindowsOpenFlags; // initialised, but not used?
    unsigned char  m_nNitroBoosts;
    unsigned char  m_nSpecialColModel;
    int32_t m_pEntityWeAreOn;
    int32_t m_pFire;
    float  m_fSteerAngle;
    float  m_f2ndSteerAngle; // used for steering 2nd set of wheels or elevators etc..
    float  m_fGasPedal;
    float  m_fBreakPedal;
    unsigned char  m_nCreatedBy; // see eVehicleCreatedBy
    unsigned char  abc1;
    unsigned char  abc2;
    unsigned char  abc3;
    uint32_t cachedTotalSteer;
    int16_t m_nExtendedRemovalRange;
    unsigned char m_nBombOnBoard : 3; // 0 = None
    // 1 = Timed
    // 2 = On ignition
    // 3 = remotely set ?
    // 4 = Timed Bomb has been activated
    // 5 = On ignition has been activated
    unsigned char m_nOverrideLights : 2; // uses enum NO_CAR_LIGHT_OVERRIDE, FORCE_CAR_LIGHTS_OFF, FORCE_CAR_LIGHTS_ON
    unsigned char m_nWinchType : 2; // Does this vehicle use a winch?
    unsigned char m_nGunsCycleIndex : 2; // Cycle through alternate gun hardpoints on planes/helis
    unsigned char m_nOrdnanceCycleIndex : 2; // Cycle through alternate ordnance hardpoints on planes/helis
    uint8_t nUsedForCover;
    uint8_t AmmoInClip;
    uint8_t PacMansCollected;
    uint8_t PedsPositionForRoadBlock;
    uint8_t NumPedsForRoadBlock[4];
    float   m_fDirtLevel; // Dirt level of vehicle body texture: 0.0f=fully clean, 15.0f=maximum dirt visible
    unsigned char m_nCurrentGear;
    uint8_t skip11[3];
    float   m_fGearChangeCount; // used as parameter for cTransmission::CalculateDriveAcceleration, but doesn't change
    float   m_fWheelSpinForAudio;
    float fHealth;				// 1224-1228	;fHealth
    CVehicleGta* m_pTowingVehicle;
    CVehicleGta* pTrailer;			// 1232 - 1236 - trailer
    uint8_t skip12[48];
    uint32_t dwDoorsLocked;
    uint32_t m_nProjectileWeaponFiringTime; // manual-aimed projectiles for hunter, lock-on projectile for hydra
    uint32_t m_nAdditionalProjectileWeaponFiringTime; // manual-aimed projectiles for hydra
    uint32_t m_nTimeForMinigunFiring; // minigun on hunter
    unsigned char m_nLastWeaponDamageType; // see eWeaponType, -1 if no damage
};

#endif //LIVERUSSIA_CVEHICLEGTA_H
