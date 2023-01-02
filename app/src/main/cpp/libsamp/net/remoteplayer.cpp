#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "../chatwindow.h"

extern CGame *pGame;

CRemotePlayer::CRemotePlayer()
{
	m_PlayerID = INVALID_PLAYER_ID;
	m_VehicleID = INVALID_VEHICLE_ID;
	m_pPlayerPed = nullptr;
	m_pCurrentVehicle = nullptr;
	m_bIsNPC = false;
	m_bIsAFK = true;
	m_dwMarkerID = 0;
	m_dwGlobalMarkerID = 0;
	m_byteState = PLAYER_STATE_NONE;
	m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;
	m_bShowNameTag = true;

	m_dwLastRecvTick = 0;
	m_dwUnkTime = 0;

	m_byteSpecialAction = 0;
	m_byteSeatID = 0;
	m_byteWeaponShotID = 0xFF;
	m_fReportedHealth = 0.0f;
	m_fReportedArmour = 0.0f;
	m_bKeyboardOpened = false;

	m_dwCurrentAnimation = 0;
	m_usPlayingAnimIdx = 0;
	m_bWasSettedFlag = false;
}

CRemotePlayer::~CRemotePlayer()
{
	Remove();
}
extern bool g_uiHeadMoveEnabled;
void CRemotePlayer::ProcessSpecialActions(BYTE byteSpecialAction)
{
	if (!m_pPlayerPed || !m_pPlayerPed->IsAdded()) return;

    // приседание
    if (m_pPlayerPed->IsCrouching() && byteSpecialAction != SPECIAL_ACTION_DUCK) {
        m_pPlayerPed->ResetCrouch();
    }
    if (byteSpecialAction == SPECIAL_ACTION_DUCK && !m_pPlayerPed->IsCrouching()) {
        m_pPlayerPed->ApplyCrouch();
    }
    ///

	m_pPlayerPed->ProcessSpecialAction(byteSpecialAction);

//	// headsync:always
	if (GetState() == PLAYER_STATE_ONFOOT)
	{


		//headmove?
		if ((GetTickCount() - m_dwLastHeadUpdate) > 500 && g_uiHeadMoveEnabled)
		{
			VECTOR LookAt;
			CAMERA_AIM* Aim = GameGetRemotePlayerAim(m_pPlayerPed->m_bytePlayerNumber);
			LookAt.X = Aim->pos1x + (Aim->f1x * 20.0f);
			LookAt.Y = Aim->pos1y + (Aim->f1y * 20.0f);
			LookAt.Z = Aim->pos1z + (Aim->f1z * 20.0f);
			ScriptCommand(&TASK_LOOK_AT_COORD, m_pPlayerPed->m_dwGTAId, LookAt.X, LookAt.Y, LookAt.Z, 3000);
			m_dwLastHeadUpdate = GetTickCount();
		}
	}
}

