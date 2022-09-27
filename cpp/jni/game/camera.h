#pragma once

enum eSwitchType : unsigned short
{
    SWITCHTYPE_NONE,
    SWITCHTYPE_INTERPOLATION,
    SWITCHTYPE_JUMPCUT
};

enum eCamMode : unsigned short {
	MODE_NONE = 0x0,
	MODE_TOPDOWN = 0x1,
	MODE_GTACLASSIC = 0x2,
	MODE_BEHINDCAR = 0x3,
	MODE_FOLLOWPED = 0x4,
	MODE_AIMING = 0x5,
	MODE_DEBUG = 0x6,
	MODE_SNIPER = 0x7,
	MODE_ROCKETLAUNCHER = 0x8,
	MODE_MODELVIEW = 0x9,
	MODE_BILL = 0xA,
	MODE_SYPHON = 0xB,
	MODE_CIRCLE = 0xC,
	MODE_CHEESYZOOM = 0xD,
	MODE_WHEELCAM = 0xE,
	MODE_FIXED = 0xF,
	MODE_1STPERSON = 0x10,
	MODE_FLYBY = 0x11,
	MODE_CAM_ON_A_STRING = 0x12,
	MODE_REACTION = 0x13,
	MODE_FOLLOW_PED_WITH_BIND = 0x14,
	MODE_CHRIS = 0x15,
	MODE_BEHINDBOAT = 0x16,
	MODE_PLAYER_FALLEN_WATER = 0x17,
	MODE_CAM_ON_TRAIN_ROOF = 0x18,
	MODE_CAM_RUNNING_SIDE_TRAIN = 0x19,
	MODE_BLOOD_ON_THE_TRACKS = 0x1A,
	MODE_IM_THE_PASSENGER_WOOWOO = 0x1B,
	MODE_SYPHON_CRIM_IN_FRONT = 0x1C,
	MODE_PED_DEAD_BABY = 0x1D,
	MODE_PILLOWS_PAPS = 0x1E,
	MODE_LOOK_AT_CARS = 0x1F,
	MODE_ARRESTCAM_ONE = 0x20,
	MODE_ARRESTCAM_TWO = 0x21,
	MODE_M16_1STPERSON = 0x22,
	MODE_SPECIAL_FIXED_FOR_SYPHON = 0x23,
	MODE_FIGHT_CAM = 0x24,
	MODE_TOP_DOWN_PED = 0x25,
	MODE_LIGHTHOUSE = 0x26,
	MODE_SNIPER_RUNABOUT = 0x27,
	MODE_ROCKETLAUNCHER_RUNABOUT = 0x28,
	MODE_1STPERSON_RUNABOUT = 0x29,
	MODE_M16_1STPERSON_RUNABOUT = 0x2A,
	MODE_FIGHT_CAM_RUNABOUT = 0x2B,
	MODE_EDITOR = 0x2C,
	MODE_HELICANNON_1STPERSON = 0x2D,
	MODE_CAMERA = 0x2E,
	MODE_ATTACHCAM = 0x2F,
	MODE_TWOPLAYER = 0x30,
	MODE_TWOPLAYER_IN_CAR_AND_SHOOTING = 0x31,
	MODE_TWOPLAYER_SEPARATE_CARS = 0x32,
	MODE_ROCKETLAUNCHER_HS = 0x33,
	MODE_ROCKETLAUNCHER_RUNABOUT_HS = 0x34,
	MODE_AIMWEAPON = 0x35,
	MODE_TWOPLAYER_SEPARATE_CARS_TOPDOWN = 0x36,
	MODE_AIMWEAPON_FROMCAR = 0x37,
	MODE_DW_HELI_CHASE = 0x38,
	MODE_DW_CAM_MAN = 0x39,
	MODE_DW_BIRDY = 0x3A,
	MODE_DW_PLANE_SPOTTER = 0x3B,
	MODE_DW_DOG_FIGHT = 0x3C,
	MODE_DW_FISH = 0x3D,
	MODE_DW_PLANECAM1 = 0x3E,
	MODE_DW_PLANECAM2 = 0x3F,
	MODE_DW_PLANECAM3 = 0x40,
	MODE_AIMWEAPON_ATTACHED = 0x41
};

class CCamera
{
public:
	CCamera() { m_matPos = (MATRIX4X4*)(g_libGTASA+0x8B1104); }
	~CCamera() {}

	// 0.3.7
	void SetBehindPlayer();
	// 0.3.7
	void Restore();
	// 0.3.7
	void SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ);
	// 0.3.7
	void LookAtPoint(float fX, float fY, float fZ, int iType);

	void GetMatrix(PMATRIX4X4 mat);
	// 0.3.7
	void InterpolateCameraPos(VECTOR *posFrom, VECTOR *posTo, int time, uint8_t mode);
	// 0.3.7
	void InterpolateCameraLookAt(VECTOR *posFrom, VECTOR *posTo, int time, uint8_t mode);

	/*
	! TypeOfSwitch : see eSwitchType
    ! ModeToGoTo : see eCamMode
	*/
	void AttachToEntity(ENTITY_TYPE* target, short ModeToGoTo, short TypeOfSwitch);


private:
	ENTITY_TYPE* m_pEntity;
	MATRIX4X4 *m_matPos;
};