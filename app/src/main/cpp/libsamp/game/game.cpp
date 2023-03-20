#include "../main.h"
#include "game.h"
#include "util/CJavaWrapper.h"
#include "net/netgame.h"
#include "java_systems/CHUD.h"
#include "util/patch.h"
#include "CGtaWidgets.h"
#include "game/Models/CModelInfo.h"

void ApplyPatches();
void ApplyInGamePatches();
void InstallHooks();
void LoadSplashTexture();
void InitScripting();

uint16_t *szGameTextMessage;
bool bUsedPlayerSlots[PLAYER_PED_SLOTS];
bool CGame::bIsGameInited = false;

void CGame::RemoveModel(int iModel, bool bFromStreaming)
{
	if (iModel >= 0 && iModel < 20000)
	{
		if (bFromStreaming)
		{
			if (ScriptCommand(&is_model_available, iModel))
				// CStreaming::RemoveModel
				((void(*)(int))(g_libGTASA + 0x002D0128 + 1))(iModel);
		}
		else
		{
			if (ScriptCommand(&is_model_available, iModel))
				ScriptCommand(&release_model);
		}
	}
}

CGame::CGame()
{
	for (bool & i : aToggleStatusHUD)
	{
		i = true;
	}
	m_pGameCamera = new CCamera();
	m_pGamePlayer = nullptr;

	m_bClockEnabled = true;
	m_bCheckpointsEnabled = false;
	m_dwCheckpointMarker = NULL;

	m_bRaceCheckpointsEnabled = false;
	m_dwRaceCheckpointHandle = NULL;
	m_dwRaceCheckpointMarker = NULL;

	memset(&bUsedPlayerSlots[0], 0, PLAYER_PED_SLOTS);
}

void CGame::exitGame()
{
    Log("Exiting game...");

	bIsGameExiting = true;

	//delete pNetGame;
    //exit(0);

  //  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	g_pJavaWrapper->ExitGame();
}

// 0.3.7
uint8_t CGame::FindFirstFreePlayerPedSlot()
{
	uint8_t x = 2;
	while(x != PLAYER_PED_SLOTS)
	{
		if(!bUsedPlayerSlots[x]) return x;
		x++;
	}

	return 0;
}

// 0.3.7
CPlayerPed* CGame::NewPlayer(int iSkin, float fX, float fY, float fZ, float fRotation, uint8_t byteCreateMarker)
{
	uint8_t bytePlayerNum = FindFirstFreePlayerPedSlot();
	if(!bytePlayerNum) return nullptr;

	auto* pPlayerNew = new CPlayerPed(bytePlayerNum, iSkin, fX, fY, fZ, fRotation);
	if(pPlayerNew->m_pPed)
		bUsedPlayerSlots[bytePlayerNum] = true;

	//if(byteCreateMarker) (no xrefs ;( )
	return pPlayerNew;
}

// 0.3.7
void CGame::RemovePlayer(CPlayerPed* pPlayer)
{
	if(pPlayer)
	{
		bUsedPlayerSlots[pPlayer->m_bytePlayerNumber] = false;
		delete pPlayer;
	}
}

CObject *CGame::NewObject(int iModel, float fPosX, float fPosY, float fPosZ, CVector vecRot, float fDrawDistance)
{
	auto *pObjectNew = new CObject(iModel, fPosX, fPosY, fPosZ, vecRot, fDrawDistance);
	return pObjectNew;
}

uint32_t CGame::CreatePickup(int iModel, int iType, float fX, float fY, float fZ, int* unk)
{
	uint32_t hnd;

	if(iModel > 0 && iModel < 20000)
	{
    	if(CModelInfo::ms_modelInfoPtrs[iModel] == nullptr)
    		iModel = 18631;
	}
	else iModel = 18631;

//	if(!ScriptCommand(&is_model_available, iModel))
//	{
//		ScriptCommand(&request_model, iModel);
//		ScriptCommand(&load_requested_models);
//		while(!ScriptCommand(&is_model_available, iModel))
//			usleep(1000);
//	}

	ScriptCommand(&create_pickup, iModel, iType, fX, fY, fZ, &hnd);

	int lol = 32 * (uint16_t)hnd;
	if(lol) lol /= 32;
	if(unk) *unk = lol;

	return hnd;
}

void *Init(void *p)
{
	ApplyPatches();
	InstallHooks();
	LoadSplashTexture();
	GameAimSyncInit();

	CGtaWidgets::init();

	pthread_exit(0);
}

void CGame::InitInMenu()
{
	Log("CGame: InitInMenu");

	pthread_t thread;
	pthread_create(&thread, nullptr, Init, nullptr);

	szGameTextMessage = new uint16_t[1076];
}