#define OUTCOMING_KEY 0x87
#define OUTCOMING_NUM 0x80
void CRemotePlayer::Process()
{
	CPlayerPool *pPool = pNetGame->GetPlayerPool();
	CLocalPlayer *pLocalPlayer = pPool->GetLocalPlayer();
	MATRIX4X4 matPlayer, matVehicle;
	VECTOR vecMoveSpeed;

	if(IsActive())
	{
		if(GetTickCount() > m_dwWaitForEntryExitAnims) {
			HandleVehicleEntryExit();
		}
		// ---- ONFOOT NETWORK PROCESSING ----
		if(GetState() == PLAYER_STATE_ONFOOT && 
			m_byteUpdateFromNetwork == UPDATE_TYPE_ONFOOT && !m_pPlayerPed->IsInVehicle())
		{
			// UPDATE CURRENT WEAPON
//			if (m_iKey0 ^ OUTCOMING_KEY != OUTCOMING_NUM)
//			{
//
//			}
			if (GetPlayerPed()->GetCurrentWeapon() != m_ofSync.byteCurrentWeapon)
			{
				GetPlayerPed()->GiveWeapon(m_ofSync.byteCurrentWeapon, 9999);
				GetPlayerPed()->SetArmedWeapon(m_ofSync.byteCurrentWeapon);
			}
			// First person weapon action hack
			/*if (!IS_TARGETING(m_ofSync.wKeys)) 
			{
				//0x0043893C
				((uint32_t(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x0043893C + 1))((uintptr_t)m_pPlayerPed, 1);
			}*/
			

			UpdateOnFootPositionAndSpeed(&m_ofSync.vecPos, &m_ofSync.vecMoveSpeed);
			UpdateOnFootTargetPosition();
		}
		else if(GetState() == PLAYER_STATE_DRIVER &&
			m_byteUpdateFromNetwork == UPDATE_TYPE_INCAR && m_pPlayerPed->IsInVehicle())
		{
			if (!m_pCurrentVehicle)
			{
				return;
			}
			if (!GamePool_Vehicle_GetAt((int)m_pCurrentVehicle->m_dwGTAId))
			{
				return;
			}

			m_icSync.quat.Normalize();
			m_icSync.quat.GetMatrix(&matVehicle);
			matVehicle.pos.X = m_icSync.vecPos.X;
			matVehicle.pos.Y = m_icSync.vecPos.Y;
			matVehicle.pos.Z = m_icSync.vecPos.Z;

			if( m_pCurrentVehicle->GetModelIndex() == TRAIN_PASSENGER_LOCO ||
				m_pCurrentVehicle->GetModelIndex() == TRAIN_FREIGHT_LOCO ||
				m_pCurrentVehicle->GetModelIndex() == TRAIN_TRAM)
			{
			}
			else
			{
				UpdateInCarMatrixAndSpeed(&matVehicle, &m_icSync.vecPos, &m_icSync.vecMoveSpeed);
				UpdateInCarTargetPosition();
			}
			if (m_icSync.fCarHealth <= 300.0f)
			{
				m_icSync.fCarHealth = 300.0f;
			}
			m_pCurrentVehicle->SetHealth(m_icSync.fCarHealth);

			// TRAILER AUTOMATIC ATTACHMENT AS THEY MOVE INTO IT
			if (m_pCurrentVehicle->GetDistanceFromLocalPlayerPed() < 200.0f)
			{
				if (m_icSync.TrailerID && m_icSync.TrailerID < MAX_VEHICLES)
				{
					CVehicle* pRealTrailer = m_pCurrentVehicle->GetTrailer();
					CVehicle* pTrailerVehicle = pNetGame->GetVehiclePool()->GetAt(m_icSync.TrailerID);
					if (!pRealTrailer) 
					{
						if (pTrailerVehicle) 
						{
							m_pCurrentVehicle->SetTrailer(pTrailerVehicle);
							m_pCurrentVehicle->AttachTrailer();
						}
					}
				}
				else
				{
					if (m_pCurrentVehicle->GetTrailer()) 
					{
						m_pCurrentVehicle->DetachTrailer();
						m_pCurrentVehicle->SetTrailer(nullptr);
					}
				}
			}
			else
			{
				m_pCurrentVehicle->SetTrailer(nullptr);
			}
		}
		else if(GetState() == PLAYER_STATE_PASSENGER && 
			m_byteUpdateFromNetwork == UPDATE_TYPE_PASSENGER)
		{
			if(!m_pCurrentVehicle) return;
		}

		m_byteUpdateFromNetwork = UPDATE_TYPE_NONE;

		// ------ PROCESSED FOR ALL FRAMES ----- 
		if(GetState() == PLAYER_STATE_ONFOOT && !m_pPlayerPed->IsInVehicle())
		{
			if ((GetTickCount() - m_dwLastHeadUpdate) > 500 /*&& pSettings->GetReadOnly().szHeadMove*/)
			{
				VECTOR LookAt;
				CAMERA_AIM* Aim = GameGetRemotePlayerAim(m_pPlayerPed->m_bytePlayerNumber);
				LookAt.X = Aim->pos1x + (Aim->f1x * 20.0f);
				LookAt.Y = Aim->pos1y + (Aim->f1y * 20.0f);
				LookAt.Z = Aim->pos1z + (Aim->f1z * 20.0f);
				m_pPlayerPed->ApplyCommandTask("FollowPedSA", 0, 2000, -1, &LookAt, 0, 0.1f, 500, 3, 0);
				m_dwLastHeadUpdate = GetTickCount();
			}
			if(GetTickCount() > m_dwWaitForEntryExitAnims) {
				ProcessSpecialActions(m_ofSync.byteSpecialAction);
				SlerpRotation();
				HandleAnimations();
			}

			if (m_byteWeaponShotID != 0xFF)
			{
				//weapon sync
				//MATRIX4X4 localMat;
				//pPool->GetLocalPlayer()->GetPlayerPed()->GetMatrix(&localMat);

				m_ofSync.byteCurrentWeapon = m_byteWeaponShotID;
				m_pPlayerPed->SetArmedWeapon((int)m_byteWeaponShotID);
				//ScriptCommand(&task_shoot_at_coord, m_pPlayerPed->m_dwGTAId, localMat.pos.X, localMat.pos.Y, localMat.pos.Z, 10);
				m_pPlayerPed->SetCurrentAim(pPool->GetLocalPlayer()->GetPlayerPed()->GetCurrentAim());
				m_pPlayerPed->SetKeys((uint16_t)4, m_ofSync.lrAnalog, m_ofSync.udAnalog);

				//unknown weapon
				m_byteWeaponShotID = 0xFF;
			}
			else
				m_pPlayerPed->SetKeys(m_ofSync.wKeys, m_ofSync.lrAnalog, m_ofSync.udAnalog);

			if (RemotePlayerKeys[GetPlayerPed()->m_bytePlayerNumber].bKeys[ePadKeys::KEY_HANDBRAKE])
			{
				GetPlayerPed()->SetPlayerAimState();
			}
			else
			{
				GetPlayerPed()->ClearPlayerAimState();
			}
			
			if( m_ofSync.vecMoveSpeed.X == 0.0f &&
				m_ofSync.vecMoveSpeed.Y == 0.0f &&
				m_ofSync.vecMoveSpeed.Z == 0.0f)
			{
				m_pPlayerPed->SetMoveSpeedVector(m_ofSync.vecMoveSpeed);
			}

			if((GetTickCount() - m_dwLastRecvTick) > 1500)
				m_bIsAFK = true;

			if(m_bIsAFK && ((GetTickCount() - m_dwLastRecvTick) > 3000))
			{
				m_ofSync.lrAnalog = 0;
				m_ofSync.udAnalog = 0;
				
				vecMoveSpeed.X = 0.0f;
				vecMoveSpeed.Y = 0.0f;
				vecMoveSpeed.Z = 0.0f;
				m_pPlayerPed->SetMoveSpeedVector(vecMoveSpeed);
				
				m_pPlayerPed->GetMatrix(&matPlayer);
				matPlayer.pos.X = m_ofSync.vecPos.X;
				matPlayer.pos.Y = m_ofSync.vecPos.Y;
				matPlayer.pos.Z = m_ofSync.vecPos.Z;
				m_pPlayerPed->SetMatrix(matPlayer);
			}
		}
		else if(GetState() == PLAYER_STATE_DRIVER && m_pPlayerPed->IsInVehicle())
		{
			if (!m_pCurrentVehicle)
			{
				return;
			}
			if (!GamePool_Vehicle_GetAt(m_pCurrentVehicle->m_dwGTAId))
			{
				return;
			}
			if( m_pCurrentVehicle->GetModelIndex() != TRAIN_PASSENGER_LOCO &&
				m_pCurrentVehicle->GetModelIndex() != TRAIN_FREIGHT_LOCO &&
				m_pCurrentVehicle->GetModelIndex() != TRAIN_TRAM)
			{
				UpdateVehicleRotation();
			}

			if(	m_icSync.vecMoveSpeed.X == 0.0f &&
				m_icSync.vecMoveSpeed.Y == 0.0f &&
				m_icSync.vecMoveSpeed.Z == 0.0f)
			{
				m_pCurrentVehicle->SetMoveSpeedVector(m_icSync.vecMoveSpeed);
			}

			m_pPlayerPed->SetKeys(m_icSync.wKeys, m_icSync.lrAnalog, m_icSync.udAnalog);

			if((GetTickCount() - m_dwLastRecvTick) > 1500)
				m_bIsAFK = true;

		}
		else if(GetState() == PLAYER_STATE_PASSENGER)
		{
			if((GetTickCount() - m_dwLastRecvTick) >= 3000)
				m_bIsAFK = true;
		}
		else
		{
			m_pPlayerPed->SetKeys(0, 0, 0);
			vecMoveSpeed.X = 0.0f;
			vecMoveSpeed.Y = 0.0f;
			vecMoveSpeed.Z = 0.0f;
			m_pPlayerPed->SetMoveSpeedVector(vecMoveSpeed);
		}

		if(m_byteState != PLAYER_STATE_WASTED)
			m_pPlayerPed->SetHealth(1000.0f);

		if((GetTickCount() - m_dwLastRecvTick) < 1500)
			m_bIsAFK = false;
	}
	else
	{
		if(m_pPlayerPed)
		{
			//ResetAllSyncAttributes();
			pGame->RemovePlayer(m_pPlayerPed);
			m_pPlayerPed = nullptr;
		}
	}
}

