#include "../main.h"
#include "game.h"
#include "CWeaponsOutFit.h"

int GameGetWeaponModelIDFromWeaponID(int iWeaponID);

CWeaponsOutFit::SWeaponOutFitSettings CWeaponsOutFit::m_Settings[MAX_WEAPON_MODELS];
bool CWeaponsOutFit::m_bUsedWeapon[MAX_WEAPON_MODELS];
int CWeaponsOutFit::m_iSlots[MAX_WEAPON_MODELS];
bool CWeaponsOutFit::m_bUsedSlots[10];
bool CWeaponsOutFit::m_bEnabled = false;
bool CWeaponsOutFit::m_bParsed = false;

uint32_t CWeaponsOutFit::m_bLastTickUpdate = 0;
bool CWeaponsOutFit::m_bUpdated = false;

int CWeaponsOutFit::GetWeaponModelIDFromSlot(int iSlot)
{
	for (int i = 0; i < MAX_WEAPON_MODELS; i++)
	{
		if (m_iSlots[i] == iSlot)
		{
			return i;
		}
	}
	return 0;
}

void CWeaponsOutFit::OnWeaponAdded(CPlayerPed* pPed, int iWeaponID)
{
	int iModelID = GameGetWeaponModelIDFromWeaponID(iWeaponID);
	if (iModelID == -1)
	{
		return;
	}
	int iSlot = -1;
	for (int i = 0; i < 10; i++)
	{
		if (!m_bUsedSlots[i])
		{
			iSlot = i;
			m_bUsedSlots[i] = true;
			break;
		}
	}
	if (iSlot == -1)
	{
		return;
	}

	m_iSlots[iWeaponID] = iSlot;

	ATTACHED_OBJECT_INFO info;
	info.dwModelId = (uint32_t)iModelID;
	info.dwBone = (uint32_t)m_Settings[iWeaponID].iBone;

	info.vecOffset.x = m_Settings[iWeaponID].vOffset.x;
	info.vecOffset.y = m_Settings[iWeaponID].vOffset.y;
	info.vecOffset.z = m_Settings[iWeaponID].vOffset.z;
	info.vecRotation.x = m_Settings[iWeaponID].vRotation.x;
	info.vecRotation.y = m_Settings[iWeaponID].vRotation.y;
	info.vecRotation.z = m_Settings[iWeaponID].vRotation.z;

	info.vecScale.x = 1.0f;
	info.vecScale.y = 1.0f;
	info.vecScale.z = 1.0f;

	pPed->AttachObject(&info, iSlot + 10);
	//CChatWindow::AddDebugMessage("ATTACH slot %d %f %f %f %d", iSlot + 10, info.vecOffset.x, info.vecOffset.y, info.vecOffset.z, info.dwBone);
}

void CWeaponsOutFit::OnWeaponRemoved(CPlayerPed* pPed, int iWeaponID)
{
	if (iWeaponID < 0 || iWeaponID >= MAX_WEAPON_MODELS)
	{
		return;
	}
	
	int iSlot = m_iSlots[iWeaponID];
	if (iSlot == -1)
	{
		return;
	}
	m_bUsedSlots[iSlot] = false;
	m_iSlots[iWeaponID] = -1;
	pPed->DeattachObject(iSlot + 10);
	//CChatWindow::AddDebugMessage("DETACH slot %d", iSlot + 10);
}

void CWeaponsOutFit::OnPedDie(CPlayerPed* pPed)
{

}

