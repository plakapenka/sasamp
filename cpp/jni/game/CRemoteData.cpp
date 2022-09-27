#include "../main.h"
#include "game.h"
#include "CRemoteData.h"

std::map<PED_TYPE*, CRemoteDataStorage*> CRemoteData::m_mapRemoteDataStorage{};

void CRemoteData::AddRemoteDataStorage(PED_TYPE* pPed, CRemoteDataStorage* pData)
{
	m_mapRemoteDataStorage.insert(std::make_pair(pPed, pData));
}

void CRemoteData::RemoveRemoteDataStorage(PED_TYPE* pPed)
{
	m_mapRemoteDataStorage.erase(pPed);
}

CRemoteDataStorage* CRemoteData::GetRemoteDataStorage(PED_TYPE* pPed)
{
	auto iter = m_mapRemoteDataStorage.find(pPed);
	if (iter != m_mapRemoteDataStorage.end()) return iter->second;
	return nullptr;
}