uint32_t CRemotePlayer::GetPlayerColorAsARGB()
{
	return (TranslateColorCodeToRGBA(m_PlayerID) >> 8) | 0xFF000000;
}


void CRemotePlayer::SlerpRotation()
{
	MATRIX4X4 mat;
	CQuaternion quatPlayer, quatResult;

	if(m_pPlayerPed)
	{
		m_pPlayerPed->GetMatrix(&mat);

		quatPlayer.SetFromMatrix(mat);

		quatResult.Slerp(&m_ofSync.quat, &quatPlayer, 0.75f);
		quatResult.GetMatrix(&mat);
		m_pPlayerPed->SetMatrix(mat);

		double fZ = atan2(-mat.up.X, mat.up.Y) * 57.295776; /* rad to deg */
		if(fZ > 360.0f) fZ -= 360.0f;
		if(fZ < 0.0f) fZ += 360.0f;
		m_pPlayerPed->SetRotation((float)fZ);
	}
}


void CRemotePlayer::Remove()
{
	if(m_dwMarkerID)
	{
		pGame->DisableMarker(m_dwMarkerID);
		m_dwMarkerID = 0;
	}

	if(m_dwGlobalMarkerID)
	{
		pGame->DisableMarker(m_dwGlobalMarkerID);
		m_dwGlobalMarkerID = 0;
	}

	if(m_pPlayerPed)
	{
		pGame->RemovePlayer(m_pPlayerPed);
		m_pPlayerPed = nullptr;
	}
}

void CRemotePlayer::UpdateInCarMatrixAndSpeed(MATRIX4X4* mat, VECTOR* pos, VECTOR* speed)
{
	m_InCarQuaternion.SetFromMatrix(*mat);

	m_vecInCarTargetPos.X = pos->X;
	m_vecInCarTargetPos.Y = pos->Y;
	m_vecInCarTargetPos.Z = pos->Z;

	m_vecInCarTargetSpeed.X = speed->X;
	m_vecInCarTargetSpeed.Y = speed->Y;
	m_vecInCarTargetSpeed.Z = speed->Z;

	m_pCurrentVehicle->SetMoveSpeedVector(*speed);
}

