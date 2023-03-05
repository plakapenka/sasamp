#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include "util/patch.h"
#include <cmath>

extern CGame *pGame;
extern CNetGame *pNetGame;

float fixAngle(float angle)
{
	if (angle > 180.0f) angle -= 360.0f;
	if (angle < -180.0f) angle += 360.0f;
	return angle;
}

float subAngle(float a1, float a2)
{
	return fixAngle(fixAngle(a2) - a1);
}

CObject::CObject(int iModel, float fPosX, float fPosY, float fPosZ, CVector vecRot, float fDrawDistance)
{
	uint32_t dwRetID 	= 0;
	m_pEntity 			= 0;
	m_dwGTAId 			= 0;

	ScriptCommand(&create_object, iModel, fPosX, fPosY, fPosZ, &dwRetID);
	ScriptCommand(&put_object_at, dwRetID, fPosX, fPosY, fPosZ);

	m_pEntity = CUtil::GetPoolObj(dwRetID);
	m_dwGTAId = dwRetID;
	m_byteMoving = 0;
	m_fMoveSpeed = 0.0;

	m_bIsPlayerSurfing = false;
	m_bNeedRotate = false;

	m_bMaterials = false;

	m_bAttachedType = 0;
	m_usAttachedVehicle = 0xFFFF;

	for (int i = 0; i < MAX_MATERIALS; i++)
	{
		m_pMaterials[i].m_bCreated = 0;
		m_pMaterials[i].pTex = nullptr;
	}

	InstantRotate(vecRot.x, vecRot.y, vecRot.z);
}
// todo
CObject::~CObject()
{
	m_bMaterials = false;
	for (int i = 0; i < MAX_MATERIALS; i++)
	{
		if (m_pMaterials[i].m_bCreated && m_pMaterials[i].pTex)
		{
			m_pMaterials[i].m_bCreated = 0;
			RwTextureDestroy(m_pMaterials[i].pTex);
			m_pMaterials[i].pTex = nullptr;
		}
	}
	m_pEntity = CUtil::GetPoolObj(m_dwGTAId);
	if(m_pEntity)
		ScriptCommand(&destroy_object, m_dwGTAId);
}

