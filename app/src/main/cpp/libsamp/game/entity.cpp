#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "../chatwindow.h"
#include "StreamingInfo.h"

#include <cmath>

extern CGame *pGame;
extern CNetGame *pNetGame;

void CEntity::Add()
{
	if (!m_pEntity || CUtil::IsGameEntityArePlaceable(m_pEntity))
	{
		return;
	}

	if (!m_pEntity->m_nStatus)
	{
		CVector vec = { 0.0f,0.0f,0.0f };
		SetMoveSpeedVector(vec);
		SetTurnSpeedVector(vec);

		WorldAddEntity((uintptr_t)m_pEntity);

		RwMatrix mat;
		GetMatrix(&mat);
		TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);
	}
}

void CEntity::SetGravityProcessing(bool bProcess)
{
}

// 0.3.7
void CEntity::SetMoveSpeedVector(CVector vec)
{
	if (!m_pEntity) return;
	m_pEntity->vecMoveSpeed = vec;
}

void CEntity::SetTurnSpeedVector(CVector vec)
{
	if (!m_pEntity) return;
	m_pEntity->m_vecTurnSpeed = vec;
}

void CEntity::GetMoveSpeedVector(CVector *vec)
{
	if (!m_pEntity) return;
	vec->x = m_pEntity->vecMoveSpeed.x;
	vec->y = m_pEntity->vecMoveSpeed.y;
	vec->z = m_pEntity->vecMoveSpeed.z;
}

void CEntity::GetTurnSpeedVector(CVector *vec)
{
	if (!m_pEntity) return;
	vec->x = m_pEntity->m_vecTurnSpeed.x;
	vec->y = m_pEntity->m_vecTurnSpeed.y;
	vec->z = m_pEntity->m_vecTurnSpeed.z;
}

void CEntity::UpdateRwMatrixAndFrame()
{
	if (m_pEntity && !CUtil::IsGameEntityArePlaceable(m_pEntity))
	{
		if (m_pEntity->m_pRwObject)
		{
			if (m_pEntity->mat)
			{
				uintptr_t pRwMatrix = *(uintptr_t*)(m_pEntity->m_pRwObject + 4) + 0x10;
				// CMatrix::UpdateRwMatrix
				((void (*) (RwMatrix*, uintptr_t))(g_libGTASA + 0x0044EE3E + 1))(m_pEntity->mat, pRwMatrix);

				// CEntity::UpdateRwFrame
				((void (*) (CEntityGta*))(g_libGTASA + 0x003EC038 + 1))(m_pEntity);
			}
		}
	}
}

void CEntity::RemovePhysical()
{
	((void (*)(CEntityGta*))(*(void**)(m_pEntity->vtable + 16)))(m_pEntity); // CPhysical::Remove
}

void CEntity::AddPhysical()
{
	((void (*)(CEntityGta*))(*(void**)(m_pEntity->vtable + 8)))(m_pEntity); // CPhysical::Add
}

void CEntity::UpdateMatrix(RwMatrix mat)
{
	if (m_pEntity)
	{
		if (m_pEntity->mat)
		{
			// CPhysical::Remove
			((void (*)(CEntityGta*))(*(uintptr_t*)(m_pEntity->vtable + 0x10)))(m_pEntity);

			SetMatrix(mat);
			UpdateRwMatrixAndFrame();

			// CPhysical::Add
			((void (*)(CEntityGta*))(*(uintptr_t*)(m_pEntity->vtable + 0x8)))(m_pEntity);
		}
	}
}

bool CEntity::GetCollisionChecking()
{
	if(!m_pEntity || CUtil::IsGameEntityArePlaceable(m_pEntity))
		return true;

	return m_pEntity->nEntityFlags.m_bCollisionProcessed;
}

void CEntity::SetCollisionChecking(bool bCheck)
{
	if(!m_pEntity || CUtil::IsGameEntityArePlaceable(m_pEntity))
		return;

	m_pEntity->nEntityFlags.m_bCollisionProcessed = bCheck;
}

