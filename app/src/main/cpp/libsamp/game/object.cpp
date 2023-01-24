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

CObject::CObject(int iModel, float fPosX, float fPosY, float fPosZ, VECTOR vecRot, float fDrawDistance)
{
	uint32_t dwRetID 	= 0;
	m_pEntity 			= 0;
	m_dwGTAId 			= 0;

	ScriptCommand(&create_object, iModel, fPosX, fPosY, fPosZ, &dwRetID);
	ScriptCommand(&put_object_at, dwRetID, fPosX, fPosY, fPosZ);

	m_pEntity = GamePool_Object_GetAt(dwRetID);
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

	InstantRotate(vecRot.X, vecRot.Y, vecRot.Z);
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
	m_pEntity = GamePool_Object_GetAt(m_dwGTAId);
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
	m_pEntity = GamePool_Object_GetAt(m_dwGTAId);
	if (!m_pEntity) return;
	if (!(m_pEntity->mat)) return;
	if (m_byteMoving & 1)
	{
		VECTOR vecSpeed = { 0.0f, 0.0f, 0.0f };
		MATRIX4X4 matEnt;
		GetMatrix(&matEnt);
		float distance = fElapsedTime * m_fMoveSpeed;
		float remaining = DistanceRemaining(&matEnt);
		uint32_t dwThisTick = GetTickCount();

		float posX = matEnt.pos.X;
		float posY = matEnt.pos.Y;
		float posZ = matEnt.pos.Z;

		float f1 = ((float)(dwThisTick - m_dwMoveTick)) * 0.001f * m_fMoveSpeed;
		float f2 = m_fDistanceToTargetPoint - remaining;

		if (distance >= remaining)
		{
			SetMoveSpeedVector(vecSpeed);
			SetTurnSpeedVector(vecSpeed);
			matEnt.pos.X = m_matTarget.pos.X;
			matEnt.pos.Y = m_matTarget.pos.Y;
			matEnt.pos.Z = m_matTarget.pos.Z;
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
		matEnt.pos.X += ((m_matTarget.pos.X - matEnt.pos.X) * delta);
		matEnt.pos.Y += ((m_matTarget.pos.Y - matEnt.pos.Y) * delta);
		matEnt.pos.Z += ((m_matTarget.pos.Z - matEnt.pos.Z) * delta);

		distance = remaining / m_fDistanceToTargetPoint;
		float slerpDelta = 1.0f - distance;

		delta = 1.0f / fElapsedTime;
		vecSpeed.X = (matEnt.pos.X - posX) * delta * 0.02f;
		vecSpeed.Y = (matEnt.pos.Y - posY) * delta * 0.02f;
		vecSpeed.Z = (matEnt.pos.Z - posZ) * delta * 0.02f;

		if (FloatOffset(f1, f2) > 0.1f)
		{
			if (f1 > f2)
			{
				delta = (f1 - f2) * 0.1f + 1.0f;
				vecSpeed.X *= delta;
				vecSpeed.Y *= delta;
				vecSpeed.Z *= delta;
			}

			if (f2 > f1)
			{
				delta = 1.0f - (f2 - f1) * 0.1f;
				vecSpeed.X *= delta;
				vecSpeed.Y *= delta;
				vecSpeed.Z *= delta;
			}
		}

		SetMoveSpeedVector(vecSpeed);
		ApplyMoveSpeed();

		if (m_bNeedRotate)
		{
			float fx, fy, fz;
			GetRotation(&fx, &fy, &fz);
			distance = m_vecRotationTarget.Z - distance * m_vecSubRotationTarget.Z;
			vecSpeed.X = 0.0f;
			vecSpeed.Y = 0.0f;
			vecSpeed.Z = subAngle(remaining, distance) * 0.01f;
			if (vecSpeed.Z <= 0.001f)
			{
				if (vecSpeed.Z < -0.001f)
					vecSpeed.Z = -0.001f;
			}
			else
			{
				vecSpeed.Z = 0.001f;
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
	fSX = (matPos->pos.X - m_matTarget.pos.X) * (matPos->pos.X - m_matTarget.pos.X);
	fSY = (matPos->pos.Y - m_matTarget.pos.Y) * (matPos->pos.Y - m_matTarget.pos.Y);
	fSZ = (matPos->pos.Z - m_matTarget.pos.Z) * (matPos->pos.Z - m_matTarget.pos.Z);
	return (float)sqrt(fSX + fSY + fSZ);
}

void CObject::SetPos(float x, float y, float z)
{
	if (GamePool_Object_GetAt(m_dwGTAId))
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
		mat.pos.X = m_matTarget.pos.X;
		mat.pos.Y = m_matTarget.pos.Y;
		mat.pos.Z = m_matTarget.pos.Z;

		if (m_bNeedRotate) {
			m_quatTarget.GetMatrix(&mat);
		}

		this->UpdateMatrix(mat);
	}

	m_dwMoveTick = GetTickCount();
	m_fMoveSpeed = fSpeed;
	m_matTarget.pos.X = fX;
	m_matTarget.pos.Y = fY;
	m_matTarget.pos.Z = fZ;
	m_byteMoving |= 1;

	if (fRotX <= -999.0f || fRotY <= -999.0f || fRotZ <= -999.0f) {
		m_bNeedRotate = false;
	}
	else
	{
		m_bNeedRotate = true;

		VECTOR vecRot;
		MATRIX4X4 matrix;
		this->GetRotation(&vecRot.X, &vecRot.Y, &vecRot.Z);
		m_vecRotationTarget.X = fixAngle(fRotX);
		m_vecRotationTarget.Y = fixAngle(fRotY);
		m_vecRotationTarget.Z = fixAngle(fRotZ);

		m_vecSubRotationTarget.X = subAngle(vecRot.X, fRotX);
		m_vecSubRotationTarget.Y = subAngle(vecRot.Y, fRotY);
		m_vecSubRotationTarget.Z = subAngle(vecRot.Z, fRotZ);

		this->InstantRotate(fRotX, fRotY, fRotZ);
		this->GetMatrix(&matrix);

		m_matTarget.right = matrix.right;
		m_matTarget.at = matrix.at;
		m_matTarget.up = matrix.up;

		this->InstantRotate(vecRot.X, vecRot.Y, vecRot.Z);
		this->GetMatrix(&matrix);

		m_quatStart.SetFromMatrix(matrix);
		m_quatTarget.SetFromMatrix(m_matTarget);
		m_quatStart.Normalize();
		m_quatTarget.Normalize();
	}

	m_fDistanceToTargetPoint = this->GetDistanceFromPoint(m_matTarget.pos.X, m_matTarget.pos.Y, m_matTarget.pos.Z);

	if (pNetGame) {
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool) {
			pPlayerPool->GetLocalPlayer()->UpdateSurfing();
		}
	}
}

void CObject::AttachToVehicle(uint16_t usVehID, VECTOR* pVecOffset, VECTOR* pVecRot)
{
	m_bAttachedType = 1;
	m_usAttachedVehicle = usVehID;
	m_vecAttachedOffset.X = pVecOffset->X;
	m_vecAttachedOffset.Y = pVecOffset->Y;
	m_vecAttachedOffset.Z = pVecOffset->Z;

	m_vecAttachedRotation.X = pVecRot->X;
	m_vecAttachedRotation.Y = pVecRot->Y;
	m_vecAttachedRotation.Z = pVecRot->Z;
}

void CObject::ProcessAttachToVehicle(CVehicle* pVehicle)
{
	if (GamePool_Object_GetAt(m_dwGTAId))
	{
		if (!ScriptCommand(&is_object_attached, m_dwGTAId))
		{
			ScriptCommand(&attach_object_to_car, m_dwGTAId, pVehicle->m_dwGTAId, m_vecAttachedOffset.X,
				m_vecAttachedOffset.Y, m_vecAttachedOffset.Z, m_vecAttachedRotation.X, m_vecAttachedRotation.Y, m_vecAttachedRotation.Z);
		}
	}
}

void CObject::InstantRotate(float x, float y, float z)
{
	if (GamePool_Object_GetAt(m_dwGTAId))
	{
		ScriptCommand(&set_object_rotation, m_dwGTAId, x, y, z);
	}
}

void CObject::StopMoving()
{
	VECTOR vec = { 0.0f, 0.0f, 0.0f };
	this->SetMoveSpeedVector(vec);
	this->SetTurnSpeedVector(vec);
	m_byteMoving &= ~1;
}

void CObject::ApplyMoveSpeed()
{
	if(m_pEntity)
	{
		float fTimeStep	= *(float*)(g_libGTASA + 0x8C9BB4); // 2.00 - 0x96B500

		MATRIX4X4 mat;
		GetMatrix(&mat);
		mat.pos.X += fTimeStep * m_pEntity->vecMoveSpeed.X;
		mat.pos.Y += fTimeStep * m_pEntity->vecMoveSpeed.Y;
		mat.pos.Z += fTimeStep * m_pEntity->vecMoveSpeed.Z;
		UpdateMatrix(mat);
	}
}

void CObject::GetRotation(float* pfX,float* pfY,float* pfZ)
{
	if (!m_pEntity) return;

	MATRIX4X4* mat = m_pEntity->mat;

	if(mat) CHook::CallFunction<void>(g_libGTASA + 0x3E8098 + 1, mat, pfX, pfY, pfZ, 21);

	*pfX = *pfX * 57.295776 * -1.0;
	*pfY = *pfY * 57.295776 * -1.0;
	*pfZ = *pfZ * 57.295776 * -1.0;

}