void CRemotePlayer::UpdateInCarTargetPosition()
{
	MATRIX4X4 matEnt;
	VECTOR vec = { 0.0f, 0.0f, 0.0f };

	float delta = 0.0f;

	if(!m_pCurrentVehicle) return;

	m_pCurrentVehicle->GetMatrix(&matEnt);

	if(m_pCurrentVehicle->IsAdded())
	{
		m_vecPosOffset.X = FloatOffset(m_vecInCarTargetPos.X, matEnt.pos.X);
		m_vecPosOffset.Y = FloatOffset(m_vecInCarTargetPos.Y, matEnt.pos.Y);
		m_vecPosOffset.Z = FloatOffset(m_vecInCarTargetPos.Z, matEnt.pos.Z);

		if(m_vecPosOffset.X > 0.05f || m_vecPosOffset.Y > 0.05f || m_vecPosOffset.Z > 0.05f)
		{
			delta = 0.5f;
			if( m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BOAT ||
				m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_PLANE ||
				m_pCurrentVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_HELI)
			{
				delta = 2.0f;
			}

			if(m_vecPosOffset.X > 8.0f || m_vecPosOffset.Y > 8.0f || m_vecPosOffset.Z > delta)
			{
				matEnt.pos.X = m_vecInCarTargetPos.X;
				matEnt.pos.Y = m_vecInCarTargetPos.Y;
				matEnt.pos.Z = m_vecInCarTargetPos.Z;
				m_pCurrentVehicle->SetMatrix(matEnt);
				m_pCurrentVehicle->SetMoveSpeedVector(m_vecInCarTargetSpeed);
			}
			else
			{
				m_pCurrentVehicle->GetMoveSpeedVector(&vec);
				if(m_vecPosOffset.X > 0.05f)
					vec.X += (m_vecInCarTargetPos.X - matEnt.pos.X) * 0.06f;
				if(m_vecPosOffset.Y > 0.05f)
					vec.Y += (m_vecInCarTargetPos.Y - matEnt.pos.Y) * 0.06f;
				if(m_vecPosOffset.Z > 0.05f)
					vec.Z += (m_vecInCarTargetPos.Z - matEnt.pos.Z) * 0.06f;

				if( FloatOffset(vec.X, 0.0f) > 0.01f ||
					FloatOffset(vec.Y, 0.0f) > 0.01f ||
					FloatOffset(vec.Z, 0.0f) > 0.01f)
				{
					m_pCurrentVehicle->SetMoveSpeedVector(vec);
				}
			}
		}
	}
	else
	{
		matEnt.pos.X = m_vecInCarTargetPos.X;
		matEnt.pos.Y = m_vecInCarTargetPos.Y;
		matEnt.pos.Z = m_vecInCarTargetPos.Z;
		m_pCurrentVehicle->SetMatrix(matEnt);
	}
}

void CRemotePlayer::UpdateVehicleRotation()
{
	CQuaternion quat, qresult;
	MATRIX4X4 matEnt;
	VECTOR vec = { 0.0f, 0.0f, 0.0f };


	if(!m_pCurrentVehicle) return;

	m_pCurrentVehicle->GetTurnSpeedVector(&vec);
	if(vec.X <= 0.02f)
	{
		if(vec.X < -0.02f) vec.X = -0.02f;
	}
	else vec.X = 0.02f;

	if(vec.Y <= 0.02f)
	{
		if(vec.Y < -0.02f) vec.Y = -0.02f;
	}
	else vec.Y = 0.02f;

	if(vec.Z <= 0.02f)
	{
		if(vec.Z < -0.02f) vec.Z = -0.02f;
	}
	else vec.Z = 0.02f;

	m_pCurrentVehicle->SetTurnSpeedVector(vec);

	m_pCurrentVehicle->GetMatrix(&matEnt);
	quat.SetFromMatrix(matEnt);
	qresult.Slerp(&m_InCarQuaternion, &quat, 0.75f);
	qresult.Normalize();
	qresult.GetMatrix(&matEnt);
	m_pCurrentVehicle->SetMatrix(matEnt);
}

bool CRemotePlayer::Spawn(uint8_t byteTeam, unsigned int iSkin, VECTOR *vecPos, float fRotation, 
	uint32_t dwColor, uint8_t byteFightingStyle, bool bVisible)
{
	if(m_pPlayerPed)
	{
		pGame->RemovePlayer(m_pPlayerPed);
		m_pPlayerPed = nullptr;
	}

	CPlayerPed *pPlayer = pGame->NewPlayer(iSkin, vecPos->X, vecPos->Y, vecPos->Z, fRotation);

	if(pPlayer)
	{
		if(dwColor != 0) SetPlayerColor(dwColor);

		if(m_dwMarkerID)
		{
			pGame->DisableMarker(m_dwMarkerID);
			m_dwMarkerID = 0;
		}

		if(pNetGame->m_iShowPlayerMarkers) 
			pPlayer->ShowMarker(m_PlayerID);

		m_pPlayerPed = pPlayer;
		m_fReportedHealth = 100.0f;
		if(byteFightingStyle != 4)
			m_pPlayerPed->SetFightingStyle(byteFightingStyle);

		SetState(PLAYER_STATE_SPAWNED);
		return true;
	}

	SetState(PLAYER_STATE_NONE);
	return false;
}

void CRemotePlayer::HandleVehicleEntryExit()
{
//	Log("HandleVehicleEntryExit");
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	MATRIX4X4 mat;

	if(!m_pPlayerPed) return;

	if(m_pPlayerPed->IsInVehicle() && m_pCurrentVehicle &&  m_pPlayerPed->GetCurrentVehicleID() != m_pCurrentVehicle->m_pVehicle) {
		//Log("da");
		CVehicle *pVehicle = pVehiclePool->GetAt(m_VehicleID);
		if (pVehicle) {
			m_pPlayerPed->PutDirectlyInVehicle(pVehicle, m_byteSeatID);
		}
	}
	if(GetState() == PLAYER_STATE_ONFOOT) {
		if(m_pPlayerPed->IsInVehicle()) {
			m_pPlayerPed->GetMatrix(&mat);
			m_pPlayerPed->RemoveFromVehicleAndPutAt(mat.pos.X,mat.pos.Y,mat.pos.Z);
		}
		return;
	}

	if( GetState() == PLAYER_STATE_DRIVER || GetState() == PLAYER_STATE_PASSENGER )
	{
		if(!m_pPlayerPed->IsInVehicle()) {
			CVehicle *pVehicle = pVehiclePool->GetAt(m_VehicleID);
			if(pVehicle) {
				m_pPlayerPed->PutDirectlyInVehicle(pVehicle, m_byteSeatID);
			}
		}
	}
}

