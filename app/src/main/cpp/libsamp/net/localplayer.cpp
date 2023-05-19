#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

#include "../game/scripting.h"

#include "../game/common.h"
#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "../CSettings.h"
#include "../util/CJavaWrapper.h"
#include "java_systems/CHUD.h"
#include "java_systems/CInventory.h"
#include "game/StreamingInfo.h"

extern CGame *pGame;

bool bFirstSpawn = true;

extern int iNetModeNormalOnfootSendRate;
extern int iNetModeNormalInCarSendRate;
extern bool bUsedPlayerSlots[];

#define IS_TARGETING(x) ((x) & 128)
#define IS_FIRING(x) ((x) & 4)

// SEND RATE TICKS
#define NETMODE_IDLE_ONFOOT_SENDRATE	80
#define NETMODE_NORMAL_ONFOOT_SENDRATE	30
#define NETMODE_IDLE_INCAR_SENDRATE		80
#define NETMODE_NORMAL_INCAR_SENDRATE	30

#define NETMODE_HEADSYNC_SENDRATE		1000
#define NETMODE_AIM_SENDRATE			100
#define NETMODE_FIRING_SENDRATE			30

#define LANMODE_IDLE_ONFOOT_SENDRATE	20
#define LANMODE_NORMAL_ONFOOT_SENDRATE	15
#define LANMODE_IDLE_INCAR_SENDRATE		30
#define LANMODE_NORMAL_INCAR_SENDRATE	15

#define STATS_UPDATE_TICKS				1000

CLocalPlayer::CLocalPlayer()
{
	m_pPlayerPed = pGame->FindPlayerPed();
	m_bIsActive = false;
	m_bIsWasted = false;

	m_iSelectedClass = 0;
	m_bHasSpawnInfo = false;
	m_bWaitingForSpawnRequestReply = false;
	m_bWantsAnotherClass = false;
	m_bInRCMode = false;

	memset(&m_OnFootData, 0, sizeof(ONFOOT_SYNC_DATA));

	m_dwLastSendTick = GetTickCount();
	m_dwLastSendAimTick = GetTickCount();
	m_dwLastSendSpecTick = GetTickCount();
	m_dwLastUpdateHudButtons = GetTickCount();
	m_dwLastAimSendTick = m_dwLastSendTick;
	m_dwLastUpdateOnFootData = GetTickCount();
	m_dwLastStatsUpdateTick = GetTickCount();
	m_dwLastUpdateInCarData = GetTickCount();
	m_dwLastUpdatePassengerData = GetTickCount();
	m_dwPassengerEnterExit = GetTickCount();

	m_CurrentVehicle = INVALID_VEHICLE_ID;
	ResetAllSyncAttributes();

	m_bIsSpectating = false;
	m_byteSpectateType = SPECTATE_TYPE_NONE;
	m_SpectateID = 0xFFFFFFFF;
	FindDeathReasonPlayer = 0;

	uint8_t i;
	for (i = 0; i < 13; i++)
	{
		m_byteLastWeapon[i] = 0;
		m_dwLastAmmo[i] = 0;
	}
}

CLocalPlayer::~CLocalPlayer()
= default;

void CLocalPlayer::ResetAllSyncAttributes()
{
	m_byteCurInterior = 0;
	m_LastVehicle = INVALID_VEHICLE_ID;
	m_bInRCMode = false;
}

void CLocalPlayer::SendStatsUpdate()
{
	RakNet::BitStream bsStats;

//	uint32_t wAmmo = m_pPlayerPed->GetCurrentWeaponSlot()->dwAmmo;

	bsStats.Write((BYTE)ID_STATS_UPDATE);
	bsStats.Write(CHUD::iLocalMoney);
	//bsStats.Write(wAmmo);
	bsStats.Write(m_pPlayerPed->drunk_level);
	pNetGame->GetRakClient()->Send(&bsStats, HIGH_PRIORITY, UNRELIABLE, 0);
}

void CLocalPlayer::CheckWeapons()
{
	if (m_pPlayerPed->IsInVehicle()) return;

	//uint8_t byteCurWep = m_pPlayerPed->GetCurrentWeapon();
	bool bMSend = false;

	for (int i = 0; i < MAX_WEAPONS_SLOT; i++) {

		if (m_byteLastWeapon[i] != m_pPlayerPed->m_pPed->WeaponSlots[i].dwType ||
                m_dwLastAmmo[i] != m_pPlayerPed->m_pPed->WeaponSlots[i].dwAmmo)
		{
			m_byteLastWeapon[i] = m_pPlayerPed->m_pPed->WeaponSlots[i].dwType;
			m_dwLastAmmo[i] = m_pPlayerPed->m_pPed->WeaponSlots[i].dwAmmo;

            bMSend = true;
			break;
		}

	}
	if (bMSend) {
		RakNet::BitStream bsWeapons;
		bsWeapons.Write((BYTE) ID_WEAPONS_UPDATE);

		for (int i = 0; i < MAX_WEAPONS_SLOT; i++) {

			bsWeapons.Write((uint8_t) i);
			bsWeapons.Write((uint8_t) m_byteLastWeapon[i]);
			bsWeapons.Write((uint16_t) m_dwLastAmmo[i]);
		}
		pNetGame->GetRakClient()->Send(&bsWeapons, HIGH_PRIORITY, RELIABLE, 0);
	}
}
uint32_t CLocalPlayer::GetCurrentAnimationIndexFlag()
{
	uint32_t dwAnim = 0;

	float fBlendData = 4.0f;

	int iAnimIdx = m_pPlayerPed->GetCurrentAnimationIndex(fBlendData);

	uint32_t hardcodedBlend = 0b00000100;	// 4
	hardcodedBlend <<= 16;

	uint32_t hardcodedFlags = 0;

	if (iAnimIdx)
	{
		hardcodedFlags = 0b00010001;
	}
	else
	{
		hardcodedFlags = 0b10000000;
		iAnimIdx = 1189;
	}

	hardcodedFlags <<= 24;

	uint16_t usAnimidx = (uint16_t)iAnimIdx;

	dwAnim = (uint32_t)usAnimidx;
	dwAnim |= hardcodedBlend;
	dwAnim |= hardcodedFlags;

	return dwAnim;
}
extern bool g_uiHeadMoveEnabled;

