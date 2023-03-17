#pragma once

#include <thread>
#include "rgba.h"
#include "common.h"
#include "aimstuff.h"
#include "pad.h"
#include "quaternion.h"
#include "playerped.h"
#include "vehicle.h"
#include "object.h"
#include "camera.h"
#include "font.h"
#include "scripting.h"
#include "radarcolors.h"
#include "util.h"
#include "sprite2d.h"
#include "RW/RenderWare.h"
#include "widget.h"
#include "widgetmanager.h"
#include "stream.h"
#include "CHandlingDefault.h"
#include "CActorPed.h"
#include "CExtendedCarColors.h"
#include "CWeaponsOutFit.h"
#include "CRadarRect.h"
#include "CFirstPersonCamera.h"
#include "CStreaming.h"
#include "TextureDatabaseRuntime.h"

class CGame
{
public:
	CGame();
	~CGame() {};

	int GetScreenWidth() { return RsGlobal->maximumWidth; };
	int GetScreenHeight() { return RsGlobal->maximumHeight; };

	static void InitInMenu();
	static void InitInGame();

	static void RemoveModel(int iModel, bool bFromStreaming);

	void HandleChangedHUDStatus();
	bool IsToggledHUDElement(int iID);

	void ToggleHUDElement(int iID, bool bToggle);

	CCamera* GetCamera() { return m_pGameCamera; }
	// 0.3.7
	CPlayerPed* FindPlayerPed() { if(!m_pGamePlayer) m_pGamePlayer = new CPlayerPed(); return m_pGamePlayer; }
	// 0.3.7
	static uint8_t FindFirstFreePlayerPedSlot();
	// 0.3.7
	CPlayerPed* NewPlayer(int iSkin, float fX, float fY, float fZ, float fRotation, uint8_t byteCreateMarker = 0);
	// 0.3.7
	static void RemovePlayer(CPlayerPed* pPlayer);
	// 0.3.7
	CObject* NewObject(int iModel, float fPosX, float fPosY, float fPosZ, CVector vecRot, float fDrawDistastatic);
	uint32_t CreatePickup(int iModel, int iType, float fX, float fY, float fZ, int *unk);

	static float FindGroundZForCoord(float x, float y, float z);
	// 0.3.7
	static uint8_t GetActiveInterior();
	// 0.3.7
	static void SetWorldTime(int iHour, int iMinute = 0);
	// 0.3.7
	void SetWorldWeather(unsigned char byteWeatherID) const;

	// 0.3.7
	void EnableZoneNames(bool bEnable);
	void PlaySound(int iSound, float fX, float fY, float fZ);

	// 0.3.7
	void ToggleRaceCheckpoints(bool bEnabled) { m_bRaceCheckpointsEnabled = bEnabled; }
	// 0.3.7
	void SetCheckpointInformation(CVector *pos, CVector *extent);
	// 0.3.7
	void SetRaceCheckpointInformation(uint8_t byteType, CVector *pos, CVector *next, float fSize);
	// 0.3.7
	void UpdateCheckpoints();
	// 0.3.7
	void MakeRaceCheckpoint();
	//
	void DisableCheckpoint();
	// 0.3.7
	void DisableRaceCheckpoint();
	// 0.3.7
	static uint32_t CreateRadarMarkerIcon(int iMarkerType, float fX, float fY, float fZ, int iColor, int iStyle);
	// 0.3.7
	static void DisableMarker(uint32_t dwMarkerID);
	// 0.3.7
	static void LoadRequestedModels();
	// 0.3.7
	static uint8_t IsModelLoaded(unsigned int iModelID);
	// 0.3.7
	static void RefreshStreamingAt(float x, float y);
	// 0.3.7
	static void DisableTrainTraffic();
	// 0.3.7
	static void SetMaxStats();

	static void SetWantedLevel(uint8_t byteLevel);
	static uint8_t GetWantedLevel();

	static bool IsAnimationLoaded(const char szAnimFile[]);

	static void RequestAnimation(const char szAnimFile[]) {
		ScriptCommand(&request_animation, szAnimFile);
	}
	// 0.3.7
	static void DisplayGameText(char* szStr, int iTime, int iType);
	// 0.3.7
	static void SetGravity(float fGravity);
	static void ToggleCJWalk(bool bUseCJWalk);
	// 0.3.7
	static void DisableInteriorEnterExits();

	// 0.3.7
	static void AddToLocalMoney(int iAmmount);

	static void DrawGangZone(float fPos[], uint32_t dwColor);
	static bool bIsGameInited;

	// race checkpoint
	bool			m_bRaceCheckpointsEnabled;
	// checkpoint
	bool			m_bCheckpointsEnabled;

	void CreateCheckPoint();

	uint32_t		m_dwCheckpointMarker;
	CVector			m_vecCheckpointPos;
	CVector			m_vecCheckpointExtent;
	bool isCasinoDiceActive = false;
	bool isAutoShopActive = false;
	bool isCasinoWheelActive = false;
	bool isShowMedicGame = false;
	bool isRegistrationActive = false;
	bool isShopStoreActive = false;
	bool isHornActive = false;
	bool isBanJump = false;
	bool bIsGameExiting = false;

	bool 			m_bDl_enabled = false;

	void exitGame();

private:
	bool aToggleStatusHUD[HUD_MAX];
	CCamera* 		m_pGameCamera;
	CPlayerPed*		m_pGamePlayer;

	uint32_t		m_dwRaceCheckpointHandle;
	uint32_t		m_dwRaceCheckpointMarker;
	float			m_fRaceCheckpointSize;
	uint8_t			m_byteRaceType;
	CVector 			m_vecRaceCheckpointPos;
	CVector			m_vecRaceCheckpointNext;

	bool 			m_bClockEnabled;

};