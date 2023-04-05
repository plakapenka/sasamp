#pragma once

#include "game/Enums/CamMode.h"

enum class eSwitchType : uint16_t {
	NONE,
	INTERPOLATION,
	JUMPCUT
};

class CCamera
{
public:
	CCamera() {
		m_matPos = (RwMatrix*)(g_libGTASA + 0x951FA8 + 0x8FC);
		m_f3rdPersonCHairMultX = (float*)(g_libGTASA + 0x00952CB0);
		m_f3rdPersonCHairMultY = (float*)(g_libGTASA + 0x00952CB4);

		TheCamera = (uintptr_t*)(g_libGTASA + 0x00951FA8);
	}

	~CCamera() {}

	// 0.3.7
	void SetBehindPlayer();
	// 0.3.7
	void Restore();
	// 0.3.7
	void SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ);
	// 0.3.7
	void LookAtPoint(float fX, float fY, float fZ, int iType);

	void GetMatrix(RwMatrix* mat);
	// 0.3.7
	void InterpolateCameraPos(CVector *posFrom, CVector *posTo, int time, uint8_t mode);
	// 0.3.7
	void InterpolateCameraLookAt(CVector *posFrom, CVector *posTo, int time, uint8_t mode);

	static void TakeControl(uintptr_t *thiz, CEntityGta *NewTarget, eCamMode CamMode, eSwitchType CamSwitchStyle, int32_t WhoIsTakingControl);

private:
	CEntityGta* m_pEntity;
	RwMatrix *m_matPos;

public:
	uintptr_t* TheCamera; // this

	static float* m_f3rdPersonCHairMultX;
	static float* m_f3rdPersonCHairMultY;
};