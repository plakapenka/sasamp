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
            unsigned int b1gBoost : 1;
            unsigned int b2gBoost : 1;
            unsigned int bNpcAntiRoll : 1;
            unsigned int bNpcNeutralHandl : 1;
            unsigned int bNoHandbrake : 1;
            unsigned int bSteerRearwheels : 1;
            unsigned int bHbRearwheelSteer : 1;
            unsigned int bAltSteerOpt : 1;
            unsigned int bWheelFNarrow2 : 1;
            unsigned int bWheelFNarrow : 1;
            unsigned int bWheelFWide : 1;
            unsigned int bWheelFWide2 : 1;
            unsigned int bWheelRNarrow2 : 1;
            unsigned int bWheelRNarrow : 1;
            unsigned int bWheelRWide : 1;
            unsigned int bWheelRWide2 : 1;
            unsigned int bHydraulicGeom : 1;
            unsigned int bHydraulicInst : 1;
            unsigned int bHydraulicNone : 1;
            unsigned int bNosInst : 1;
            unsigned int bOffroadAbility : 1;
            unsigned int bOffroadAbility2 : 1;
            unsigned int bHalogenLights : 1;
            unsigned int bProcRearwheelFirst : 1;
            unsigned int bUseMaxspLimit : 1;
            unsigned int bLowRider : 1;
            unsigned int bStreetRacer : 1;
            unsigned int bSwingingChassis : 1;
        } handlingFlags;
    };
    uint8_t AutoPilot[152];
    union
    {
        struct
        {
            unsigned int m_nVehicleUpperFlags;
            unsigned int m_nVehicleLowerFlags;
        };
        struct {
            unsigned int bIsLawEnforcer : 1; // Is this guy chasing the player at the moment
            unsigned int bIsAmbulanceOnDuty : 1; // Ambulance trying to get to an accident
            unsigned int bIsFireTruckOnDuty : 1; // Firetruck trying to get to a fire
            unsigned int bIsLocked : 1; // Is this guy locked by the script (cannot be removed)
            unsigned int bEngineOn : 1; // For sound purposes. Parked cars have their engines switched off (so do destroyed cars)
            unsigned int bIsHandbrakeOn : 1; // How's the handbrake doing ?
            unsigned int bLightsOn : 1; // Are the lights switched on ?
            unsigned int bFreebies : 1; // Any freebies left in this vehicle ?

            unsigned int bIsVan : 1; // Is this vehicle a van (doors at back of vehicle)
            unsigned int bIsBus : 1; // Is this vehicle a bus
            unsigned int bIsBig : 1; // Is this vehicle a bus
            unsigned int bLowVehicle : 1; // Need this for sporty type cars to use low getting-in/out anims
            unsigned int bComedyControls : 1; // Will make the car hard to control (hopefully in a funny way)
            unsigned int bWarnedPeds : 1; // Has scan and warn peds of danger been processed?
            unsigned int bCraneMessageDone : 1; // A crane message has been printed for this car allready
            unsigned int bTakeLessDamage : 1; // This vehicle is stronger (takes about 1/4 of damage)

            unsigned int bIsDamaged : 1; // This vehicle has been damaged and is displaying all its components
            unsigned int bHasBeenOwnedByPlayer : 1;// To work out whether stealing it is a crime
            unsigned int bFadeOut : 1; // Fade vehicle out
            unsigned int bIsBeingCarJacked : 1; // Fade vehicle out
            unsigned int bCreateRoadBlockPeds : 1;// If this vehicle gets close enough we will create peds (coppers or gang members) round it
            unsigned int bCanBeDamaged : 1; // Set to FALSE during cut scenes to avoid explosions
            unsigned int bOccupantsHaveBeenGenerated : 1; // Is true if the occupants have already been generated. (Shouldn't happen again)
            unsigned int bGunSwitchedOff : 1; // Level designers can use this to switch off guns on boats

            unsigned int bVehicleColProcessed : 1;// Has ProcessEntityCollision been processed for this car?
            unsigned int bIsCarParkVehicle : 1; // Car has been created using the special CAR_PARK script command
            unsigned int bHasAlreadyBeenRecorded : 1; // Used for replays
            unsigned int bPartOfConvoy : 1;
            unsigned int bHeliMinimumTilt : 1; // This heli should have almost no tilt really
            unsigned int bAudioChangingGear : 1; // sounds like vehicle is changing gear
            unsigned int bIsDrowning : 1; // is vehicle occupants taking damage in water (i.e. vehicle is dead in water)
            unsigned int bTyresDontBurst : 1; // If this is set the tyres are invincible

            unsigned int bCreatedAsPoliceVehicle : 1;// True if this guy was created as a police vehicle (enforcer, policecar, miamivice car etc)
            unsigned int bRestingOnPhysical : 1; // Dont go static cause car is sitting on a physical object that might get removed
            unsigned int bParking : 1;
            unsigned int bCanPark : 1;
            unsigned int bFireGun : 1; // Does the ai of this vehicle want to fire it's gun?
            unsigned int bDriverLastFrame : 1; // Was there a driver present last frame ?
            unsigned int bNeverUseSmallerRemovalRange : 1;// Some vehicles (like planes) we don't want to remove just behind the camera.
            unsigned int bIsRCVehicle : 1; // Is this a remote controlled (small) vehicle. True whether the player or AI controls it.

            unsigned int bAlwaysSkidMarks : 1; // This vehicle leaves skidmarks regardless of the wheels' states.
            unsigned int bEngineBroken : 1; // Engine doesn't work. Player can get in but the vehicle won't drive
            unsigned int bVehicleCanBeTargetted : 1;// The ped driving this vehicle can be targetted, (for Torenos plane mission)
            unsigned int bPartOfAttackWave : 1; // This car is used in an attack during a gang war
            unsigned int bWinchCanPickMeUp : 1; // This car cannot be picked up by any ropes.
            unsigned int bImpounded : 1; // Has this vehicle been in a police impounding garage
            unsigned int bVehicleCanBeTargettedByHS : 1;// Heat seeking missiles will not target this vehicle.
            unsigned int bSirenOrAlarm : 1; // Set to TRUE if siren or alarm active, else FALSE

            unsigned int bHasGangLeaningOn : 1;
            unsigned int bGangMembersForRoadBlock : 1;// Will generate gang members if NumPedsForRoadBlock > 0
            unsigned int bDoesProvideCover : 1; // If this is false this particular vehicle can not be used to take cover behind.
            unsigned int bMadDriver : 1; // This vehicle is driving like a lunatic
            unsigned int bUpgradedStereo : 1; // This vehicle has an upgraded stereo
            unsigned int bConsideredByPlayer : 1; // This vehicle is considered by the player to enter
            unsigned int bPetrolTankIsWeakPoint : 1;// If false shootong the petrol tank will NOT Blow up the car
            unsigned int bDisableParticles : 1; // Disable particles from this car. Used in garage.

            unsigned int bHasBeenResprayed : 1; // Has been resprayed in a respray garage. Reset after it has been checked.
            unsigned int bUseCarCheats : 1; // If this is true will set the car cheat stuff up in ProcessControl()
            unsigned int bDontSetColourWhenRemapping : 1;// If the texture gets remapped we don't want to change the colour with it.
            unsigned int bUsedForReplay : 1; // This car is controlled by replay and should be removed when replay is done.
        } m_nVehicleFlags;
    };

    unsigned int m_nCreationTime;
    uint8_t m_nPrimaryColor;			// 1076-1077	;byteColor1
    uint8_t m_nSecondaryColor;			// 1077-1078	;byteColor2
    uint8_t m_colour3;
    uint8_t m_colour4;
    uint8_t m_comp1;
    uint8_t m_comp2;
    short  m_anUpgrades[15];
    float m_wheelScale;
    unsigned short m_nAlarmState;
    short  m_nForcedRandomRouteSeed; // if this is non-zero the random wander gets deterministic
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
    short m_nExtendedRemovalRange;
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
    uint32_t m_pTowingVehicle;
    uint32_t dwTrailer;			// 1232 - 1236 - trailer
    uint8_t skip12[48];
    uint32_t dwDoorsLocked;
    uint32_t m_nProjectileWeaponFiringTime; // manual-aimed projectiles for hunter, lock-on projectile for hydra
    uint32_t m_nAdditionalProjectileWeaponFiringTime; // manual-aimed projectiles for hydra
    uint32_t m_nTimeForMinigunFiring; // minigun on hunter
    unsigned char m_nLastWeaponDamageType; // see eWeaponType, -1 if no damage
};

#endif //LIVERUSSIA_CVEHICLEGTA_H