void CObject::Process(float fElapsedTime)
{
	if (m_bAttachedType == 1)
	{
		CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(m_usAttachedVehicle);
		if (pVehicle)
		{
			if (pVehicle->IsAdded())
			{
				ProcessAttachToVehicle(pVehicle);
			}
		}
	}
	m_pEntity = CUtil::GetPoolObj(m_dwGTAId);
	if (!m_pEntity) return;
	if (!(m_pEntity->mat)) return;
	if (m_byteMoving & 1)
	{
		CVector vecSpeed = { 0.0f, 0.0f, 0.0f };
		MATRIX4X4 matEnt;
		GetMatrix(&matEnt);
		float distance = fElapsedTime * m_fMoveSpeed;
		float remaining = DistanceRemaining(&matEnt);
		uint32_t dwThisTick = GetTickCount();

		float posX = matEnt.pos.x;
		float posY = matEnt.pos.y;
		float posZ = matEnt.pos.z;

		float f1 = ((float)(dwThisTick - m_dwMoveTick)) * 0.001f * m_fMoveSpeed;
		float f2 = m_fDistanceToTargetPoint - remaining;

		if (distance >= remaining)
		{
			SetMoveSpeedVector(vecSpeed);
			SetTurnSpeedVector(vecSpeed);
			matEnt.pos.x = m_matTarget.pos.x;
			matEnt.pos.y = m_matTarget.pos.y;
			matEnt.pos.z = m_matTarget.pos.z;
			if (m_bNeedRotate) {
				m_quatTarget.GetMatrix(&matEnt);
			}
			UpdateMatrix(matEnt);
			StopMoving();
			return;
		}

		if (fElapsedTime <= 0.0f)
			return;

		float delta = 1.0f / (remaining / distance);
		matEnt.pos.x += ((m_matTarget.pos.x - matEnt.pos.x) * delta);
		matEnt.pos.y += ((m_matTarget.pos.y - matEnt.pos.y) * delta);
		matEnt.pos.z += ((m_matTarget.pos.z - matEnt.pos.z) * delta);

		distance = remaining / m_fDistanceToTargetPoint;
		float slerpDelta = 1.0f - distance;

		delta = 1.0f / fElapsedTime;
		vecSpeed.x = (matEnt.pos.x - posX) * delta * 0.02f;
		vecSpeed.y = (matEnt.pos.y - posY) * delta * 0.02f;
		vecSpeed.z = (matEnt.pos.z - posZ) * delta * 0.02f;

		if (FloatOffset(f1, f2) > 0.1f)
		{
			if (f1 > f2)
			{
				delta = (f1 - f2) * 0.1f + 1.0f;
				vecSpeed.x *= delta;
				vecSpeed.y *= delta;
				vecSpeed.z *= delta;
			}

			if (f2 > f1)
			{
				delta = 1.0f - (f2 - f1) * 0.1f;
				vecSpeed.x *= delta;
				vecSpeed.y *= delta;
				vecSpeed.z *= delta;
			}
		}

		SetMoveSpeedVector(vecSpeed);
		ApplyMoveSpeed();

		if (m_bNeedRotate)
		{
			float fx, fy, fz;
			GetRotation(&fx, &fy, &fz);
			distance = m_vecRotationTarget.z - distance * m_vecSubRotationTarget.z;
			vecSpeed.x = 0.0f;
			vecSpeed.y = 0.0f;
			vecSpeed.z = subAngle(remaining, distance) * 0.01f;
			if (vecSpeed.z <= 0.001f)
			{
				if (vecSpeed.z < -0.001f)
					vecSpeed.z = -0.001f;
			}
			else
			{
				vecSpeed.z = 0.001f;
			}

			SetTurnSpeedVector(vecSpeed);
			GetMatrix(&matEnt);
			CQuaternion quat;
			quat.Slerp(&m_quatStart, &m_quatTarget, slerpDelta);
			quat.Normalize();
			quat.GetMatrix(&matEnt);
		}
		else
		{
			GetMatrix(&matEnt);
		}

		UpdateMatrix(matEnt);
	}
}

float CObject::DistanceRemaining(MATRIX4X4 *matPos)
{

	float	fSX,fSY,fSZ;
	fSX = (matPos->pos.x - m_matTarget.pos.x) * (matPos->pos.x - m_matTarget.pos.x);
	fSY = (matPos->pos.y - m_matTarget.pos.y) * (matPos->pos.y - m_matTarget.pos.y);
	fSZ = (matPos->pos.z - m_matTarget.pos.z) * (matPos->pos.z - m_matTarget.pos.z);
	return (float)sqrt(fSX + fSY + fSZ);
}

void CObject::SetPos(float x, float y, float z)
{
	if (CUtil::GetPoolObj(m_dwGTAId))
	{
		ScriptCommand(&put_object_at, m_dwGTAId, x, y, z);
	}
}

