#pragma once

#include "CStats.h"
#include <map>

class CRemoteDataStorage
{
private:
	float StatsTypesFloat[MAX_FLOAT_STATS];

public:
	CRemoteDataStorage()
	{
		memset(StatsTypesFloat, 0, sizeof(StatsTypesFloat));
	}

	float* GetStatsTypesFloat() { return StatsTypesFloat; }
	void SetStatsTypesFloat(float* statsTypesFloat) { memcpy(StatsTypesFloat, statsTypesFloat, sizeof(StatsTypesFloat)); }
};

class CRemoteData
{
public:
	static void AddRemoteDataStorage(PED_TYPE* pPed, CRemoteDataStorage* pData);
	static void RemoveRemoteDataStorage(PED_TYPE* pPed);
	static CRemoteDataStorage* GetRemoteDataStorage(PED_TYPE* pPed);

private:
	static std::map<PED_TYPE*, CRemoteDataStorage*> m_mapRemoteDataStorage;

};