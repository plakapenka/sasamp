#pragma once

class CCamera
{
public:
	CCamera() { m_matPos = (MATRIX4X4*)(g_libGTASA + 0x951FA8 + 0x8FC); }
	~CCamera() {}

	// 0.3.7
	void SetBehindPlayer();
	// 0.3.7
	void Restore();
	// 0.3.7
	void SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ);
	// 0.3.7
	void LookAtPoint(float fX, float fY, float fZ, int iType);

	void GetMatrix(PMATRIX4X4 mat);
	// 0.3.7
	void InterpolateCameraPos(CVector *posFrom, CVector *posTo, int time, uint8_t mode);
	// 0.3.7
	void InterpolateCameraLookAt(CVector *posFrom, CVector *posTo, int time, uint8_t mode);

private:
	CEntityGta* m_pEntity;
	MATRIX4X4 *m_matPos;
};