void CGame::InitInGame()
{
	
	Log("CGame: InitInGame");
	ApplyInGamePatches();
	InitScripting();
	
	GameResetRadarColors();
}


void CGame::ToggleHUDElement(int iID, bool bToggle)
{
	if (iID < 0 || iID >= HUD_MAX)
	{
		return;
	}
	aToggleStatusHUD[iID] = bToggle;
}

bool CGame::IsToggledHUDElement(int iID)
{
	if (iID < 0 || iID >= HUD_MAX)
	{
		return 1;
	}
	return aToggleStatusHUD[iID];
}

float CGame::FindGroundZForCoord(float x, float y, float z)
{
	float fGroundZ;
	ScriptCommand(&get_ground_z, x, y, z, &fGroundZ);
	return fGroundZ;
}

// 0.3.7
void CGame::SetCheckpointInformation(CVector *pos, CVector *extent)
{
	memcpy(&m_vecCheckpointPos,pos,sizeof(CVector));
	memcpy(&m_vecCheckpointExtent,extent,sizeof(CVector));
}

// 0.3.7
void CGame::SetRaceCheckpointInformation(uint8_t byteType, CVector *pos, CVector *next, float fSize)
{
	memcpy(&m_vecRaceCheckpointPos,pos,sizeof(CVector));
	memcpy(&m_vecRaceCheckpointNext,next,sizeof(CVector));
	m_fRaceCheckpointSize = fSize;
	m_byteRaceType = byteType;

	MakeRaceCheckpoint();
}

// 0.3.7
void CGame::MakeRaceCheckpoint()
{
	if(m_bRaceCheckpointsEnabled)
	{
		DisableRaceCheckpoint();
	}

	ScriptCommand(&create_racing_checkpoint, (int)m_byteRaceType,
				m_vecRaceCheckpointPos.x, m_vecRaceCheckpointPos.y, m_vecRaceCheckpointPos.z,
				m_vecRaceCheckpointNext.x, m_vecRaceCheckpointNext.y, m_vecRaceCheckpointNext.z,
				m_fRaceCheckpointSize, &m_dwRaceCheckpointHandle);

	m_dwRaceCheckpointMarker = CreateRadarMarkerIcon(0, m_vecRaceCheckpointPos.x,
													 m_vecRaceCheckpointPos.y, m_vecRaceCheckpointPos.z, 1005, 0);

	m_bRaceCheckpointsEnabled = true;
}

// 0.3.7
void CGame::DisableRaceCheckpoint()
{
	if (m_dwRaceCheckpointHandle != NULL)
	{
		ScriptCommand(&destroy_racing_checkpoint, m_dwRaceCheckpointHandle);
		m_dwRaceCheckpointHandle = NULL;
	}
	if(m_dwRaceCheckpointMarker != NULL)
	{
		DisableMarker(m_dwRaceCheckpointMarker);
		m_dwRaceCheckpointMarker = NULL;
	}
	m_bRaceCheckpointsEnabled = false;
}
void CGame::DisableCheckpoint() {
	if(m_dwCheckpointMarker != NULL)
	{
		DisableMarker(m_dwCheckpointMarker);
		m_dwCheckpointMarker = NULL;
	}
	m_bCheckpointsEnabled = false;
}

void CGame::CreateCheckPoint()
{
	if(m_bCheckpointsEnabled)
	{
		DisableCheckpoint();
	}

	m_dwCheckpointMarker =
			CreateRadarMarkerIcon(0,
								  m_vecCheckpointPos.x,
								  m_vecCheckpointPos.y,
								  m_vecCheckpointPos.z, 1005, 0);

	m_bCheckpointsEnabled = true;
}

// 0.3.7
uint32_t CGame::CreateRadarMarkerIcon(int iMarkerType, float fX, float fY, float fZ, int iColor, int iStyle)
{
	uint32_t dwMarkerID = 0;

	if(iStyle == 1) 
		ScriptCommand(&create_marker_icon, fX, fY, fZ, iMarkerType, &dwMarkerID);
	else if(iStyle == 2) 
		ScriptCommand(&create_radar_marker_icon, fX, fY, fZ, iMarkerType, &dwMarkerID);
	else if(iStyle == 3) 
		ScriptCommand(&create_icon_marker_sphere, fX, fY, fZ, iMarkerType, &dwMarkerID);
	else 
		ScriptCommand(&create_radar_marker_without_sphere, fX, fY, fZ, iMarkerType, &dwMarkerID);

	if(iMarkerType == 0)
	{
		if(iColor >= 1004)
		{
			ScriptCommand(&set_marker_color, dwMarkerID, iColor);
			ScriptCommand(&show_on_radar, dwMarkerID, 3);
		}
		else
		{
			ScriptCommand(&set_marker_color, dwMarkerID, iColor);
			ScriptCommand(&show_on_radar, dwMarkerID, 2);
		}
	}

	return dwMarkerID;
}