void CEntity::Render()
{
	auto pRwObject = m_pEntity->m_pRpClump;

	int iModel = GetModelIndex();
	if(iModel >= 400 && iModel <= 611 && pRwObject)
	{
		// CVisibilityPlugins::SetupVehicleVariables
		((void (*)(uintptr_t))(g_libGTASA + 0x005D4B90 + 1))(pRwObject);
	}

	// CEntity::PreRender
	(( void (*)(CEntityGta*))(*(void**)(m_pEntity->vtable+0x48)))(m_pEntity);

	// CRenderer::RenderOneNonRoad
	(( void (*)(CEntityGta*))(g_libGTASA + 0x0041030C + 1))(m_pEntity);
}

void CEntity::Remove()
{
    if(!m_pEntity || CUtil::IsGameEntityArePlaceable(m_pEntity) )
        return;

    CUtil::WorldRemoveEntity((uintptr_t)m_pEntity);
}

// 0.3.7
void CEntity::GetMatrix(RwMatrix* Matrix)
{
	if (!m_pEntity || !m_pEntity->mat) return;

	Matrix->right.x = m_pEntity->mat->right.x;
	Matrix->right.y = m_pEntity->mat->right.y;
	Matrix->right.z = m_pEntity->mat->right.z;

	Matrix->up.x = m_pEntity->mat->up.x;
	Matrix->up.y = m_pEntity->mat->up.y;
	Matrix->up.z = m_pEntity->mat->up.z;

	Matrix->at.x = m_pEntity->mat->at.x;
	Matrix->at.y = m_pEntity->mat->at.y;
	Matrix->at.z = m_pEntity->mat->at.z;

	Matrix->pos.x = m_pEntity->mat->pos.x;
	Matrix->pos.y = m_pEntity->mat->pos.y;
	Matrix->pos.z = m_pEntity->mat->pos.z;
}

// 0.3.7
void CEntity::SetMatrix(RwMatrix Matrix)
{
	if (!m_pEntity) return;
	if (!m_pEntity->mat) return;

	m_pEntity->mat->right.x = Matrix.right.x;
	m_pEntity->mat->right.y = Matrix.right.y;
	m_pEntity->mat->right.z = Matrix.right.z;

	m_pEntity->mat->up.x = Matrix.up.x;
	m_pEntity->mat->up.y = Matrix.up.y;
	m_pEntity->mat->up.z = Matrix.up.z;

	m_pEntity->mat->at.x = Matrix.at.x;
	m_pEntity->mat->at.y = Matrix.at.y;
	m_pEntity->mat->at.z = Matrix.at.z;

	m_pEntity->mat->pos.x = Matrix.pos.x;
	m_pEntity->mat->pos.y = Matrix.pos.y;
	m_pEntity->mat->pos.z = Matrix.pos.z;
}

// 0.3.7
uint16_t CEntity::GetModelIndex()
{
	if (!m_pEntity)
	{
		return 0;
	}
	return m_pEntity->nModelIndex;
}

// 0.3.7
bool CEntity::IsAdded()
{
	if(!m_pEntity)
		return false;

	if(CUtil::IsGameEntityArePlaceable(m_pEntity))
		return false;

	return true;
}