void CWeaponsOutFit::ParseDatFile()
{

	char buff[255];
	memset(buff, 0, sizeof(buff));

	snprintf(buff, sizeof(buff), "%sSAMP/outfit.dat", g_pszStorage);

	FILE* pFile = fopen(buff, "r");

	if (!pFile)
	{
		Log("Cannot parse DAT file, no weapons outfit");
		m_bParsed = false;
		return;
	}

	memset(buff, 0, sizeof(buff));

	while (fgets(buff, sizeof(buff), pFile)) 
	{
		if (buff[0] == ';')
		{
			memset(buff, 0, sizeof(buff));
			continue;
		}

		int iWeaponId;
		int iSlot;
		int iBone;
		float fPosX, fPosY, fPosZ;
		float fRotX, fRotY, fRotZ;

		int iSecondaryBone;
		float fSecondaryPosX, fSecondaryPosY, fSecondaryPosZ;
		float fSecondaryRotX, fSecondaryRotY, fSecondaryRotZ;
		sscanf(buff, "%d %d %d %f %f %f %f %f %f %d %f %f %f %f %f %f", 
			&iWeaponId, &iSlot, &iBone, 
			&fPosX, &fPosY, &fPosZ, &fRotX, &fRotY, &fRotZ,
			&iSecondaryBone, 
			&fSecondaryPosX, &fSecondaryPosY, &fSecondaryPosZ, 
			&fSecondaryRotX, &fSecondaryRotY, &fSecondaryRotZ);

		if (iWeaponId < 0 || iWeaponId >= MAX_WEAPON_MODELS)
		{
			memset(buff, 0, sizeof(buff));
			continue;
		}

		m_Settings[iWeaponId].iSlot = iSlot;
		m_Settings[iWeaponId].iBone = iBone;
		m_Settings[iWeaponId].vOffset.x = fPosX;
		m_Settings[iWeaponId].vOffset.y = fPosY;
		m_Settings[iWeaponId].vOffset.z = fPosZ;

		m_Settings[iWeaponId].vRotation.x = fRotX;
		m_Settings[iWeaponId].vRotation.y = fRotY;
		m_Settings[iWeaponId].vRotation.z = fRotZ;

		if (iSlot != -1)
		{
			m_Settings[iWeaponId].bUseSecondary = true;
		}

		m_Settings[iWeaponId].iBoneSecondary = iBone;
		m_Settings[iWeaponId].vOffsetSecondary.x = fSecondaryPosX;
		m_Settings[iWeaponId].vOffsetSecondary.y = fSecondaryPosY;
		m_Settings[iWeaponId].vOffsetSecondary.z = fSecondaryPosZ;

		m_Settings[iWeaponId].vRotationSecondary.x = fSecondaryRotX;
		m_Settings[iWeaponId].vRotationSecondary.y = fSecondaryRotY;
		m_Settings[iWeaponId].vRotationSecondary.z = fSecondaryRotZ;
		

		memset(buff, 0, sizeof(buff));
	}

	for (int i = 0; i < MAX_WEAPON_MODELS; i++)
	{
		m_bUsedWeapon[i] = false;
		m_iSlots[i] = -1;
	}

	for (int i = 0; i < 10; i++)
	{
		m_bUsedSlots[i] = false;
	}
	m_bParsed = true;
	fclose(pFile);
}

void CWeaponsOutFit::ProcessLocalPlayer(CPlayerPed* pPed)
{
	if (!m_bEnabled || !m_bParsed)
	{
		for (int i = 0; i < MAX_WEAPON_MODELS; i++)
		{
			// delete this
			m_bUsedWeapon[i] = false;
			OnWeaponRemoved(pPed, i);
		}
		return;
	}

	if (m_bUpdated && GetTickCount() - m_bLastTickUpdate >= 1)
	{
		for (int i = 0; i < 10; i++)
		{
			int iWeaponID = GetWeaponModelIDFromSlot(i);
			CVector vecPos, vecRot;
			if (i >= MAX_ATTACHED_OBJECTS || iWeaponID == 0)
			{
				continue;
			}

			vecPos = m_Settings[iWeaponID].vOffset;
			vecRot = m_Settings[iWeaponID].vRotation;

			pPed->SetAttachOffset(i + 10, vecPos, vecRot);
		}

		m_bUpdated = false;
		m_bLastTickUpdate = 0;
		return;
	}

	bool m_bCurrentWeapons[MAX_WEAPON_MODELS];
	for (int i = 0; i < MAX_WEAPON_MODELS; i++)
	{
		m_bCurrentWeapons[i] = false;
	}

	for (int i = 0; i < 13; i++)
	{
		int iWeapon = pPed->m_pPed->WeaponSlots[i].dwType;
		if (pPed->m_pPed->WeaponSlots[i].dwAmmo == 0)
		{
			iWeapon = 0;
		}
		int iCurrentWeapon = (int)pPed->GetCurrentWeapon();
		if (iWeapon && iWeapon != iCurrentWeapon)
		{
			m_bCurrentWeapons[iWeapon] = true;
		}
	}

	for (int i = 0; i < MAX_WEAPON_MODELS; i++)
	{
		if (m_bCurrentWeapons[i] && !m_bUsedWeapon[i])
		{
			m_bUsedWeapon[i] = true;
			OnWeaponAdded(pPed, i);
			// appeared!
		}
		else if(!m_bCurrentWeapons[i] && m_bUsedWeapon[i])
		{
			// delete this
			m_bUsedWeapon[i] = false;
			OnWeaponRemoved(pPed, i);
		}
	}
}

void CWeaponsOutFit::SetEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
}

void CWeaponsOutFit::OnUpdateOffsets()
{
	m_bUpdated = true;
	m_bLastTickUpdate = GetTickCount();
}
