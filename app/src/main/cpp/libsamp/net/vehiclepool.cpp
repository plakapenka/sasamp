#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

extern CGame *pGame;
extern CNetGame *pNetGame;

CVehiclePool::CVehiclePool()
{
	for(auto & m_pVehicle : m_pVehicles)
	{
		m_pVehicle = nullptr;
	}
}

CVehiclePool::~CVehiclePool()
{
	for(VEHICLEID VehicleID = 0; VehicleID < MAX_VEHICLES; VehicleID++)
		Delete(VehicleID);
}

CVehicle* CVehiclePool::GetVehicleFromTrailer(CVehicle *pTrailer) {

	for(const auto & pVehicle : m_pVehicles)
	{
		if(pVehicle == nullptr || pVehicle->m_pVehicle == nullptr)
			continue;

		if(pVehicle->m_pVehicle->pTrailer == pTrailer->m_pVehicle)
			return pVehicle;
	}

	return nullptr;
}

void CVehiclePool::Process()
{

	for(const auto & pVehicle : m_pVehicles)
	{
		if (pVehicle == nullptr || pVehicle->m_pVehicle == nullptr)
			continue;

		if (pVehicle->GetHealth() < 300.0f) {
			MATRIX4X4 matrix4X4;
			pVehicle->GetMatrix(&matrix4X4);
			pVehicle->SetMatrix(matrix4X4);
			pVehicle->SetHealth(300.0f);
		}
		if (pVehicle->m_pVehicle->m_nCurrentGear == 0 &&
			pVehicle->m_pVehicle->pHandling->m_transmissionData.m_fCurrentSpeed < -0.01 &&
			pVehicle->m_bEngineOn) {
			if (pVehicle->m_pLeftReverseLight == nullptr) {
				pVehicle->toggleReverseLight(true);
			}
		} else {
			if (pVehicle->m_pLeftReverseLight != nullptr) {
				pVehicle->toggleReverseLight(false);
			}
		}
		if (pVehicle->m_iTurnState == CVehicle::eTurnState::TURN_RIGHT) {
			if (!pVehicle->m_bIsOnRightTurnLight) {
				pVehicle->toggleLeftTurnLight(false);
				pVehicle->toggleRightTurnLight(true);
			}

		} else if (pVehicle->m_iTurnState == CVehicle::eTurnState::TURN_LEFT) {
			if (!pVehicle->m_bIsOnLeftTurnLight) {
				pVehicle->toggleRightTurnLight(false);
				pVehicle->toggleLeftTurnLight(true);
			}
		} else if (pVehicle->m_iTurnState == CVehicle::eTurnState::TURN_ALL) {
			if (!pVehicle->m_bIsOnAllTurnLight) {
				pVehicle->toggleLeftTurnLight(true);
				pVehicle->toggleRightTurnLight(true);
				pVehicle->m_bIsOnAllTurnLight = true;
				pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_ALL;
			}

		} else {
			if (pVehicle->m_bIsOnRightTurnLight)
				pVehicle->toggleRightTurnLight(false);

			if (pVehicle->m_bIsOnLeftTurnLight)
				pVehicle->toggleLeftTurnLight(false);

			if (pVehicle->m_bIsOnAllTurnLight) {
				pVehicle->toggleLeftTurnLight(false);
				pVehicle->toggleRightTurnLight(false);

			}
			pVehicle->m_bIsOnAllTurnLight = false;
			pVehicle->m_iTurnState = CVehicle::eTurnState::TURN_OFF;
		}

		pVehicle->SetEngineState(pVehicle->m_bEngineOn);
		pVehicle->SetLightsState(pVehicle->m_bLightsOn);

		pVehicle->ProcessDamage();

		if (pVehicle->IsDriverLocalPlayer())
			pVehicle->SetInvulnerable(false);
		else
			pVehicle->SetInvulnerable(true);

		pVehicle->ProcessMarkers();
	}
}
#include "..//game/CCustomPlateManager.h"
#include "chatwindow.h"

