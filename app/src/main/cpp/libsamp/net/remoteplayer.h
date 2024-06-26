#pragma once

#include "../vendor/raknet/rijndael.h"

#define PLAYER_STATE_NONE						0
#define PLAYER_STATE_ONFOOT						17
#define PLAYER_STATE_PASSENGER					18
#define PLAYER_STATE_DRIVER						19
#define PLAYER_STATE_WASTED						32
#define PLAYER_STATE_SPAWNED					33
#define PLAYER_STATE_EXIT_VEHICLE				4
#define PLAYER_STATE_ENTER_VEHICLE_DRIVER		5
#define PLAYER_STATE_ENTER_VEHICLE_PASSENGER	6
#define PLAYER_STATE_SPECTATING					9

#define UPDATE_TYPE_NONE		0
#define UPDATE_TYPE_ONFOOT		1
#define UPDATE_TYPE_INCAR		2
#define UPDATE_TYPE_PASSENGER	3

class CRemotePlayer
{
public:
	CRemotePlayer();
	~CRemotePlayer();

	void Process();
	uint32_t GetPlayerColorAsARGB();
	bool Spawn(uint8_t byteTeam, unsigned int iSkin, CVector *vecPos, float fRotation, uint32_t dwColor, uint8_t byteFightingStyle, bool bVisible);
	void Remove();

	CPlayerPed* GetPlayerPed() { return m_pPlayerPed; }

	void StateChange(uint8_t byteNewState, uint8_t byteOldState);
	void SetState(uint8_t byteState)
	{
		if(byteState != m_byteState)
		{
			StateChange(byteState, m_byteState);
			m_byteState = byteState;
		}
	}
	uint8_t GetState() { return m_byteState; };

	void StoreBulletSyncData(BULLET_SYNC* blSync);
	void SetID(PLAYERID playerId) { m_PlayerID = playerId; }
	PLAYERID GetID() { return m_PlayerID; }

	void SetNPC(bool bNPC) { m_bIsNPC = bNPC; }
	bool IsNPC() { return m_bIsNPC; }

	bool IsAFK() { return m_bIsAFK; }

	void SetPlayerColor(uint32_t dwColor);

	void Say(unsigned char* szText);

	bool IsActive()
	{
		if(m_pPlayerPed && m_byteState != PLAYER_STATE_NONE)
			return true;

		return false;
	}
	void RemoveFromVehicle();

	void HandleDeath();
	void HandleAnimations();
	void UpdateAimFromSyncData(AIM_SYNC_DATA *paimSync);
	void StoreOnFootFullSyncData(ONFOOT_SYNC_DATA *pofSync, uint32_t time);
	void StoreInCarFullSyncData(INCAR_SYNC_DATA *picSync, uint32_t time);
	void StorePassengerFullSyncData(PASSENGER_SYNC_DATA *psSync);
	void UpdateOnFootPositionAndSpeed(CVector *vecPos, CVector *vecMoveSpeed);
	void StoreTrailerFullSyncData(TRAILER_SYNC_DATA* trSync);
	void UpdateOnFootTargetPosition();
	void SlerpRotation();

	void UpdateInCarMatrixAndSpeed(RwMatrix* mat, CVector* pos, CVector* speed);
	void UpdateInCarTargetPosition();
	void UpdateVehicleRotation();
	void EnterVehicle(VEHICLEID VehicleID, bool bPassenger);
	void ExitVehicle();

	uint32_t GetPlayerColor();
	void ShowGlobalMarker(short sX, short sY, short sZ);
	void HideGlobalMarker();

public:
	bool				m_bShowNameTag;
	bool				m_bKeyboardOpened;
	float 				m_fCurrentHealth;
	float 				m_fCurrentArmor;

private:
	CPlayerPed			*m_pPlayerPed;
	CVehicle			*m_pCurrentVehicle;

	PLAYERID			m_PlayerID;
public:
	VEHICLEID 			m_VehicleID;
private:
	bool				m_bIsNPC;
	bool				m_bIsAFK;
	uint32_t 			m_dwMarkerID;
	uint32_t 			m_dwGlobalMarkerID;
	uint8_t 			m_byteState;
	uint8_t				m_byteUpdateFromNetwork;
	uint32_t			m_dwLastRecvTick;
	uint32_t			m_dwLastHeadUpdate;
	uint32_t			m_dwUnkTime;
	//uint8_t				m_iKey0;

	uint32_t			m_dwCurrentAnimation;
	uint16_t			m_usPlayingAnimIdx;
	bool				m_bWasSettedFlag;

	ONFOOT_SYNC_DATA	m_ofSync;
	INCAR_SYNC_DATA		m_icSync;
	PASSENGER_SYNC_DATA m_psSync;

	uint8_t m_byteWeaponShotID;

	CVector 				m_vecPosOffset;

	CVector				m_vecOnFootTargetPos;
	CVector 				m_vecOnFootTargetSpeed;

	CVector 				m_vecInCarTargetPos;
	CVector 				m_vecInCarTargetSpeed;
	CQuaternion			m_InCarQuaternion;

	uint8_t				m_byteSpecialAction;
	uint8_t				m_byteSeatID;

	void ProcessSpecialActions(BYTE byteSpecialAction);
};