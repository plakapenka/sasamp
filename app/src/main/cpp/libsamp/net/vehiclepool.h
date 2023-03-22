#pragma once

typedef struct _NEW_VEHICLE
{
	VEHICLEID 	VehicleID;
	int 		iVehicleType;
	CVector		vecPos;
	float 		fRotation;
	uint8_t		aColor1;
	uint8_t		aColor2;
	float		fHealth;
	uint8_t		byteInterior;
	uint32_t	dwDoorDamageStatus;
	uint32_t 	dwPanelDamageStatus;
	uint8_t		byteLightDamageStatus;
	uint8_t		byteTireDamageStatus;
	uint8_t		byteAddSiren;
	uint8_t		byteModSlots[14];
	uint8_t	  	bytePaintjob;
	uint32_t 	cColor1;
	int	cColor2;
} NEW_VEHICLE;

class CVehiclePool
{
public:
	CVehiclePool();
	~CVehiclePool();

	void Process();

	bool New(NEW_VEHICLE* pNewVehicle);
	bool Delete(VEHICLEID VehicleID);

	CVehicle* GetAt(VEHICLEID vehicleId)
	{
		if(vehicleId > MAX_VEHICLES) return nullptr;
		return m_pVehicles[vehicleId];
	}


	VEHICLEID findSampIdFromGtaPtr(CVehicleGta * pGtaVehicle);
	VEHICLEID FindIDFromRwObject(RwObject* pRWObject);
	int FindGtaIDFromID(int ID);
	
	void AssignSpecialParamsToVehicle(VEHICLEID VehicleID, uint8_t byteObjective, uint8_t byteDoorsLocked);

	CVehicle* FindNearestToLocalPlayerPed();

	void LinkToInterior(VEHICLEID VehicleID, int iInterior);

	void NotifyVehicleDeath(VEHICLEID VehicleID);

	CVehicle*		m_pVehicles[MAX_VEHICLES];

	CVehicle *GetVehicleFromTrailer(CVehicle *pTrailer);

	CVehicle *FindVehicle(CVehicleGta *pGtaVehicle);

private:
	bool			m_bIsWasted[MAX_VEHICLES];

};