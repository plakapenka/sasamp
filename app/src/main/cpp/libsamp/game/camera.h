#pragma once

class CCamera
{
public:
	CCamera() { m_matPos = (RwMatrix*)(g_libGTASA+0x8B1104); }
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
	void InterpolateCameraPos(VECTOR *posFrom, VECTOR *posTo, int time, uint8_t mode);
	// 0.3.7
	void InterpolateCameraLookAt(VECTOR *posFrom, VECTOR *posTo, int time, uint8_t mode);

private:
	ENTITY_TYPE* m_pEntity;
	RwMatrix *m_matPos;
};