// 0.3.7
uint8_t CGame::GetActiveInterior()
{
	uint32_t dwRet;
	ScriptCommand(&get_active_interior, &dwRet);
	return (uint8_t)dwRet;
}

// 0.3.7
void CGame::SetWorldTime(int iHour, int iMinute)
{
	*(uint8_t*)(g_libGTASA+0x00953143) = (uint8_t)iMinute;
	*(uint8_t*)(g_libGTASA+0x00953142) = (uint8_t)iHour;
	ScriptCommand(&set_current_time, iHour, iMinute);

	pNetGame->m_byteWorldTime = iHour;
	pNetGame->m_byteWorldMinute = iMinute;
}

// 0.3.7
void CGame::SetWorldWeather(unsigned char byteWeatherID) const
{
	Log("SetWorldWeather");
	//*(unsigned char*)(g_libGTASA+0x9DB98E) = byteWeatherID;

	CHook::CallFunction<void>(g_libGTASA + 0x005CDF88 + 1, byteWeatherID);

//	if(!m_bClockEnabled)
//	{
//		*(uint16_t*)(g_libGTASA+0x9DB990) = byteWeatherID;
//		*(uint16_t*)(g_libGTASA+0x9DB992) = byteWeatherID;
//	}
}

// 0.3.7
void CGame::EnableZoneNames(bool bEnable)
{
	ScriptCommand(&enable_zone_names, bEnable);
}

// ��������
void CGame::PlaySound(int iSound, float fX, float fY, float fZ)
{
	ScriptCommand(&play_sound, fX, fY, fZ, iSound);
}

// 0.3.7
uint8_t CGame::IsModelLoaded(unsigned int iModelID)
{
	return ScriptCommand(&is_model_available, iModelID);
}

// 0.3.7
void CGame::RefreshStreamingAt(float x, float y)
{
	ScriptCommand(&refresh_streaming_at, x, y);
}

// 0.3.7
void CGame::DisableTrainTraffic()
{
	ScriptCommand(&enable_train_traffic,0);
}

// 0.3.7
void CGame::SetMaxStats()
{
	// CCheat::VehicleSkillsCheat
	(( int (*)())(g_libGTASA+0x002FE690 + 1))();

	// CCheat::WeaponSkillsCheat
	(( int (*)())(g_libGTASA+0x002FE62A + 1))();

	// CStats::SetStatValue nop
	CHook::RET(g_libGTASA + 0x0041557C);
}

void CGame::SetWantedLevel(uint8_t byteLevel)
{
	CHUD::iWantedLevel = byteLevel;
	CHUD::UpdateWanted();
}

bool CGame::IsAnimationLoaded(const char szAnimFile[])
{
	return ScriptCommand(&is_animation_loaded, szAnimFile);
}
// 0.3.7
void CGame::DisplayGameText(char* szStr, int iTime, int iType)
{
	ScriptCommand(&text_clear_all);
	CFont::AsciiToGxtChar(szStr, szGameTextMessage);

	// CMessages::AddBigMesssage
	(( void (*)(uint16_t*, int, int))(g_libGTASA+0x0054C62C + 1))(szGameTextMessage, iTime, iType);
}

// 0.3.7
void CGame::SetGravity(float fGravity)
{
//	CHook::UnFuck(g_libGTASA + 0x3A0B64);
//	*(float*)(g_libGTASA+0x3A0B64) = fGravity;
}

void CGame::ToggleCJWalk(bool bUseCJWalk)
{
//	if(bUseCJWalk)
//		CHook::WriteMemory(g_libGTASA + 0x45477E, (uintptr_t)"\xC4\xF8\xDC\x64", 4);
//	else

	CHook::NOP(g_libGTASA + 0x004C5F6A, 2);
}

void CGame::DisableMarker(uint32_t dwMarkerID)
{
	ScriptCommand(&disable_marker, dwMarkerID);
}

// 0.3.7

void CGame::AddToLocalMoney(int iAmmount)
{
	CHUD::iLocalMoney = iAmmount;

	CHUD::UpdateMoney();
}

// 0.3.7
void CGame::DisableInteriorEnterExits()
{
	uintptr_t addr = *(uintptr_t*)(g_libGTASA+0x007A1E20);
	int count = *(uint32_t*)(addr+8);

	addr = *(uintptr_t*)addr;

	for(int i=0; i<count; i++)
	{
		*(uint16_t*)(addr+0x30) = 0;
		addr += 0x3C;
	}
}

extern uint8_t bGZ;
void CGame::DrawGangZone(float fPos[], uint32_t dwColor)
{
    (( void (*)(float*, uint32_t*, uint8_t))(g_libGTASA+0x00443C60 + 1))(fPos, &dwColor, bGZ);
}




