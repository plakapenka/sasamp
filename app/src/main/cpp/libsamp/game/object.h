#pragma once

struct RetextureMaterial
{
	RwTexture* texture;
	bool m_bUse;
};

class CObject : public CEntity
{
public:
	MATRIX4X4	m_matTarget;
	MATRIX4X4	m_matCurrent;
	uint8_t		m_byteMoving;
	float		m_fMoveSpeed;
	bool		m_bIsPlayerSurfing;
	bool		m_bNeedRotate;

	CQuaternion m_quatTarget;
	CQuaternion m_quatStart;

	MaterialInfo m_pMaterials[MAX_MATERIALS];
	bool		m_bMaterials;

	MaterialInfo m_MaterialTextTexture[MAX_MATERIALS];
    bool		m_bMaterialsText;

	CVector m_vecAttachedOffset;
	CVector m_vecAttachedRotation;
	uint16_t m_usAttachedVehicle;
	uint8_t m_bAttachedType;

	CVector 		m_vecRot;
	CVector		m_vecTargetRot;
	CVector		m_vecTargetRotTar;
	CVector		m_vecRotationTarget;
	CVector		m_vecSubRotationTarget;
	float		m_fDistanceToTargetPoint;
	uint32_t		m_dwMoveTick;

	CObject(int iModel, float fPosX, float fPosY, float fPosZ, CVector vecRot, float fDrawDistance);
	~CObject();

	void Process(float fElapsedTime);
	float DistanceRemaining(MATRIX4X4 *matPos);
	float RotaionRemaining(CVector matPos);

	void SetPos(float x, float y, float z);
	void MoveTo(float x, float y, float z, float speed, float rX, float rY, float rZ);

	void AttachToVehicle(uint16_t usVehID, CVector* pVecOffset, CVector* pVecRot);
	void ProcessAttachToVehicle(CVehicle* pVehicle);

	void InstantRotate(float x, float y, float z);
	void StopMoving();

	void ApplyMoveSpeed();
	void GetRotation(float* pfX, float* pfY, float* pfZ);
};