void CRemotePlayer::EnterVehicle(VEHICLEID VehicleID, bool bPassenger)
{
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	if(m_pPlayerPed && !m_pPlayerPed->IsInVehicle()) {

		if(bPassenger) {
			SetState(PLAYER_STATE_ENTER_VEHICLE_PASSENGER);
		} else {
			SetState(PLAYER_STATE_ENTER_VEHICLE_DRIVER);
		}

		m_pPlayerPed->SetKeys(0,0,0);
		if(m_pPlayerPed->GetDistanceFromLocalPlayerPed() < 120.0f) {
			int iGtaVehicleID = pVehiclePool->FindGtaIDFromID(VehicleID);
			if(iGtaVehicleID && iGtaVehicleID != INVALID_VEHICLE_ID) {
				m_pPlayerPed->EnterVehicle(iGtaVehicleID, bPassenger);
			}
		}
	}
	m_dwWaitForEntryExitAnims = GetTickCount() + 2000;
}

void CRemotePlayer::ExitVehicle()
{
	if(m_pPlayerPed && m_pPlayerPed->IsInVehicle())
	{
		SetState(PLAYER_STATE_EXIT_VEHICLE);
		m_pPlayerPed->SetKeys(0, 0, 0);
		m_pPlayerPed->ExitCurrentVehicle();
	}
	m_dwWaitForEntryExitAnims = GetTickCount() + 1500;
}

void CRemotePlayer::UpdateOnFootPositionAndSpeed(VECTOR *vecPos, VECTOR *vecMove)
{
	m_vecOnFootTargetPos.X = vecPos->X;
	m_vecOnFootTargetPos.Y = vecPos->Y;
	m_vecOnFootTargetPos.Z = vecPos->Z;
	m_vecOnFootTargetSpeed.X = vecMove->X;
	m_vecOnFootTargetSpeed.Y = vecMove->Y;
	m_vecOnFootTargetSpeed.Z = vecMove->Z;

	m_pPlayerPed->SetMoveSpeedVector(m_vecOnFootTargetSpeed);
}

void CRemotePlayer::StoreTrailerFullSyncData(TRAILER_SYNC_DATA* trSync)
{
	VEHICLEID TrailerID = trSync->trailerID;
	if (!TrailerID) return;

	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(TrailerID);

	if (pVehicle)
	{
		MATRIX4X4 matWorld;

		trSync->quat.GetMatrix(&matWorld);
		matWorld.pos.X = trSync->vecPos.X;
		matWorld.pos.Y = trSync->vecPos.Y;
		matWorld.pos.Z = trSync->vecPos.Z;

		pVehicle->SetMatrix(matWorld);
		pVehicle->SetMoveSpeedVector(trSync->vecMoveSpeed);
		pVehicle->SetTurnSpeedVector(trSync->vecTurnSpeed);
	}
}