#include "..//game/CWeaponsOutFit.h"
#include "java_systems/CEditobject.h"
#include "java_systems/casino/CChip.h"
#include "java_systems/CAucContainer.h"
#include "util/patch.h"
#include "java_systems/CAdminRecon.h"
#include "java_systems/CMedic.h"
#include "java_systems/CTab.h"
#include "java_systems/CDailyReward.h"
#include "java_systems/CDialog.h"
#include "java_systems/CTechInspect.h"
#include "java_systems/casino/CBaccarat.h"
#include "game/CWorld.h"

CAMERA_AIM* caAim = new CAMERA_AIM();

bool CLocalPlayer::Process()
{
	m_pPlayerPed->SetCurrentAim(caAim);
	caAim = m_pPlayerPed->GetCurrentAim();

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	uint32_t dwThisTick = GetTickCount();

	if(m_bIsActive && m_pPlayerPed) {
		if (m_pPlayerPed->drunk_level) {
			m_pPlayerPed->drunk_level--;
			ScriptCommand(&SET_PLAYER_DRUNKENNESS, m_pPlayerPed->m_bytePlayerNumber,
						  m_pPlayerPed->drunk_level / 100);
		}
		// handle dead
		if (!m_bIsWasted && m_pPlayerPed->GetActionTrigger() == ACTION_DEATH ||
			m_pPlayerPed->IsDead()) {
			ToggleSpectating(false);
			m_pPlayerPed->FlushAttach();
			// reset tasks/anims
			m_pPlayerPed->TogglePlayerControllable(true);

			if (m_pPlayerPed->IsAPassenger()) {

				SendInCarFullSyncData();
				m_LastVehicle = pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(
						m_pPlayerPed->GetGtaVehicle());
			}

			//m_pPlayerPed->SetHealth(0.0f);
			m_pPlayerPed->SetDead();
			SendWastedNotification();
			Spawn();
			m_bIsActive = false;
			m_bIsWasted = true;

			return true;
		}

		if ((dwThisTick - m_dwLastStatsUpdateTick) > STATS_UPDATE_TICKS) {
			SendStatsUpdate();
			m_dwLastStatsUpdateTick = dwThisTick;
		}

		CheckWeapons();
	//	CWeaponsOutFit::ProcessLocalPlayer(m_pPlayerPed);

		m_pPlayerPed->ProcessSpecialAction(m_pPlayerPed->m_iCurrentSpecialAction);


		// handle interior changing
		uint8_t byteInterior = pGame->GetActiveInterior();
		if (byteInterior != m_byteCurInterior)
			UpdateRemoteInterior(byteInterior);

		// The new regime for adjusting sendrates is based on the number
		// of players that will be effected by this update. The more players
		// there are within a small radius, the more we must scale back
		// the number of sends.
		int iNumberOfPlayersInLocalRange = 0;
		iNumberOfPlayersInLocalRange = DetermineNumberOfPlayersInLocalRange();
		if (!iNumberOfPlayersInLocalRange) iNumberOfPlayersInLocalRange = 10;

		// SPECTATING
		if (m_bIsSpectating) {
			ProcessSpectating();
		}

		// IN VEHICLE
		if (m_pPlayerPed->m_pPed->bInVehicle)
		{
			if(m_pPlayerPed->IsAPassenger()) {
				if ((dwThisTick - m_dwLastSendTick) > (unsigned int) GetOptimumInCarSendRate()) {
					m_dwLastSendTick = GetTickCount();
					SendPassengerFullSyncData();
				}
			}
			else {
				if (pVehiclePool)
					m_CurrentVehicle = m_pPlayerPed->GetCurrentSampVehicleID();

				if ((dwThisTick - m_dwLastSendTick) > (unsigned int) GetOptimumInCarSendRate()) {
					m_dwLastSendTick = GetTickCount();
					SendInCarFullSyncData();
				}
			}
		}
		else  { // ONFOOT
			UpdateSurfing();

			if ((dwThisTick - m_dwLastHeadUpdate) > 1000 && g_uiHeadMoveEnabled) {
				CVector LookAt;
				CAMERA_AIM *Aim = GameGetInternalAim();
				LookAt.x = Aim->pos1x + (Aim->f1x * 20.0f);
				LookAt.y = Aim->pos1y + (Aim->f1y * 20.0f);
				LookAt.z = Aim->pos1z + (Aim->f1z * 20.0f);

				//	ScriptCommand(&TASK_LOOK_AT_COORD, m_pPlayerPed->m_dwGTAId, LookAt.x, LookAt.y, LookAt.z, 3000);
				pGame->FindPlayerPed()->ApplyCommandTask("FollowPedSA", 0, 2000, -1, &LookAt, 0,
														 0.1f, 500, 3, 0);
				m_dwLastHeadUpdate = dwThisTick;
			}

			if (m_CurrentVehicle != INVALID_VEHICLE_ID) {
				m_LastVehicle = m_CurrentVehicle;
				m_CurrentVehicle = INVALID_VEHICLE_ID;
			}

			if ((dwThisTick - m_dwLastSendTick) > (unsigned int) GetOptimumOnFootSendRate()) {
				m_dwLastSendTick = GetTickCount();
				SendOnFootFullSyncData();
			}

			if ((dwThisTick - m_dwLastSendTick) > (unsigned int) GetOptimumOnFootSendRate() ||
				LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK] ||
				LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] ||
				LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] ||
				LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK]) {

				m_dwLastSendTick = GetTickCount();
				// Log("[DEBUG] Send Packet Key RPC");
				SendOnKeyFullSyncData();

			}
			// TIMING FOR ONFOOT AIM SENDS
			uint16_t lrAnalog, udAnalog;
			uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

			// Not targeting or firing. We need a very slow rate to sync the head.
			if (!IS_TARGETING(wKeys) && !IS_FIRING(wKeys)) {
				if ((dwThisTick - m_dwLastAimSendTick) > NETMODE_HEADSYNC_SENDRATE) {
					m_dwLastAimSendTick = dwThisTick;
					SendAimSyncData();
				}
			}

				// Targeting only. Just synced for show really, so use a slower rate
			else if (IS_TARGETING(wKeys) && !IS_FIRING(wKeys)) {
				if ((dwThisTick - m_dwLastAimSendTick) >
					(uint32_t) NETMODE_AIM_SENDRATE + (iNumberOfPlayersInLocalRange)) {
					m_dwLastAimSendTick = dwThisTick;
					SendAimSyncData();
				}
			}

				// Targeting and Firing. Needs a very accurate send rate.
			else if (IS_TARGETING(wKeys) && IS_FIRING(wKeys)) {
				if ((dwThisTick - m_dwLastAimSendTick) >
					(uint32_t) NETMODE_FIRING_SENDRATE + (iNumberOfPlayersInLocalRange)) {
					m_dwLastAimSendTick = dwThisTick;
					SendAimSyncData();
				}
			}

				// Firing without targeting. Needs a normal onfoot sendrate.
			else if (!IS_TARGETING(wKeys) && IS_FIRING(wKeys)) {
				if ((dwThisTick - m_dwLastAimSendTick) > (uint32_t) GetOptimumOnFootSendRate()) {
					m_dwLastAimSendTick = dwThisTick;
					SendAimSyncData();
				}
			}
		}
			// PASSENGER

		if ((dwThisTick - m_dwLastUpdateHudButtons) > 100) {
			m_dwLastUpdateHudButtons = GetTickCount();
			//  нопки вход/выход/закрыть машину
			if (!m_pPlayerPed->lToggle ||
				m_pPlayerPed->m_iCurrentSpecialAction == SPECIAL_ACTION_CARRY) {
				if (CHUD::bIsShowPassengerButt) {
					CHUD::togglePassengerButton(false);
				}
				if (CHUD::bIsShowEnterExitButt) {
					CHUD::toggleEnterExitButton(false);
				}
				if (CHUD::bIsShowLockButt) {
					CHUD::toggleLockButton(false);
				}
			} else if (!m_pPlayerPed->IsInVehicle()) {
				if (CHUD::bIsShowHornButt) {
					CHUD::toggleHornButton(false);
				}
				if(CHUD::bIsShowSirenButt) {
					CHUD::toggleSirenButton(false);
				}
				if (pVehiclePool) {
					auto pVehicle = pVehiclePool->FindNearestToLocalPlayerPed();

					if (pVehicle != nullptr) {

						if (pVehicle->GetDistanceFromLocalPlayerPed() < 4.0f  && !pVehicle->IsTrailer()) {
							//if(!pVehicle->m_bIsLocked)
							if (!pVehicle->m_bIsLocked) {// тачка открыта
								if (!CHUD::bIsShowPassengerButt) {
									CHUD::togglePassengerButton(true);
								}
								if (!CHUD::bIsShowEnterExitButt) {
									CHUD::toggleEnterExitButton(true);
								}
							} else {
								if (CHUD::bIsShowPassengerButt) {
									CHUD::togglePassengerButton(false);
								}
								if (CHUD::bIsShowEnterExitButt) {
									CHUD::toggleEnterExitButton(false);
								}
							}
							if (!CHUD::bIsShowLockButt) {
								CHUD::toggleLockButton(true);
							}

						} else {
							if (CHUD::bIsShowPassengerButt) {
								CHUD::togglePassengerButton(false);
							}
							if (CHUD::bIsShowEnterExitButt) {
								CHUD::toggleEnterExitButton(false);
							}
							if (CHUD::bIsShowLockButt) {
								CHUD::toggleLockButton(false);
							}
						}
					} else {
						if (CHUD::bIsShowPassengerButt) {
							CHUD::togglePassengerButton(false);
						}
						if (CHUD::bIsShowEnterExitButt) {
							CHUD::toggleEnterExitButton(false);
						}
						if (CHUD::bIsShowLockButt) {
							CHUD::toggleLockButton(false);
						}
					}
				}

			} else {// в машине
				if (m_pPlayerPed->IsAPassenger()) {// на пассажирке
					if (!CHUD::bIsShowEnterExitButt) {
						CHUD::toggleEnterExitButton(true);
					}
					if (CHUD::bIsShowLockButt) {
						CHUD::toggleLockButton(false);
					}
					if (CHUD::bIsShowHornButt) {
						CHUD::toggleHornButton(false);
					}
					if(CHUD::bIsShowSirenButt) {
						CHUD::toggleSirenButton(false);
					}
					if (CHUD::bIsShowPassengerButt) {
						CHUD::togglePassengerButton(false);
					}
				} else {
					if (!CHUD::bIsShowEnterExitButt) {
						CHUD::toggleEnterExitButton(true);
					}

					if (!CHUD::bIsShowLockButt) {
						CHUD::toggleLockButton(true);
					}
					if (!CHUD::bIsShowHornButt) {
						CHUD::toggleHornButton(true);
					}
					if(!CHUD::bIsShowSirenButt) {
						if(m_pPlayerPed->GetCurrentVehicle()->m_bHasSiren) {
							CHUD::toggleSirenButton(true);
						}
					}
					if (CHUD::bIsShowPassengerButt) {
						CHUD::togglePassengerButton(false);
					}
				}
			}
		}
	}
	////////////////////////////
	bool needDrawableHud = true;
	if(CDialog::bIsShow || pGame->isCasinoDiceActive || CTab::bIsShow || pGame->isAutoShopActive
	   || pGame->isCasinoWheelActive || !m_pPlayerPed || pGame->isRegistrationActive || pGame->isShopStoreActive ||
	   CMedic::bIsShow || CInventory::bIsToggle || bFirstSpawn || CEditobject::bIsToggle || CChip::bIsShow
	   || CAucContainer::bIsShow || CAdminRecon::bIsToggle || CHUD::bIsCamEditGui || CDailyReward::bIsShow ||
	   CTechInspect::bIsShow || CBaccarat::bIsShow)
	{
		needDrawableHud = false;
	}

	CHUD::toggleAll(needDrawableHud);

    if(m_bIsSpectating && !m_bIsActive)
    {
        ProcessSpectating();
        return true;
    }

    // handle needs to respawn
    if(m_bIsWasted && (m_pPlayerPed->GetActionTrigger() != ACTION_WASTED) && (m_pPlayerPed->GetActionTrigger() != ACTION_DEATH) )
    {
        if( m_bClearedToSpawn && !m_bWantsAnotherClass && pNetGame->GetGameState() == GAMESTATE_CONNECTED)
        {
            if(m_pPlayerPed->GetHealth() > 0.0f)
                Spawn();
        }
        else
        {
            m_bIsWasted = false;
            HandleClassSelection();
            m_bWantsAnotherClass = false;
        }
        return true;
    }
    return true;
}