// 0.3.7
bool CEntity::SetModelIndex(unsigned int uiModel)
{
	if(!m_pEntity) return false;

	int iTryCount = 0;
	if(!pGame->IsModelLoaded(uiModel) && !IsValidModel(uiModel))
	{
		CStreaming::RequestModel(uiModel, STREAMING_GAME_REQUIRED);
		CStreaming::LoadAllRequestedModels(false);
		while(!pGame->IsModelLoaded(uiModel))
		{
			usleep(1000);
			if(iTryCount > 200)
			{
				CChatWindow::AddDebugMessage("Warning: Model %u wouldn't load in time!", uiModel);
				return false;
			}

			iTryCount++;
		}
	}

	// CEntity::DeleteRWObject()
	(( void (*)(CEntityGta*))(*(void**)(m_pEntity->vtable+0x24)))(m_pEntity);

	m_pEntity->nModelIndex = uiModel;

	// CEntity::SetModelIndex()
	(( void (*)(CEntityGta*, unsigned int))(*(void**)(m_pEntity->vtable+0x18)))(m_pEntity, uiModel);

	return true;
}

// 0.3.7
void CEntity::TeleportTo(float fX, float fY, float fZ)
{
	if(m_pEntity && !CUtil::IsGameEntityArePlaceable(m_pEntity)) /* CPlaceable */
	{
		uint16_t modelIndex = m_pEntity->nModelIndex;
		if(	modelIndex != TRAIN_PASSENGER_LOCO &&
			modelIndex != TRAIN_FREIGHT_LOCO &&
			modelIndex != TRAIN_TRAM)
			(( void (*)(CEntityGta*, float, float, float, bool))(*(void**)(m_pEntity->vtable+0x3C)))(m_pEntity, fX, fY, fZ, 0);
		else
			ScriptCommand(&put_train_at, m_dwGTAId, fX, fY, fZ);
	}
}

float CEntity::GetDistanceFromCamera()
{ // not v2.1
	RwMatrix matEnt;

	if(!m_pEntity || CUtil::IsGameEntityArePlaceable(m_pEntity))
		return 100000.0f;

	this->GetMatrix(&matEnt);
	RwMatrix matCam;
	pGame->GetCamera()->GetMatrix(&matCam);

	float tmpX = (matEnt.pos.x - matCam.pos.x);
	float tmpY = (matEnt.pos.y - matCam.pos.y);
	float tmpZ = (matEnt.pos.z - matCam.pos.z);

	return sqrt( tmpX*tmpX + tmpY*tmpY + tmpZ*tmpZ );
}

float CEntity::GetDistanceFromLocalPlayerPed()
{
	RwMatrix	matFromPlayer;
	RwMatrix	matThis;
	float 		fSX, fSY, fSZ;

	CPlayerPed *pLocalPlayerPed = pGame->FindPlayerPed();
	CLocalPlayer *pLocalPlayer  = nullptr;

	if(!pLocalPlayerPed) return 10000.0f;

	GetMatrix(&matThis);

	if(pNetGame)
	{
		pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		if(pLocalPlayer && (pLocalPlayer->IsSpectating() || pLocalPlayer->IsInRCMode()))
		{
			pGame->GetCamera()->GetMatrix(&matFromPlayer);
		}
		else
		{
			pLocalPlayerPed->GetMatrix(&matFromPlayer);
		}
	}
	else
	{
		pLocalPlayerPed->GetMatrix(&matFromPlayer);
	}

	fSX = (matThis.pos.x - matFromPlayer.pos.x) * (matThis.pos.x - matFromPlayer.pos.x);
	fSY = (matThis.pos.y - matFromPlayer.pos.y) * (matThis.pos.y - matFromPlayer.pos.y);
	fSZ = (matThis.pos.z - matFromPlayer.pos.z) * (matThis.pos.z - matFromPlayer.pos.z);

	return (float)sqrt(fSX + fSY + fSZ);
}

float CEntity::GetDistanceFromPoint(float X, float Y, float Z)
{
	RwMatrix	matThis;
	float		fSX,fSY,fSZ;

	GetMatrix(&matThis);
	fSX = (matThis.pos.x - X) * (matThis.pos.x - X);
	fSY = (matThis.pos.y - Y) * (matThis.pos.y - Y);
	fSZ = (matThis.pos.z - Z) * (matThis.pos.z - Z);
	
	return (float)sqrt(fSX + fSY + fSZ);
}