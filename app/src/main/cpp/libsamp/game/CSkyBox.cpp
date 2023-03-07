#include "../main.h"
#include "game.h"
#include "CSkyBox.h"
#include "..//chatwindow.h"
#include "font.h"
#include "..//gui/gui.h"
#include "..//net/netgame.h"
#include "util/patch.h"
#include "CVector.h"

#define SA_ADDR(addr) (g_libGTASA + (addr))
CObject* CSkyBox::m_pSkyObjectDaily = nullptr;
CObject* CSkyBox::m_pSkyObjectNight = nullptr;
CObject* CSkyBox::m_pSkyObjectEvening = nullptr;
CObject* CSkyBox::m_pSkyObjectAfternoon = nullptr;
CObject* CSkyBox::m_pSkyObject = nullptr;

bool CSkyBox::m_bNeedRender = true;
float CSkyBox::m_fRotSpeed = 0.01f;
bool CSkyBox::m_bEnabled = false;
bool CSkyBox::m_bPendingStatus = false;
extern CGame* pGame;
extern CNetGame* pNetGame;

void RwMatrixScale(RwMatrix* matrix, VECTOR* scale);
RwMatrix* RwMatrixMultiplyByVector(VECTOR* out, RwMatrix* a2, VECTOR* in);

CObject* CSkyBox::CreateObjectScaled(int iModel, float fScale)
{
	//auto *vecRot = new VECTOR;
	auto *vecScale = new CVector(fScale);

	VECTOR vec;
	vec.X = vec.Y = vec.Z = 0;

	if (!pNetGame)
		return nullptr;

	if(!pNetGame->GetObjectPool())
		return nullptr;

	auto *object = pGame->NewObject(iModel, 0.0f, 0.0f, 0.0f, vec, 0.0f);

	*(uint32_t*)((uintptr_t)object->m_pEntity + 28) &= 0xFFFFFFFE;
	*(uint8_t*)((uintptr_t)object->m_pEntity + 29) |= 1;

	object->RemovePhysical();

	RwMatrix matrix;
	object->GetMatrix(&matrix);

	RwMatrixScale(&matrix, reinterpret_cast<VECTOR *>(vecScale));

	object->SetMatrix(matrix);
	object->UpdateRwMatrixAndFrame();

	*(uint8_t*)((uintptr_t)object->m_pEntity + 29) |= 1;
	object->AddPhysical();

	return object;
}

void CSkyBox::SetEnabled(bool bEnable)
{
	m_bPendingStatus = bEnable;
}

void CSkyBox::Initialise()
{
	auto* dwModelArray = (uintptr_t*)(SA_ADDR(0x87BF48));
	if (!dwModelArray[17476])
		return;

	m_pSkyObject = CreateObjectScaled(17980, 18.8f);

//	SetTexture("lkskybox");
}

int delay;
void CSkyBox::Process()
{

	if (!m_pSkyObject)
		Initialise();

	if (m_pSkyObject)
	{
		CAMERA_AIM *aim = GameGetInternalAim();
		RwMatrix matrix;

		m_pSkyObject->GetMatrix(&matrix);

		matrix.pos.X = aim->pos1x;
		matrix.pos.Y = aim->pos1y;
		matrix.pos.Z = aim->pos1z;

		RwMatrixRotate(&matrix, eAxis::Z, 50);

		m_bNeedRender = true;

	//	ReTexture();
		((void(*)())(SA_ADDR(0x559EF8 + 1)))(); // -- DeActivateDirectional
		((void*(*)())(SA_ADDR(0x559FC8 + 1)))(); // -- SetFullAmbient
		((void(*)())(SA_ADDR(0x559FEC + 1)))(); // -- SetAmbientColours

		m_pSkyObject->SetMatrix(matrix);
		m_pSkyObject->UpdateMatrix(matrix);
		m_pSkyObject->Render();

		m_bNeedRender = false;
	}
}