void CRemotePlayer::UpdateOnFootTargetPosition()
{
	MATRIX4X4 mat;
	VECTOR vec;

	if(!m_pPlayerPed) return;
	m_pPlayerPed->GetMatrix(&mat);

	if(!m_pPlayerPed->IsAdded())
	{
		mat.pos.X = m_vecOnFootTargetPos.X;
		mat.pos.Y = m_vecOnFootTargetPos.Y;
		mat.pos.Z = m_vecOnFootTargetPos.Z;

		m_pPlayerPed->SetMatrix(mat);
		return;
	}

	m_vecPosOffset.X = FloatOffset(m_vecOnFootTargetPos.X, mat.pos.X);
	m_vecPosOffset.Y = FloatOffset(m_vecOnFootTargetPos.Y, mat.pos.Y);
	m_vecPosOffset.Z = FloatOffset(m_vecOnFootTargetPos.Z, mat.pos.Z);

	if(m_vecPosOffset.X > 0.00001f || m_vecPosOffset.Y > 0.00001f || m_vecPosOffset.Z > 0.00001f)
	{
		if(m_vecPosOffset.X > 2.0f || m_vecPosOffset.Y > 2.0f || m_vecPosOffset.Z > 1.0f)
		{
			mat.pos.X = m_vecOnFootTargetPos.X;
			mat.pos.Y = m_vecOnFootTargetPos.Y;
			mat.pos.Z = m_vecOnFootTargetPos.Z;
			m_pPlayerPed->SetMatrix(mat);
			return;
		}

		m_pPlayerPed->GetMoveSpeedVector(&vec);
		if(m_vecPosOffset.X > 0.00001f)
			vec.X += (m_vecOnFootTargetPos.X - mat.pos.X) * 0.1f;
		if(m_vecPosOffset.Y > 0.00001f)
			vec.Y += (m_vecOnFootTargetPos.Y - mat.pos.Y) * 0.1f;
		if(m_vecPosOffset.Z > 0.00001f)
			vec.Z += (m_vecOnFootTargetPos.Z - mat.pos.Z) * 0.1f;

		m_pPlayerPed->SetMoveSpeedVector(vec);
	}
}
float                    m_fWeaponDamages[43 + 1]
= {
5.0f, /* Fist */
5.0f, /* Brass Knuckles */
5.0f, /* Golf Club */
5.0f, /* Nightstick */
5.0f, /* Knife */
5.0f, /* Baseball Bat */
5.0f, /* Shovel */
5.0f, /* Pool Cue */
5.0f, /* Katana */
5.0f, /* Chainsaw */
5.0f, /* Purple Dildo */
5.0f, /* Dildo */
5.0f, /* Vibrator */
5.0f, /* Silver Vibrator */
5.0f, /* Flowers */
5.0f, /* Cane */
75.0f, /* Grenade */
0.0f, /* Tear Gas */
75.0f, /* Molotov Cocktail */
0.0f, /* +EMPTY+ */
0.0f, /* +EMPTY+ */
0.0f, /* +EMPTY+ */
25.0f, /* 9mm (pistol) */
10.0f, /* Silenced 9mm */
45.0f, /* Desert Eagle */
40.0f, /* Shotgun */
10.0f, /* Sawnoff Shotgun */
10.0f, /* Combat Shotgun */
10.0f, /* Micro SMG/Uzi */
10.0f, /* MP5 */
10.0f, /* AK-47 */
10.0f, /* M4 */
20.0f, /* Tec-9 */
45.0f, /* Country Rifle */
45.0f, /* Sniper Rifle */
75.0f, /* RPG */
75.0f, /* HS Rocket */
5.0f, /* Flamethrower */
70.0f, /* Minigun */
75.0f, /* Satchel Charge */
0.0f, /* Detonator */
1.0f, /* Spraycan */
5.0f, /* Fire Extinguisher */
0.0f /* Camera */
};
void CRemotePlayer::StoreBulletSyncData(BULLET_SYNC* pBulletSync)
{
	if (!m_pPlayerPed || !m_pPlayerPed->IsAdded()) return;

	BULLET_DATA btData;
	memset(&btData, 0, sizeof(BULLET_DATA));

	btData.vecOrigin.X = pBulletSync->vecOrigin.X;
	btData.vecOrigin.Y = pBulletSync->vecOrigin.Y;
	btData.vecOrigin.Z = pBulletSync->vecOrigin.Z;

	btData.vecPos.X = pBulletSync->vecPos.X;
	btData.vecPos.Y = pBulletSync->vecPos.Y;
	btData.vecPos.Z = pBulletSync->vecPos.Z;

	btData.vecOffset.X = pBulletSync->vecOffset.X;
	btData.vecOffset.Y = pBulletSync->vecOffset.Y;
	btData.vecOffset.Z = pBulletSync->vecOffset.Z;

	/*Log("Receiving button data..");
	Log("vecOrigin: %f, %f, %f", btData.vecOrigin.X, btData.vecOrigin.Y, btData.vecOrigin.Z);
	Log("vecPos: %f, %f, %f", btData.vecPos.X, btData.vecPos.Y, btData.vecPos.Z);
	Log("vecOffset: %f, %f, %f", btData.vecOffset.X, btData.vecOffset.Y, btData.vecOffset.Z);
	Log("hit: %d, ID: %d", pBulletSync->byteHitType, pBulletSync->PlayerID);*/

	if (pBulletSync->byteHitType != 0)
	{
		if (btData.vecOffset.X > 300.0f ||
			btData.vecOffset.X < -300.0f ||
			btData.vecOffset.Y > 300.0f ||
			btData.vecOffset.Y < -300.0f ||
			btData.vecOffset.Z > 300.0f ||
			btData.vecOffset.Z < -300.0f
				)
		{
			return;
		}

		if (pBulletSync->byteHitType == 1)
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if (pPlayerPool)
			{
				if (pBulletSync->PlayerID == pPlayerPool->GetLocalPlayerID())
				{
					btData.pEntity = &pGame->FindPlayerPed()->m_pPed->entity;
				}
				else if (pBulletSync->PlayerID == m_PlayerID)
				{
					return;
				}
				else
				{
					CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(pBulletSync->PlayerID);
					if (pRemotePlayer)
					{
						CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();

						if (pPlayerPed)
							btData.pEntity = &pPlayerPed->m_pPed->entity;
					}
				}
			}
		}
		else if (pBulletSync->byteHitType == 2)
		{
			CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
			if (pVehiclePool)
			{
				CVehicle* pVehicle = pVehiclePool->GetAt(pBulletSync->PlayerID);
				if (pVehicle)
				{
					btData.pEntity = &pVehicle->m_pVehicle->entity;
				}
			}
		}

		m_pPlayerPed->ProcessBulletData(&btData);
		m_pPlayerPed->FireInstant();
	}

	if (m_pPlayerPed->IsAdded())
	{
		uint8_t byteWeapon = pBulletSync->byteWeaponID;
		if (m_pPlayerPed->GetCurrentWeapon() != byteWeapon)
		{
			m_pPlayerPed->SetArmedWeapon(byteWeapon);
			if (m_pPlayerPed->GetCurrentWeapon() != byteWeapon)
			{
				m_pPlayerPed->GiveWeapon(byteWeapon, 9999);
				m_pPlayerPed->SetArmedWeapon(byteWeapon);
			}
		}
	}
}

void CRemotePlayer::SetPlayerColor(uint32_t dwColor)
{
	SetRadarColor(m_PlayerID, dwColor);
}