void CObject::MoveTo(float fX, float fY, float fZ, float fSpeed, float fRotX, float fRotY, float fRotZ)
{
	MATRIX4X4 mat;
	this->GetMatrix(&mat);

	if (m_byteMoving & 1) {
		this->StopMoving();
		mat.pos.x = m_matTarget.pos.x;
		mat.pos.y = m_matTarget.pos.y;
		mat.pos.z = m_matTarget.pos.z;

		if (m_bNeedRotate) {
			m_quatTarget.GetMatrix(&mat);
		}

		this->UpdateMatrix(mat);
	}

	m_dwMoveTick = GetTickCount();
	m_fMoveSpeed = fSpeed;
	m_matTarget.pos.x = fX;
	m_matTarget.pos.y = fY;
	m_matTarget.pos.z = fZ;
	m_byteMoving |= 1;

	if (fRotX <= -999.0f || fRotY <= -999.0f || fRotZ <= -999.0f) {
		m_bNeedRotate = false;
	}
	else
	{
		m_bNeedRotate = true;

		CVector vecRot;
		MATRIX4X4 matrix;
		this->GetRotation(&vecRot.x, &vecRot.y, &vecRot.z);
		m_vecRotationTarget.x = fixAngle(fRotX);
		m_vecRotationTarget.y = fixAngle(fRotY);
		m_vecRotationTarget.z = fixAngle(fRotZ);

		m_vecSubRotationTarget.x = subAngle(vecRot.x, fRotX);
		m_vecSubRotationTarget.y = subAngle(vecRot.y, fRotY);
		m_vecSubRotationTarget.z = subAngle(vecRot.z, fRotZ);

		this->InstantRotate(fRotX, fRotY, fRotZ);
		this->GetMatrix(&matrix);

		m_matTarget.right = matrix.right;
		m_matTarget.at = matrix.at;
		m_matTarget.up = matrix.up;

		this->InstantRotate(vecRot.x, vecRot.y, vecRot.z);
		this->GetMatrix(&matrix);

		m_quatStart.SetFromMatrix(matrix);
		m_quatTarget.SetFromMatrix(m_matTarget);
		m_quatStart.Normalize();
		m_quatTarget.Normalize();
	}

	m_fDistanceToTargetPoint = this->GetDistanceFromPoint(m_matTarget.pos.x, m_matTarget.pos.y, m_matTarget.pos.z);

	if (pNetGame) {
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool) {
			pPlayerPool->GetLocalPlayer()->UpdateSurfing();
		}
	}
}

void CObject::AttachToVehicle(uint16_t usVehID, CVector* pVecOffset, CVector* pVecRot)
{
	m_bAttachedType = 1;
	m_usAttachedVehicle = usVehID;
	m_vecAttachedOffset.x = pVecOffset->x;
	m_vecAttachedOffset.y = pVecOffset->y;
	m_vecAttachedOffset.z = pVecOffset->z;

	m_vecAttachedRotation.x = pVecRot->y;
	m_vecAttachedRotation.y = pVecRot->y;
	m_vecAttachedRotation.z = pVecRot->z;
}

void CObject::ProcessAttachToVehicle(CVehicle* pVehicle)
{
	if (CUtil::GetPoolObj(m_dwGTAId))
	{
		if (!ScriptCommand(&is_object_attached, m_dwGTAId))
		{
			ScriptCommand(&attach_object_to_car, m_dwGTAId, pVehicle->m_dwGTAId, m_vecAttachedOffset.x,
				m_vecAttachedOffset.y, m_vecAttachedOffset.z, m_vecAttachedRotation.x, m_vecAttachedRotation.y, m_vecAttachedRotation.z);
		}
	}
}

void CObject::InstantRotate(float x, float y, float z)
{
	if (CUtil::GetPoolObj(m_dwGTAId))
	{
		ScriptCommand(&set_object_rotation, m_dwGTAId, x, y, z);
	}
}

void CObject::StopMoving()
{
	CVector vec = { 0.0f, 0.0f, 0.0f };
	this->SetMoveSpeedVector(vec);
	this->SetTurnSpeedVector(vec);
	m_byteMoving &= ~1;
}

void CObject::ApplyMoveSpeed()
{
	if(m_pEntity)
	{
		float fTimeStep	= *(float*)(g_libGTASA + 0x0096B500); // 2.00 - 0x96B500

		MATRIX4X4 mat;
		GetMatrix(&mat);
		mat.pos.x += fTimeStep * m_pEntity->vecMoveSpeed.x;
		mat.pos.y += fTimeStep * m_pEntity->vecMoveSpeed.y;
		mat.pos.z += fTimeStep * m_pEntity->vecMoveSpeed.z;
		UpdateMatrix(mat);
	}
}

void CObject::GetRotation(float* pfX,float* pfY,float* pfZ)
{
	if (!m_pEntity) return;

	MATRIX4X4* mat = m_pEntity->mat;

	if(mat) CHook::CallFunction<void>(g_libGTASA + 0x0044E6AC + 1, mat, pfX, pfY, pfZ, 21);

	*pfX = *pfX * 57.295776 * -1.0;
	*pfY = *pfY * 57.295776 * -1.0;
	*pfZ = *pfZ * 57.295776 * -1.0;

}