void CLocalPlayer::GiveTakeDamage(bool bGiveOrTake, uint16_t wPlayerID, float damage_amount, uint32_t weapon_id, uint32_t bodypart)
{
	RakNet::BitStream bitStream;

	bitStream.Write((bool)bGiveOrTake);
	bitStream.Write((uint16_t)wPlayerID);
	bitStream.Write((float)damage_amount);
	bitStream.Write((uint32_t)weapon_id);

	if(bodypart < 3 || bodypart > 9) bodypart = 5; // omp fix?

	bitStream.Write((uint32_t)bodypart);

	FindDeathReasonPlayer = weapon_id;

	// pChatWindow->AddDebugMessage("Id: %d, Weapon: %d, Damage: %d", wPlayerID, weapon_id, damage_amount);

	pNetGame->GetRakClient()->RPC(&RPC_PlayerGiveTakeDamage, &bitStream, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::SendWastedNotification()
{
	RakNet::BitStream bsPlayerDeath;
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if(pPlayerPool)
	{
		PLAYERID WhoWasResponsible = m_pPlayerPed->FindDeathResponsiblePlayer();
		bsPlayerDeath.Write(FindDeathReasonPlayer);
		bsPlayerDeath.Write(WhoWasResponsible);
		pNetGame->GetRakClient()->RPC(&RPC_Death, &bsPlayerDeath, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
		Log("SendWastedNotification");
	}
//
}

void CLocalPlayer::GoEnterVehicle(bool passenger)
{
	if (GetTickCount() - m_dwPassengerEnterExit < 1000)
		return;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	// CTouchInterface::IsHoldDown

	auto pVehicle = pVehiclePool->FindNearestToLocalPlayerPed();
	if (pVehicle != nullptr)
	{
		if (pVehicle != nullptr && pVehicle->GetDistanceFromLocalPlayerPed() < 4.0f)
		{
			m_pPlayerPed->EnterVehicle(pVehicle->m_dwGTAId, passenger);
			SendEnterVehicleNotification(pVehicle->getSampId(), passenger);
			m_dwPassengerEnterExit = GetTickCount();
		}
	}

}

void CLocalPlayer::HandleClassSelection()
{
	m_bClearedToSpawn = false;

	if(m_pPlayerPed)
	{
		m_pPlayerPed->SetInitialState();
		m_pPlayerPed->SetHealth(100.0f);
		m_pPlayerPed->TogglePlayerControllable(false);
	}
	
	RequestClass(m_iSelectedClass);

	return;
}

// 0.3.7
void CLocalPlayer::HandleClassSelectionOutcome()
{
	if(m_pPlayerPed)
	{
		m_pPlayerPed->ClearAllWeapons();
		m_pPlayerPed->SetModelIndex(m_SpawnInfo.iSkin);
	}

	m_bClearedToSpawn = true;
}

void CLocalPlayer::SendNextClass()
{
	RwMatrix matPlayer;
	m_pPlayerPed->GetMatrix(&matPlayer);

	if(m_iSelectedClass == (pNetGame->m_iSpawnsAvailable - 1)) m_iSelectedClass = 0;
	else m_iSelectedClass++;

	pGame->PlaySound(1052, matPlayer.pos.x, matPlayer.pos.y, matPlayer.pos.z);
	RequestClass(m_iSelectedClass);
}

void CLocalPlayer::SendPrevClass()
{
	RwMatrix matPlayer;
	m_pPlayerPed->GetMatrix(&matPlayer);
	
	if(m_iSelectedClass == 0) m_iSelectedClass = (pNetGame->m_iSpawnsAvailable - 1);
	else m_iSelectedClass--;		

	pGame->PlaySound(1053,matPlayer.pos.x,matPlayer.pos.y,matPlayer.pos.z);
	RequestClass(m_iSelectedClass);
}

void CLocalPlayer::SendSpawn()
{
	RequestSpawn();
	m_bWaitingForSpawnRequestReply = true;
}

void CLocalPlayer::RequestClass(int iClass)
{
	RakNet::BitStream bsSpawnRequest;
	bsSpawnRequest.Write(iClass);
	pNetGame->GetRakClient()->RPC(&RPC_RequestClass, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID,
                                  nullptr);
}

void CLocalPlayer::RequestSpawn()
{
	RakNet::BitStream bsSpawnRequest;
	pNetGame->GetRakClient()->RPC(&RPC_RequestSpawn, &bsSpawnRequest, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}

uint32_t CLocalPlayer::GetPlayerColorAsARGB()
{
	return (TranslateColorCodeToRGBA(pNetGame->GetPlayerPool()->GetLocalPlayerID()) >> 8) | 0xFF000000;
}



void CLocalPlayer::UpdateSurfing() {}

void CLocalPlayer::SendEnterVehicleNotification(VEHICLEID VehicleID, bool bPassenger)
{
	RakNet::BitStream bsSend;

	bsSend.Write(VehicleID);
	bsSend.Write((uint8_t)bPassenger);

	pNetGame->GetRakClient()->RPC(&RPC_EnterVehicle, &bsSend, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0,false, UNASSIGNED_NETWORK_ID, nullptr);
}

void CLocalPlayer::SendExitVehicleNotification(VEHICLEID VehicleID)
{
	RakNet::BitStream bsSend;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);

	if(pVehicle)
	{ 
		if (!m_pPlayerPed->IsAPassenger()) 
			m_LastVehicle = VehicleID;

		bsSend.Write(VehicleID);
		pNetGame->GetRakClient()->RPC(&RPC_ExitVehicle,&bsSend,HIGH_PRIORITY,RELIABLE_SEQUENCED,0,false, UNASSIGNED_NETWORK_ID, NULL);
    }
}

void CLocalPlayer::UpdateRemoteInterior(uint8_t byteInterior)
{
	Log("CLocalPlayer::UpdateRemoteInterior %d", byteInterior);

	m_byteCurInterior = byteInterior;
	RakNet::BitStream bsUpdateInterior;
	bsUpdateInterior.Write(byteInterior);
	pNetGame->GetRakClient()->RPC(&RPC_SetInteriorId, &bsUpdateInterior, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void CLocalPlayer::SetSpawnInfo(PLAYER_SPAWN_INFO *pSpawn)
{
	memcpy(&m_SpawnInfo, pSpawn, sizeof(PLAYER_SPAWN_INFO));
	m_bHasSpawnInfo = true;
}

bool CLocalPlayer::Spawn()
{
	Log("CLocalPlayer::Spawn");
//	if(!m_bHasSpawnInfo) return false;

	m_pPlayerPed->m_pPed->m_nPedState = PED_NONE;
	*(uint8_t*)(g_libGTASA + 0x007A1F11) = 0;

	m_pPlayerPed->SetInterior(0);

    //g_pJavaWrapper->ShowSpeed();

	//pGame->DisplayHUD(true);

	CCamera *pGameCamera = pGame->GetCamera();
	pGameCamera->Restore();
	pGameCamera->SetBehindPlayer();
	//pGame->DisplayHUD(true);
	m_pPlayerPed->TogglePlayerControllable(true);
	
	if(!bFirstSpawn)
		m_pPlayerPed->SetInitialState();
	else
		bFirstSpawn = false;

	bFirstSpawn = false;

	pGame->RefreshStreamingAt(m_SpawnInfo.vecPos.x,m_SpawnInfo.vecPos.y);

	m_pPlayerPed->RestartIfWastedAt(&m_SpawnInfo.vecPos, m_SpawnInfo.fRotation);
	m_pPlayerPed->SetModelIndex(m_SpawnInfo.iSkin);
	m_pPlayerPed->ClearAllWeapons();
	m_pPlayerPed->ResetDamageEntity();

//	pGame->DisableTrainTraffic();

	// CCamera::Fade
//	CHook::WriteMemory(g_libGTASA + 0x36EA2C, "\x70\x47", 2); // bx lr

	m_pPlayerPed->TeleportTo(m_SpawnInfo.vecPos.x,
		m_SpawnInfo.vecPos.y, (m_SpawnInfo.vecPos.z + 0.5f));

	m_pPlayerPed->ForceTargetRotation(m_SpawnInfo.fRotation);

	m_bIsWasted = false;
	m_bIsActive = true;
	m_bWaitingForSpawnRequestReply = false;

	RakNet::BitStream bsSendSpawn;
	pNetGame->GetRakClient()->RPC(&RPC_Spawn, &bsSendSpawn, SYSTEM_PRIORITY,
		RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);

	return true;
}

uint32_t CLocalPlayer::GetPlayerColor()
{
	return TranslateColorCodeToRGBA(pNetGame->GetPlayerPool()->GetLocalPlayerID());
}

void CLocalPlayer::SetPlayerColor(uint32_t dwColor)
{
	SetRadarColor(pNetGame->GetPlayerPool()->GetLocalPlayerID(), dwColor);
}

int CLocalPlayer::GetOptimumOnFootSendRate()
{
	if(!m_pPlayerPed) return 1000;

	return (iNetModeNormalOnfootSendRate + DetermineNumberOfPlayersInLocalRange());
}

int CLocalPlayer::GetOptimumInCarSendRate()
{
	if(!m_pPlayerPed) return 1000;

	return (iNetModeNormalInCarSendRate + DetermineNumberOfPlayersInLocalRange());
}

uint8_t CLocalPlayer::DetermineNumberOfPlayersInLocalRange()
{
	int iNumPlayersInRange = 0;
	for(int i=2; i < PLAYER_PED_SLOTS; i++)
		if(bUsedPlayerSlots[i]) iNumPlayersInRange++;

	return iNumPlayersInRange;
}
#include "..//chatwindow.h"

void CLocalPlayer::SendOnKeyFullSyncData()
{
	RakNet::BitStream bsPlayerSync;
	//RwMatrix matPlayer;


	uint8_t exKeys = GetPlayerPed()->GetExtendedKeys();
}


void CLocalPlayer::SendOnFootFullSyncData()
{
	RakNet::BitStream bsPlayerSync;

	uint16_t lrAnalog, udAnalog;
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

	ONFOOT_SYNC_DATA ofSync;
	auto pMat = m_pPlayerPed->m_pPed->mat;

	ofSync.lrAnalog = lrAnalog;
	ofSync.udAnalog = udAnalog;
	ofSync.wKeys = wKeys;

	ofSync.vecPos = pMat->GetPosition();

	ofSync.quat.Set(pMat->ToRwMatrix());
	ofSync.quat.Normalise();

	if( FloatOffset(ofSync.quat.w, m_OnFootData.quat.w) < 0.00001 &&
		FloatOffset(ofSync.quat.x, m_OnFootData.quat.x) < 0.00001 &&
		FloatOffset(ofSync.quat.y, m_OnFootData.quat.y) < 0.00001 &&
		FloatOffset(ofSync.quat.z, m_OnFootData.quat.z) < 0.00001)
	{
		ofSync.quat = m_OnFootData.quat;
	}

	ofSync.byteHealth = (uint8_t)m_pPlayerPed->GetHealth();
	ofSync.byteArmour = (uint8_t)m_pPlayerPed->GetArmour();

	uint8_t exKeys = GetPlayerPed()->GetExtendedKeys();
	ofSync.byteCurrentWeapon = m_pPlayerPed->GetCurrentWeapon();
	//ofSync.byteCurrentWeapon = (exKeys << 6) | ofSync.byteCurrentWeapon & 0x3F;
	//ofSync.byteCurrentWeapon ^= (ofSync.byteCurrentWeapon ^ GetPlayerPed()->GetCurrentWeapon()) & 0x3F;

	if (m_pPlayerPed->IsCrouching())
	{
		ofSync.byteSpecialAction = SPECIAL_ACTION_DUCK;
	}
	else
	{
		ofSync.byteSpecialAction = m_pPlayerPed->m_iCurrentSpecialAction;
	}

	ofSync.vecMoveSpeed = m_pPlayerPed->m_pPed->vecMoveSpeed;

	ofSync.vecSurfOffsets.x = 0.0f;
	ofSync.vecSurfOffsets.y = 0.0f;
	ofSync.vecSurfOffsets.z = 0.0f;
	ofSync.wSurfInfo = 0;

	ofSync.dwAnimation = 0;

	if( (GetTickCount() - m_dwLastUpdateOnFootData) > 500 || memcmp(&m_OnFootData, &ofSync, sizeof(ONFOOT_SYNC_DATA)))
	{
		m_dwLastUpdateOnFootData = GetTickCount();

		bsPlayerSync.Write((uint8_t)ID_PLAYER_SYNC);
		bsPlayerSync.Write((char*)&ofSync, sizeof(ONFOOT_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsPlayerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		memcpy(&m_OnFootData, &ofSync, sizeof(ONFOOT_SYNC_DATA));
	}
}

void CLocalPlayer::SendBulletSyncData(PLAYERID byteHitID, uint8_t byteHitType, CVector vecHitPos)
{
	if (!m_pPlayerPed) return;
	switch (byteHitType)
	{
		case BULLET_HIT_TYPE_NONE:
			break;
		case BULLET_HIT_TYPE_PLAYER:
			if (!pNetGame->GetPlayerPool()->m_pPlayers[byteHitID]) return;
			break;

	}
	uint8_t byteCurrWeapon = m_pPlayerPed->GetCurrentWeapon(), byteShotWeapon;

	RwMatrix matPlayer;
	BULLET_SYNC blSync;

	m_pPlayerPed->GetMatrix(&matPlayer);

	blSync.PlayerID = byteHitID;
	blSync.byteHitType = byteHitType;

	if (byteHitType == BULLET_HIT_TYPE_PLAYER)
	{
		float fDistance = pNetGame->GetPlayerPool()->GetAt((PLAYERID)byteHitID)->GetPlayerPed()->GetDistanceFromLocalPlayerPed();
		if (byteCurrWeapon != 0 && fDistance < 1.0f)
			byteShotWeapon = 0;
		else
			byteShotWeapon = byteCurrWeapon;
	}
	else
	{
		byteShotWeapon = m_pPlayerPed->GetCurrentWeapon();
	}
	blSync.byteWeaponID = byteShotWeapon;

	blSync.vecOrigin.x = vecHitPos.x;
	blSync.vecOrigin.y = vecHitPos.y;
	blSync.vecOrigin.z = vecHitPos.z;

	blSync.vecOffset.x = blSync.vecOffset.y = blSync.vecOffset.z = 0.0f;

	RakNet::BitStream bsBulletSync;
	bsBulletSync.Write((uint8_t)ID_BULLET_SYNC);
	bsBulletSync.Write((const char*)& blSync, sizeof(BULLET_SYNC));
	pNetGame->GetRakClient()->Send(&bsBulletSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void CLocalPlayer::SendInCarFullSyncData()
{
	RakNet::BitStream bsVehicleSync;
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(!pVehiclePool) return;
	if(!m_pPlayerPed || !m_pPlayerPed->m_pPed)return;

	RwMatrix matPlayer;
	CVector vecMoveSpeed;

	uint16_t lrAnalog, udAnalog;
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);

	CVehicle *pVehicle = m_pPlayerPed->GetCurrentVehicle();
	if(!pVehicle || !pVehicle->m_pVehicle)return;

	INCAR_SYNC_DATA icSync;
	memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));

	VEHICLEID vehicleid = m_pPlayerPed->GetCurrentSampVehicleID();
	if(vehicleid == INVALID_VEHICLE_ID)return;

	icSync.VehicleID = vehicleid;

	icSync.lrAnalog = lrAnalog;
	icSync.udAnalog = udAnalog;
	icSync.wKeys = wKeys;

	pVehicle->GetMatrix(&matPlayer);
	pVehicle->GetMoveSpeedVector(&vecMoveSpeed);

	icSync.quat.Set(&matPlayer);
	icSync.quat.Normalise();

	if(	FloatOffset(icSync.quat.w, m_InCarData.quat.w) < 0.00001 &&
		FloatOffset(icSync.quat.x, m_InCarData.quat.x) < 0.00001 &&
		FloatOffset(icSync.quat.y, m_InCarData.quat.y) < 0.00001 &&
		FloatOffset(icSync.quat.z, m_InCarData.quat.z) < 0.00001)
	{
		icSync.quat = m_InCarData.quat;
	}

	// pos
	icSync.vecPos.x = matPlayer.pos.x;
	icSync.vecPos.y = matPlayer.pos.y;
	icSync.vecPos.z = matPlayer.pos.z;
	// move speed
	icSync.vecMoveSpeed.x = vecMoveSpeed.x;
	icSync.vecMoveSpeed.y = vecMoveSpeed.y;
	icSync.vecMoveSpeed.z = vecMoveSpeed.z;

	icSync.fCarHealth = pVehicle->GetHealth();
	icSync.bytePlayerHealth = (uint8_t)m_pPlayerPed->GetHealth();
	icSync.bytePlayerArmour = (uint8_t)m_pPlayerPed->GetArmour();

	icSync.byteSirenOn = pVehicle->m_bIsSirenOn;

	icSync.HydraThrustAngle = pVehicle->m_iTurnState;

	icSync.TrailerID = 0;

	auto pTrailer = pVehicle->m_pVehicle->pTrailer;
	if (pTrailer != nullptr)
	{
		uint16_t trailerId = pNetGame->GetVehiclePool()->findSampIdFromGtaPtr(pTrailer);

		if (ScriptCommand(&is_trailer_on_cab, trailerId, pVehicle->m_dwGTAId)) {
			icSync.TrailerID = trailerId;
		}
	}
    if (icSync.TrailerID && icSync.TrailerID < MAX_VEHICLES)
	{
		RwMatrix matTrailer;
		TRAILER_SYNC_DATA trSync;
		CVehicle* pTrailer = pVehiclePool->GetAt(icSync.TrailerID);

		if (pTrailer && pTrailer->m_pVehicle)
		{
			pTrailer->GetMatrix(&matTrailer);
			trSync.trailerID = icSync.TrailerID;

			trSync.vecPos.x = matTrailer.pos.x;
			trSync.vecPos.y = matTrailer.pos.y;
			trSync.vecPos.z = matTrailer.pos.z;

			CQuaternion syncQuat;
			syncQuat.Set(&matTrailer);
			trSync.quat = syncQuat;

			trSync.vecMoveSpeed = pTrailer->m_pVehicle->vecMoveSpeed;
			trSync.vecTurnSpeed = pTrailer->m_pVehicle->m_vecTurnSpeed;

			RakNet::BitStream bsTrailerSync;
			bsTrailerSync.Write((BYTE)ID_TRAILER_SYNC);
			bsTrailerSync.Write((char*)& trSync, sizeof(TRAILER_SYNC_DATA));
			pNetGame->GetRakClient()->Send(&bsTrailerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
		}
	}
	// send
	if( (GetTickCount() - m_dwLastUpdateInCarData) > 500 || memcmp(&m_InCarData, &icSync, sizeof(INCAR_SYNC_DATA))) {
		m_dwLastUpdateInCarData = GetTickCount();
		bsVehicleSync.Write((uint8_t) ID_VEHICLE_SYNC);
		bsVehicleSync.Write((char *) &icSync, sizeof(INCAR_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsVehicleSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		memcpy(&m_InCarData, &icSync, sizeof(INCAR_SYNC_DATA));
	}
}

void CLocalPlayer::SendPassengerFullSyncData()
{
	RakNet::BitStream bsPassengerSync;

	uint16_t lrAnalog, udAnalog;
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);
	PASSENGER_SYNC_DATA psSync;
	RwMatrix mat;

	psSync.VehicleID = m_pPlayerPed->GetCurrentSampVehicleID();

	if(psSync.VehicleID == INVALID_VEHICLE_ID) return;

	psSync.lrAnalog = lrAnalog;
	psSync.udAnalog = udAnalog;
	psSync.wKeys = wKeys;
	psSync.bytePlayerHealth = (uint8_t)m_pPlayerPed->GetHealth();
	psSync.bytePlayerArmour = (uint8_t)m_pPlayerPed->GetArmour();

	psSync.byteSeatFlags = m_pPlayerPed->GetVehicleSeatID();

	psSync.byteDriveBy = 0;//m_bPassengerDriveByMode;

	psSync.byteCurrentWeapon = 0;//m_pPlayerPed->GetCurrentWeapon();

	m_pPlayerPed->GetMatrix(&mat);
	psSync.vecPos.x = mat.pos.x;
	psSync.vecPos.y = mat.pos.y;
	psSync.vecPos.z = mat.pos.z;

	// send
	if((GetTickCount() - m_dwLastUpdatePassengerData) > 500 || memcmp(&m_PassengerData, &psSync, sizeof(PASSENGER_SYNC_DATA)))
	{
		m_dwLastUpdatePassengerData = GetTickCount();

		bsPassengerSync.Write((uint8_t)ID_PASSENGER_SYNC);
		bsPassengerSync.Write((char*)&psSync, sizeof(PASSENGER_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsPassengerSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);

		memcpy(&m_PassengerData, &psSync, sizeof(PASSENGER_SYNC_DATA));
	}
}

void CLocalPlayer::SendAimSyncData()
{
    AIM_SYNC_DATA aimSync;

    CAMERA_AIM* caAim = m_pPlayerPed->GetCurrentAim();

    aimSync.byteCamMode = m_pPlayerPed->GetCameraMode();
    aimSync.vecAimf.x = caAim->f1x;
    aimSync.vecAimf.y = caAim->f1y;
    aimSync.vecAimf.z = caAim->f1z;
    aimSync.vecAimPos.x = caAim->pos1x;
    aimSync.vecAimPos.y = caAim->pos1y;
    aimSync.vecAimPos.z = caAim->pos1z;
    aimSync.fAimZ = m_pPlayerPed->GetAimZ();
    aimSync.aspect_ratio = /*GameGetAspectRatio() * */ 255.0f;
    aimSync.byteCamExtZoom = (uint8_t)(m_pPlayerPed->GetCameraExtendedZoom() * 63.0f);

	CWeaponGta* pwstWeapon = m_pPlayerPed->GetCurrentWeaponSlot();
    if (pwstWeapon->dwState == 2) {
        aimSync.byteWeaponState = WS_RELOADING;
    }
    else {
        aimSync.byteWeaponState = (pwstWeapon->dwAmmoInClip > 1) ? WS_MORE_BULLETS : pwstWeapon->dwAmmoInClip;
    }

    RakNet::BitStream bsAimSync;
    bsAimSync.Write((char)ID_AIM_SYNC);
    bsAimSync.Write((char*)&aimSync, sizeof(AIM_SYNC_DATA));
    pNetGame->GetRakClient()->Send(&bsAimSync, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void CLocalPlayer::ProcessSpectating()
{
	RakNet::BitStream bsSpectatorSync;
	SPECTATOR_SYNC_DATA spSync;
	RwMatrix matPos;

	uint16_t lrAnalog, udAnalog;
	uint16_t wKeys = m_pPlayerPed->GetKeys(&lrAnalog, &udAnalog);
	pGame->GetCamera()->GetMatrix(&matPos);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	if(!pPlayerPool || !pVehiclePool) return;

	spSync.vecPos.x = matPos.pos.x;
	spSync.vecPos.y = matPos.pos.y;
	spSync.vecPos.z = matPos.pos.z;
	spSync.lrAnalog = lrAnalog;
	spSync.udAnalog = udAnalog;
	spSync.wKeys = wKeys;

	if((GetTickCount() - m_dwLastSendSpecTick) > GetOptimumOnFootSendRate())
	{
		m_dwLastSendSpecTick = GetTickCount();
		bsSpectatorSync.Write((uint8_t)ID_SPECTATOR_SYNC);
		bsSpectatorSync.Write((char*)&spSync, sizeof(SPECTATOR_SYNC_DATA));
		pNetGame->GetRakClient()->Send(&bsSpectatorSync, HIGH_PRIORITY, UNRELIABLE, 0);

		if((GetTickCount() - m_dwLastSendAimTick) > (GetOptimumOnFootSendRate() * 2))
		{
			m_dwLastSendAimTick = GetTickCount();
			
		}
	}

	m_pPlayerPed->SetHealth(100.0f);
	GetPlayerPed()->TeleportTo(spSync.vecPos.x, spSync.vecPos.y, spSync.vecPos.z + 20.0f);

	// handle spectate player left the server
	if(m_byteSpectateType == SPECTATE_TYPE_PLAYER &&
		!pPlayerPool->m_pPlayers[m_SpectateID])
	{
		m_byteSpectateType = SPECTATE_TYPE_NONE;
		m_bSpectateProcessed = false;
	}

	// handle spectate player is no longer active (ie Died)
	if(m_byteSpectateType == SPECTATE_TYPE_PLAYER &&
		pPlayerPool->m_pPlayers[m_SpectateID] &&
		(!pPlayerPool->GetAt(m_SpectateID)->IsActive() ||
		pPlayerPool->GetAt(m_SpectateID)->GetState() == PLAYER_STATE_WASTED))
	{
		m_byteSpectateType = SPECTATE_TYPE_NONE;
		m_bSpectateProcessed = false;
	}

	if(m_bSpectateProcessed) return;

	if(m_byteSpectateType == SPECTATE_TYPE_NONE)
	{
		GetPlayerPed()->RemoveFromVehicleAndPutAt(0.0f, 0.0f, 10.0f);
		pGame->GetCamera()->SetPosition(50.0f, 50.0f, 50.0f, 0.0f, 0.0f, 0.0f);
		pGame->GetCamera()->LookAtPoint(60.0f, 60.0f, 50.0f, 2);
		m_bSpectateProcessed = true;
	}
	else if(m_byteSpectateType == SPECTATE_TYPE_PLAYER)
	{
		if(pPlayerPool->m_pPlayers[m_SpectateID])
		{

			auto pPlayerPed = pPlayerPool->m_pPlayers[m_SpectateID]->GetPlayerPed();
			if(pPlayerPed)
			{
				auto* TheCamera = (uintptr_t*)(g_libGTASA + 0x00951FA8);
				CCamera::TakeControl(TheCamera, pPlayerPed->m_pPed,static_cast<eCamMode>(m_byteSpectateMode), eSwitchType::JUMPCUT, 1);

				m_bSpectateProcessed = true;
			}
		}
	}
	else if(m_byteSpectateType == SPECTATE_TYPE_VEHICLE)
	{
		auto pVehicle = pVehiclePool->m_pVehicles[ m_SpectateID ];

		auto* TheCamera = (uintptr_t*)(g_libGTASA + 0x00951FA8);
		CCamera::TakeControl(TheCamera, pVehicle->m_pVehicle,static_cast<eCamMode>(m_byteSpectateMode), eSwitchType::JUMPCUT, 1);
		m_bSpectateProcessed = true;
	}	
}

void CLocalPlayer::ToggleSpectating(bool bToggle)
{
	if(m_bIsSpectating && !bToggle)
		Spawn();

	m_bIsSpectating = bToggle;
	m_byteSpectateType = SPECTATE_TYPE_NONE;
	m_SpectateID = 0xFFFFFFFF;
	m_bSpectateProcessed = false;
}

void CLocalPlayer::SpectatePlayer(PLAYERID playerId)
{
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if(pPlayerPool && pPlayerPool->m_pPlayers[playerId])
	{
		if(pPlayerPool->GetAt(playerId)->GetState() != PLAYER_STATE_NONE &&
			pPlayerPool->GetAt(playerId)->GetState() != PLAYER_STATE_WASTED)
		{
			m_byteSpectateType = SPECTATE_TYPE_PLAYER;
			m_SpectateID = playerId;
			m_bSpectateProcessed = false;
		}
	}
}

void CLocalPlayer::SpectateVehicle(VEHICLEID VehicleID)
{
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	if (pVehiclePool && pVehiclePool->m_pVehicles[VehicleID])
	{
		m_byteSpectateType = SPECTATE_TYPE_VEHICLE;
		m_SpectateID = VehicleID;
		m_bSpectateProcessed = false;
	}
}