void CRemotePlayer::Say(unsigned char* szText)
{
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	if (pPlayerPool) 
	{
		char * szPlayerName = pPlayerPool->GetPlayerName(m_PlayerID);
		CChatWindow::AddChatMessage(szPlayerName,GetPlayerColor(), (char*)szText);
	}
}

void calculateAimVector(VECTOR* vec1, VECTOR* vec2)
{
	float f1;
	float f2;
	float f3;

	f1 = atan2(vec1->X, vec1->Y) - 1.570796370506287;
	f2 = sin(f1);
	f3 = cos(f1);
	vec2->X = vec1->Y * 0.0 - f3 * vec1->Z;
	vec2->Y = f2 * vec1->Z - vec1->X * 0.0;
	vec2->Z = f3 * vec1->X - f2 * vec1->Y;
}

void CRemotePlayer::UpdateAimFromSyncData(AIM_SYNC_DATA * pAimSync)
{
	if (!m_pPlayerPed) return;
	m_pPlayerPed->SetCameraMode(pAimSync->byteCamMode);

	CAMERA_AIM Aim;

	Aim.f1x = pAimSync->vecAimf.X;
	Aim.f1y = pAimSync->vecAimf.Y;
	Aim.f1z = pAimSync->vecAimf.Z;
	Aim.pos1x = pAimSync->vecAimPos.X;
	Aim.pos1y = pAimSync->vecAimPos.Y;
	Aim.pos1z = pAimSync->vecAimPos.Z;
	Aim.pos2x = pAimSync->vecAimPos.X;
	Aim.pos2y = pAimSync->vecAimPos.Y;
	Aim.pos2z = pAimSync->vecAimPos.Z;

	VECTOR vec1;
	vec1.X = Aim.f1x;
	vec1.Y = Aim.f1y;
	vec1.Z = Aim.f1z;

	VECTOR vec2;
	vec2.X = 0.0f;
	vec2.Y = 0.0f;
	vec2.Z = 0.0f;

	calculateAimVector(&vec1, &vec2);

	Aim.f2x = vec2.X;
	Aim.f2y = vec2.Y;
	Aim.f2z = vec2.Z;

	m_pPlayerPed->SetCurrentAim(&Aim);
	m_pPlayerPed->SetAimZ(pAimSync->fAimZ);

	float fAspect = pAimSync->aspect_ratio * 0.0039215689f;
	float fExtZoom = (pAimSync->byteCamExtZoom) * 0.015873017f;

	m_pPlayerPed->SetCameraExtendedZoom(fExtZoom, fAspect);

	WEAPON_SLOT_TYPE* pwstWeapon = m_pPlayerPed->GetCurrentWeaponSlot();
	if (pAimSync->byteWeaponState == WS_RELOADING)
		pwstWeapon->dwState = 2;		// Reloading
	else
	if (pAimSync->byteWeaponState != WS_MORE_BULLETS)
		pwstWeapon->dwAmmoInClip = (uint32_t)pAimSync->byteWeaponState;
	else
	if (pwstWeapon->dwAmmoInClip < 2)
		pwstWeapon->dwAmmoInClip = 2;
}

void CRemotePlayer::StoreOnFootFullSyncData(ONFOOT_SYNC_DATA *pofSync, uint32_t dwTime)
{
	if(GetTickCount() < m_dwWaitForEntryExitAnims) return;

	if( !dwTime || (dwTime - m_dwUnkTime) >= 0 )
	{
		m_dwUnkTime = dwTime;
		//m_iKey0 = key;
		m_dwLastRecvTick = GetTickCount();
		memcpy(&m_ofSync, pofSync, sizeof(ONFOOT_SYNC_DATA));
		m_fReportedHealth = (float)pofSync->byteHealth;
		m_fReportedArmour = (float)pofSync->byteArmour;
		m_byteSpecialAction = pofSync->byteSpecialAction;
		m_byteUpdateFromNetwork = UPDATE_TYPE_ONFOOT;

		if(m_pPlayerPed) {
			if(m_pPlayerPed->IsInVehicle()) {
				if(m_byteSpecialAction != SPECIAL_ACTION_ENTER_VEHICLE && m_byteSpecialAction != SPECIAL_ACTION_EXIT_VEHICLE)
					RemoveFromVehicle();
			}
		}

		SetState(PLAYER_STATE_ONFOOT);
	}
}

void CRemotePlayer::StoreInCarFullSyncData(INCAR_SYNC_DATA *picSync, uint32_t dwTime)
{
	if(GetTickCount() < m_dwWaitForEntryExitAnims) return;
	if(!dwTime || (dwTime - m_dwUnkTime >= 0))
	{
		m_dwUnkTime = dwTime;

		CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

		memcpy(&m_icSync, picSync, sizeof(INCAR_SYNC_DATA));
		m_VehicleID = picSync->VehicleID;
		if(!pVehiclePool) return;
		CVehicle *pVehicle = pVehiclePool->GetAt(m_VehicleID);
		if(!pVehicle)return;
		if(pVehicle != m_pCurrentVehicle){
			m_pPlayerPed->PutDirectlyInVehicle(pVehicle, 0);
			m_pCurrentVehicle = pVehicle;
		}
		//if(pVehiclePool) m_pCurrentVehicle = pVehiclePool->GetAt(m_VehicleID);

		m_byteSeatID = 0;
		m_fReportedHealth = (float)picSync->bytePlayerHealth;
		m_fReportedArmour = (float)picSync->bytePlayerArmour;
		m_byteUpdateFromNetwork = UPDATE_TYPE_INCAR;
		m_dwLastRecvTick = GetTickCount();

		m_byteSpecialAction = 0;

		SetState(PLAYER_STATE_DRIVER);
	}
}

