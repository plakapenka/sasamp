#include "../main.h"
#include "game.h"
#include "camera.h"


float* CCamera::m_f3rdPersonCHairMultX;
float* CCamera::m_f3rdPersonCHairMultY;

// 0.3.7
void CCamera::SetBehindPlayer()
{
	ScriptCommand(&lock_camera_position, 0);
  	ScriptCommand(&restore_camera_to_user);
  	ScriptCommand(&set_camera_behind_player);
  	ScriptCommand(&restore_camera_jumpcut);
}

// 0.3.7
void CCamera::Restore()
{
	ScriptCommand(&restore_camera_jumpcut);
}

// 0.3.7
void CCamera::SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ)
{
	ScriptCommand(&restore_camera_to_user);
	ScriptCommand(&set_camera_position, fX, fY, fZ, fRotationX, fRotationY, fRotationZ);
}


// 0.3.7
void CCamera::LookAtPoint(float fX, float fY, float fZ, int iType)
{
	ScriptCommand(&restore_camera_to_user);
	ScriptCommand(&point_camera, fX, fY, fZ, iType);
}

void CCamera::GetMatrix(RwMatrix* Matrix)
{
	Matrix->right.x = m_matPos->right.x;
	Matrix->right.y = m_matPos->right.y;
	Matrix->right.z = m_matPos->right.z;

	Matrix->up.x = m_matPos->up.x;
	Matrix->up.y = m_matPos->up.y;
	Matrix->up.z = m_matPos->up.z;

	Matrix->at.x = m_matPos->at.x;
	Matrix->at.y = m_matPos->at.y;
	Matrix->at.z = m_matPos->at.z;

	Matrix->pos.x = m_matPos->pos.x;
	Matrix->pos.y = m_matPos->pos.y;
	Matrix->pos.z = m_matPos->pos.z;
}

// 0.3.7
void CCamera::InterpolateCameraPos(CVector *posFrom, CVector *posTo, int time, uint8_t mode)
{
	ScriptCommand(&restore_camera_to_user);
	ScriptCommand(&lock_camera_position1, 1);
	ScriptCommand(
			&set_camera_pos_time_smooth,
				  posFrom->x, posFrom->y, posFrom->z,
				  posTo->x, posTo->y, posTo->z,
				  time,
				  mode);
}

// 0.3.7
void CCamera::InterpolateCameraLookAt(CVector *posFrom, CVector *posTo, int time, uint8_t mode)
{
	ScriptCommand(&lock_camera_position, 1);
	ScriptCommand(&point_camera_transverse, posFrom->x, posFrom->y, posFrom->z, posTo->x, posTo->y, posTo->z, time, mode);
}

void CCamera::TakeControl(uintptr_t *thiz, CEntityGta *NewTarget, eCamMode CamMode, eSwitchType CamSwitchStyle, int32_t WhoIsTakingControl)
{
	((void(*)(uintptr_t*, CEntityGta*, eCamMode, eSwitchType, int32_t))(g_libGTASA + 0x003E1714 + 1))(thiz, NewTarget, CamMode, CamSwitchStyle, WhoIsTakingControl);
}
