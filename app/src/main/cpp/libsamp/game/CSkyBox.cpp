#include "../main.h"
#include "game.h"
#include "CSkyBox.h"
#include "..//chatwindow.h"
#include "font.h"
#include "..//gui/gui.h"
#include "..//net/netgame.h"
#include "util/patch.h"

CObject* CSkyBox::m_pSkyObjectDaily = nullptr;
CObject* CSkyBox::m_pSkyObjectNight = nullptr;
CObject* CSkyBox::m_pSkyObjectEvening = nullptr;
CObject* CSkyBox::m_pSkyObjectAfternoon = nullptr;
bool CSkyBox::m_bEnabled = false;
bool CSkyBox::m_bPendingStatus = false;
extern CGame* pGame;
extern CNetGame* pNetGame;

void RwMatrixScale(MATRIX4X4* matrix, VECTOR* scale);
MATRIX4X4* RwMatrixMultiplyByVector(VECTOR* out, MATRIX4X4* a2, VECTOR* in);

CObject* CSkyBox::CreateObjectScaled(int iModel, float fScale)
{
	VECTOR vecRot, vecPos;
	vecRot.X = 0.0f;
	vecRot.Y = 0.0f;
	vecRot.Z = 0.0f;

	vecPos.X = 0.0f;
	vecPos.Y = 0.0f;
	vecPos.Z = 0.0f;

	CObject* retn = new CObject(iModel, 0.0f, 0.0f, -32.0f, vecRot, 5000.0f);

	*(uint32_t*)((uintptr_t)retn->m_pEntity + 28) &= 0xFFFFFFFE;

	*(uint8_t*)((uintptr_t)retn->m_pEntity + 29) |= 1;
	((void (*)(ENTITY_TYPE*))(*(void**)(retn->m_pEntity->vtable + 16)))(retn->m_pEntity); // CPhysical::Remove
	VECTOR vecScale;
	vecScale.X = fScale;
	vecScale.Y = fScale;
	vecScale.Z = fScale;

	MATRIX4X4 objMat;
	retn->GetMatrix(&objMat);


	RwMatrixScale(&objMat, &vecScale);

	retn->SetMatrix(objMat); // copy to CMatrix
	if (retn->m_pEntity->m_pRwObject)
	{
		if (retn->m_pEntity->mat)
		{
			uintptr_t v8 = *(uintptr_t*)(retn->m_pEntity->m_pRwObject + 4) + 16;
			if (v8)
			{
				((int(*)(MATRIX4X4*, uintptr_t))(g_libGTASA + 0x003E862C + 1))(retn->m_pEntity->mat, v8); // CEntity::UpdateRwMatrix
			}
		}
	}

	((int(*)(ENTITY_TYPE*))(g_libGTASA + 0x0039194C + 1))(retn->m_pEntity); // CEntity::UpdateRwFrame
	*(uint8_t*)((uintptr_t)retn->m_pEntity + 29) |= 1;
	((void (*)(ENTITY_TYPE*))(*(void**)(retn->m_pEntity->vtable + 8)))(retn->m_pEntity); // CPhysical::Add
	return nullptr;
}

void CSkyBox::SetEnabled(bool bEnable)
{
	m_bPendingStatus = bEnable;
}

bool fastCheck()
{
	char buff[255];
	snprintf(buff, 255, "%sdata/ctimecy.dat", g_pszStorage);

	FILE* pFile = fopen(buff, "r");

	if (pFile)
	{
		fclose(pFile);
		return true;
	}
	else
	{
		return false;
	}
}

void CSkyBox::Process()
{

	if (m_bEnabled && !m_bPendingStatus)
	{
		CHook::WriteMemory(g_libGTASA + 0x005BAF5C, (uintptr_t)"TIMECYC.DAT", 12);
		((void(*)())(g_libGTASA + 0x0040B0F8 + 1))(); // CTimeCycle::Initialise()
		m_bEnabled = false;
		// process disable
	}
	else if (!m_bEnabled && m_bPendingStatus)
	{
		if (!fastCheck())
		{
			m_bEnabled = false;
			m_bPendingStatus = false;
			return;
		}

		// process enable

		CHook::WriteMemory(g_libGTASA + 0x005BAF5C, (uintptr_t)"CTIMECY.DAT", 12);
		*(float*)(g_libGTASA + 0x8C9AE4) = 2000.0f;
		((void(*)())(g_libGTASA + 0x0040B0F8 + 1))(); // CTimeCycle::Initialise()
		m_bEnabled = true;
	}

	if (!m_bEnabled)
	{
		return;
	}

	pGame->SetWorldWeather(1);

	uintptr_t* dwModelarray = (uintptr_t*)(g_libGTASA + 0x87BF48);
	if (!dwModelarray[17475])
	{
		return;
	}

	if (!m_pSkyObjectDaily)
	{
		m_pSkyObjectDaily = CreateObjectScaled(17475, 0.5f);
		m_pSkyObjectAfternoon = CreateObjectScaled(17476, 0.5f);
		m_pSkyObjectNight = CreateObjectScaled(17477, 0.5f);
		m_pSkyObjectEvening = CreateObjectScaled(17478, 0.5f);
	}

	ScriptCommand(&set_object_visible, m_pSkyObjectDaily->m_dwGTAId, false);
	ScriptCommand(&set_object_visible, m_pSkyObjectAfternoon->m_dwGTAId, false);
	ScriptCommand(&set_object_visible, m_pSkyObjectNight->m_dwGTAId, false);
	ScriptCommand(&set_object_visible, m_pSkyObjectEvening->m_dwGTAId, false);

	int iHours, iMinutes;
	ScriptCommand(&get_current_time, &iHours, &iMinutes);

	if (iHours >= 0 && iHours <= 5)
	{
		ScriptCommand(&set_object_visible, m_pSkyObjectNight->m_dwGTAId, true);
	}
	if (iHours >= 6 && iHours <= 10)
	{
		ScriptCommand(&set_object_visible, m_pSkyObjectAfternoon->m_dwGTAId, true);
	}
	if (iHours >= 11 && iHours <= 18)
	{
		ScriptCommand(&set_object_visible, m_pSkyObjectDaily->m_dwGTAId, true);
	}
	if (iHours >= 19 && iHours <= 24)
	{
		ScriptCommand(&set_object_visible, m_pSkyObjectEvening->m_dwGTAId, true);
	}

	if (m_pSkyObjectDaily)
	{
		MATRIX4X4 matCamera;
		VECTOR vecOut;
		VECTOR vecOffset;

		vecOffset.X = 0.0f;
		vecOffset.Y = 1.0f;
		vecOffset.Z = 0.0f;

		pGame->GetCamera()->GetMatrix(&matCamera);

		RwMatrixMultiplyByVector(&vecOut, &matCamera, &vecOffset);

		PED_TYPE* pPed = GamePool_FindPlayerPed();
		if (pPed)
		{
			m_pSkyObjectDaily->SetPos(vecOut.X, vecOut.Y, vecOut.Z);
			m_pSkyObjectAfternoon->SetPos(pPed->entity.mat->pos.X, pPed->entity.mat->pos.Y, pPed->entity.mat->pos.Z);
			m_pSkyObjectNight->SetPos(pPed->entity.mat->pos.X, pPed->entity.mat->pos.Y, pPed->entity.mat->pos.Z);
			m_pSkyObjectEvening->SetPos(pPed->entity.mat->pos.X, pPed->entity.mat->pos.Y, pPed->entity.mat->pos.Z);
		}
	}
}