bool CVehiclePool::New(NEW_VEHICLE *pNewVehicle)
{
	if(m_pVehicles[pNewVehicle->VehicleID] != nullptr)
	{
		CChatWindow::AddDebugMessage("Warning: vehicle %u was not deleted", pNewVehicle->VehicleID);
		Delete(pNewVehicle->VehicleID);
	}

	m_pVehicles[pNewVehicle->VehicleID] = new	CVehicle(pNewVehicle->iVehicleType,
														  pNewVehicle->vecPos.x,
														  pNewVehicle->vecPos.y,
														  pNewVehicle->vecPos.z,
														  pNewVehicle->fRotation,
														  pNewVehicle->byteAddSiren);

	if(m_pVehicles[pNewVehicle->VehicleID])
	{
		// colors
		if(pNewVehicle->aColor1 != -1 || pNewVehicle->aColor2 != -1)
		{
			m_pVehicles[pNewVehicle->VehicleID]->SetColor(
				pNewVehicle->aColor1, pNewVehicle->aColor2 );
		}

		// health
		m_pVehicles[pNewVehicle->VehicleID]->SetHealth(pNewVehicle->fHealth);

		// interior
		if(pNewVehicle->byteInterior > 0)
			LinkToInterior(pNewVehicle->VehicleID, pNewVehicle->byteInterior);

		// damage status
		if(pNewVehicle->dwPanelDamageStatus ||
		   pNewVehicle->dwDoorDamageStatus ||
		   pNewVehicle->byteLightDamageStatus)
		{
			m_pVehicles[pNewVehicle->VehicleID]->UpdateDamageStatus(
					pNewVehicle->dwPanelDamageStatus,
					pNewVehicle->dwDoorDamageStatus,
					pNewVehicle->byteLightDamageStatus, pNewVehicle->byteTireDamageStatus);
		}

		m_bIsWasted[pNewVehicle->VehicleID] = false;

		return true;
	}

	return false;
}

bool CVehiclePool::Delete(VEHICLEID VehicleID)
{
	delete m_pVehicles[VehicleID];
	m_pVehicles[VehicleID] = nullptr;

	return true;
}

VEHICLEID CVehiclePool::findSampIdFromGtaPtr(CVehicleGta *pGtaVehicle)
{
	int x=1;

	while(x != MAX_VEHICLES) 
	{
		if(m_pVehicles[x] && pGtaVehicle == m_pVehicles[x]->m_pVehicle) return x;
		x++;
	}

	return INVALID_VEHICLE_ID;
}

CVehicle* CVehiclePool::FindVehicle(CVehicleGta *pGtaVehicle)
{
	for(const auto & i : m_pVehicles)
	{
		if(i->m_pVehicle != nullptr && i->m_pVehicle == pGtaVehicle)
			return i;
	}
	return nullptr;
}

VEHICLEID CVehiclePool::FindIDFromRwObject(RwObject* pRWObject)
{
	int x = 1;

	while (x != MAX_VEHICLES)
	{
		if (m_pVehicles[x] && m_pVehicles[x]->m_pVehicle)
		{
			if (pRWObject == (RwObject*)(m_pVehicles[x]->m_pVehicle->m_pRwObject)) return x;
		}
		x++;
	}

	return INVALID_VEHICLE_ID;
}

int CVehiclePool::FindGtaIDFromID(int id)
{
	if(m_pVehicles[id]->m_pVehicle)
		return GamePool_Vehicle_GetIndex(m_pVehicles[id]->m_pVehicle);
	else
		return INVALID_VEHICLE_ID;
}

CVehicle* CVehiclePool::FindNearestToLocalPlayerPed()
{
	float fLeastDistance = 10000.0f;
	float fThisDistance = 0.0f;

	CVehicle* ClosetSoFar = nullptr;

	for(const auto & pVehicle : m_pVehicles)
	{
		if(pVehicle == nullptr || pVehicle->m_pVehicle == nullptr)
			continue;

		fThisDistance = pVehicle->GetDistanceFromLocalPlayerPed();
		if(fThisDistance < fLeastDistance)
		{
			fLeastDistance = fThisDistance;
			ClosetSoFar = pVehicle;
		}
	}

	return ClosetSoFar;
}

void CVehiclePool::LinkToInterior(VEHICLEID VehicleID, int iInterior)
{
	m_pVehicles[VehicleID]->LinkToInterior(iInterior);
}

void CVehiclePool::NotifyVehicleDeath(VEHICLEID VehicleID)
{
	if(pNetGame->GetPlayerPool()->GetLocalPlayer()->m_LastVehicle != VehicleID) return;
	Log("CVehiclePool::NotifyVehicleDeath");

	RakNet::BitStream bsDeath;
	bsDeath.Write(VehicleID);
	pNetGame->GetRakClient()->RPC(&RPC_VehicleDestroyed, &bsDeath, HIGH_PRIORITY, RELIABLE_SEQUENCED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
	pNetGame->GetPlayerPool()->GetLocalPlayer()->m_LastVehicle = INVALID_VEHICLE_ID;
}

void CVehiclePool::AssignSpecialParamsToVehicle(VEHICLEID VehicleID, uint8_t byteObjective, uint8_t byteDoorsLocked)
{
	CVehicle *pVehicle = m_pVehicles[VehicleID];

	if(pVehicle)
	{
		pVehicle->m_byteObjectiveVehicle = byteObjective;

		pVehicle->SetDoorState(byteDoorsLocked);
	}
}