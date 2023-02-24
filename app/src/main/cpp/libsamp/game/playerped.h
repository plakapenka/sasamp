#pragma once
#include "aimstuff.h"
#include "bass.h"
#include "eWeaponType.h"
#include "CWeaponGta.h"
#include "CVehicleGta.h"
#include "vehicle.h"

class CPlayerPed: public CEntity
{
public:
	CPlayerPed();	// local
	CPlayerPed(uint8_t bytePlayerNumber, int iSkin, float fX, float fY, float fZ, float fRotation); // remote
	~CPlayerPed();

	CAMERA_AIM * GetCurrentAim();
	void SetCurrentAim(CAMERA_AIM *pAim);

	uint16_t GetCameraMode();

	void SetCameraMode(uint16_t byteCamMode);

	float GetCameraExtendedZoom();
	void ClumpUpdateAnimations(float step, int flag);
	uint8_t GetExtendedKeys();

	void ApplyCrouch();
	void ResetCrouch();
	bool IsCrouching();

	void SetCameraExtendedZoom(float fZoom);

	void SetDead();
	// 0.3.7
	bool IsInVehicle();
	// 0.3.7
	bool IsAPassenger();
	// 0.3.7
	void ApplyCommandTask(char* a2, int a4, int a5, int a6, CVector* a7, char a8, float a9, int a10, int a11, char a12);
	CVehicleGta* GetGtaVehicle();
	// 0.3.7
	void RemoveFromVehicleAndPutAt(float fX, float fY, float fZ);
	// 0.3.7
	void SetInitialState();
	// 0.3.7
	void SetHealth(float fHealth);
	void SetArmour(float fArmour);
	// 0.3.7
	float GetHealth();
	float GetArmour();
	// 0.3.7
	void TogglePlayerControllable(bool bToggle, bool isTemp = false);
	// 0.3.7
	void SetModelIndex(unsigned int uiModel);

	bool IsAnimationPlaying(char* szAnimName);
	void ClearAllTasks();

	void SetInterior(uint8_t byteID, bool refresh = true);

	void PutDirectlyInVehicle(CVehicle *pVehicle, int iSeat);
	void EnterVehicle(int iVehicleID, bool bPassenger);
	// 0.3.7
	void ExitCurrentVehicle();
	// 0.3.7
	int GetVehicleSeatID();

	CEntityGta* GetEntityUnderPlayer();

	void GiveWeapon(int iWeaponID, int iAmmo);
	uint8_t GetCurrentWeapon();
	void SetCurrentWeapon(eWeaponType iWeaponID);

	void SetPlayerAimState();
	void ClearPlayerAimState();
	void SetAimZ(float fAimZ);
	float GetAimZ();
	CWeaponGta* GetCurrentWeaponSlot();
	//CAMERA_AIM* GetCurrentAim();

	void ClearAllWeapons();
	void DestroyFollowPedTask();
	void ResetDamageEntity();

	// 0.3.7
	void RestartIfWastedAt(CVector *vecRestart, float fRotation);
	// 0.3.7
	void ForceTargetRotation(float fRotation);
	// 0.3.7
	uint8_t GetActionTrigger();
	// 0.3.7
	bool IsDead();
	uint16_t GetKeys(uint16_t *lrAnalog, uint16_t *udAnalog);
	void SetKeys(uint16_t wKeys, uint16_t lrAnalog, uint16_t udAnalog);
	// 0.3.7
	void ShowMarker(uint32_t iMarkerColorID);
	// 0.3.7
	void HideMarker();
	// 0.3.7
	void SetFightingStyle(int iStyle);
	// 0.3.7
	void SetRotation(float fRotation);
	// 0.3.7
	void ApplyAnimation( char *szAnimName, char *szAnimFile, float fT, bool opt1, bool opt2, bool opt3, bool opt4, int iUnk );
	// 0.3.7
	void GetBonePosition(int iBoneID, CVector* vecOut);
	// roflan
	//BYTE FindDeathReasonAndResponsiblePlayer(PLAYERID *nPlayer);
	PLAYERID FindDeathResponsiblePlayer();
	void SetActionTrigger(uint8_t action);
	CPedGta * GetGtaActor() { return m_pPed; };

	void AttachObject(ATTACHED_OBJECT_INFO* pInfo, int iSlot);
	void SetAttachOffset(int iSlot, CVector pos, CVector rot);
	void DeattachObject(int iSlot);
	bool IsHasAttach();
	void FlushAttach();
	void ProcessAttach();

	void ProcessHeadMatrix();

	bool IsPlayingAnim(int idx);
	int GetCurrentAnimationIndex(float& blendData);
	void PlayAnimByIdx(int idx, float BlendData, bool loop = false, bool freeze = false, uint8_t time = 0);

	void SetMoveAnim(int iAnimGroup);

public:
	CPedGta*	m_pPed;
	uint8_t		m_bytePlayerNumber;
	uint32_t	m_dwArrow;
	bool lToggle;

	BYTE m_iCurrentSpecialAction = 0;
	int drunk_level = 0;
	MATRIX4X4 m_HeadBoneMatrix;
	ATTACHED_OBJECT_INFO_INTERNAL m_aAttachedObjects[MAX_ATTACHED_OBJECTS];

	void ProcessSpecialAction(BYTE byteSpecialAction);

	void ClearAnimations();

	void SetWeaponAmmo(int iWeaponID, int iAmmo);

    uint8_t animFlagTime;
	bool animFlagLoop;
	bool animFlagFreeze;
	bool animFlagLockY;
	bool animFlagLockX;
	bool IsActionCarry;
    bool m_iPissingState = false;
	DWORD m_dwPissParticlesHandle = 0;

    bool IsValidAttach(int iSlot);

	void SetCameraExtendedZoom(float fZoom, float fAspectRatio);

    void FireInstant();

    void ProcessBulletData(BULLET_DATA *btData);

    bool m_bHaveBulletData;
	BULLET_DATA 		m_bulletData;

	void GetWeaponInfoForFire(int bLeft, CVector *vecBone, CVector *vecOut);

	CVector *GetCurrentWeaponFireOffset();

	CVehicle *GetCurrentVehicle();

	CVehicleGta *GetCurrentGtaVehicle();

	uint32_t GetCurrentGTAVehicleID();

	VEHICLEID GetCurrentSampVehicleID();

	void TurnBody();

    void ClearLook();

    void GiveWeapon(eWeaponType weaponType, uint32_t ammoQuantity, bool GenerateOldWeaponPickup);
};