void CRemotePlayer::StorePassengerFullSyncData(PASSENGER_SYNC_DATA *ppsSync)
{
	if(GetTickCount() < m_dwWaitForEntryExitAnims) return;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

	memcpy(&m_psSync, ppsSync, sizeof(PASSENGER_SYNC_DATA));
	m_VehicleID = ppsSync->VehicleID;
	m_byteSeatID = ppsSync->byteSeatFlags & 127;
	if(pVehiclePool) m_pCurrentVehicle = pVehiclePool->GetAt(m_VehicleID);
	m_fReportedHealth = (float)ppsSync->bytePlayerHealth;
	m_fReportedArmour = (float)ppsSync->bytePlayerArmour;
	m_byteUpdateFromNetwork = UPDATE_TYPE_PASSENGER;
	m_dwLastRecvTick = GetTickCount();

	SetState(PLAYER_STATE_PASSENGER);
}

void CRemotePlayer::RemoveFromVehicle()
{
	MATRIX4X4 mat;

	if(m_pPlayerPed)
	{
		if(m_pPlayerPed->IsInVehicle())
		{
			m_pPlayerPed->GetMatrix(&mat);
			m_pPlayerPed->RemoveFromVehicleAndPutAt(mat.pos.X, mat.pos.Y, mat.pos.Z+1.0);
		}
	}
}

void CRemotePlayer::HandleDeath()
{
	if (GetPlayerPed())
	{
		GetPlayerPed()->SetKeys(0, 0, 0);
		GetPlayerPed()->SetDead();
	}
	m_fReportedHealth = 0.0f;
	m_fReportedArmour = 0.0f;
	SetState(PLAYER_STATE_WASTED);

	memset(&m_ofSync, 0, sizeof(m_ofSync));
	memset(&m_icSync, 0, sizeof(m_icSync));
	memset(&m_psSync, 0, sizeof(m_psSync));

	HideGlobalMarker();
}

void CRemotePlayer::HandleAnimations()
{
	if (!m_pPlayerPed)
	{
		return;
	}

	int flag = m_ofSync.dwAnimation >> 31;
	//CChatWindow::AddDebugMessage("FLAG REMOTE %d", flag);

	if (!flag)
	{
		uint16_t newAnim = (uint16_t)m_ofSync.dwAnimation;
		if (newAnim != m_usPlayingAnimIdx)
		{
			//CChatWindow::AddDebugMessage("applying %hu anium", newAnim);
			m_usPlayingAnimIdx = newAnim;
			m_pPlayerPed->PlayAnimByIdx(newAnim, 4.0f);
		}
		if (m_usPlayingAnimIdx)
		{
			if (!m_pPlayerPed->IsPlayingAnim(m_usPlayingAnimIdx))
			{
				m_pPlayerPed->PlayAnimByIdx(newAnim, 4.0f);
			}
		}
		m_bWasSettedFlag = true;
	}
	if (flag && m_bWasSettedFlag)
	{
		m_usPlayingAnimIdx = 0;
		// we have to reset anim
		m_pPlayerPed->PlayAnimByIdx(0, 4.0f,
									m_ofSync.animation.flags.loop,
									m_ofSync.animation.flags.freeze
		);
		m_bWasSettedFlag = false;
	}
}


uint32_t CRemotePlayer::GetPlayerColor()
{
	return TranslateColorCodeToRGBA(m_PlayerID);
}

void CRemotePlayer::ShowGlobalMarker(short sX, short sY, short sZ)
{
	if(m_dwGlobalMarkerID)
	{
		pGame->DisableMarker(m_dwGlobalMarkerID);
		m_dwGlobalMarkerID = 0;
	}

	if(!m_pPlayerPed)
	{
		m_dwGlobalMarkerID = pGame->CreateRadarMarkerIcon(0, (float)sX, (float)sY, (float)sZ, m_PlayerID, 0);
	}
}

void CRemotePlayer::HideGlobalMarker()
{
	if(m_dwGlobalMarkerID)
	{
		pGame->DisableMarker(m_dwGlobalMarkerID);
		m_dwGlobalMarkerID = 0;
	}
}

void CRemotePlayer::StateChange(uint8_t byteNewState, uint8_t byteOldState)
{
	if(byteNewState == PLAYER_STATE_DRIVER && byteOldState == PLAYER_STATE_ONFOOT)
	{
		// Если их новая машина принадлежит местному игроку
		// нам придется выгнать локального игрока
		CPlayerPed *pLocalPlayerPed = pGame->FindPlayerPed();
		VEHICLEID LocalVehicle=0xFFFF;
		MATRIX4X4 mat;

		if(pLocalPlayerPed && pLocalPlayerPed->IsInVehicle() && !pLocalPlayerPed->IsAPassenger())
		{
			LocalVehicle = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(pLocalPlayerPed->GetGtaVehicle());
			if(LocalVehicle == m_VehicleID) {
				pLocalPlayerPed->GetMatrix(&mat);
				pLocalPlayerPed->RemoveFromVehicleAndPutAt(mat.pos.X,mat.pos.Y,mat.pos.Z + 1.0f);
				pGame->DisplayGameText("~r~Car Jacked~w~!",1000,5);
			}
		}
	}
}