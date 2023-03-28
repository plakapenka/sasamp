// -- -- -- -- -- --
// Created by Loony-dev © 2021
// VK: https://vk.com/loonydev
// -- -- -- -

// -- INCLUDE`S
#include "CSkyBox.h"

#include "game/game.h"
#include "game/core/CVector.h"

#include "net/netgame.h"
#include "gui/gui.h"

#include "CSettings.h"
#include "game/Models/ModelInfo.h"

// -- EXTERN`S
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CGUI *pGUI;
extern CSettings *pSettings;

// -- VARIABLE`S
CObject *CSkyBox::m_pSkyObject = nullptr;
RwTexture *pSkyTexture = nullptr;

bool CSkyBox::m_bNeedRender = true;
float CSkyBox::m_fRotSpeed = 0.01f;

uint8_t pChangeTime = 0;

void CSkyBox::Initialise() {
    SetTexture("saA");

   // m_pSkyObject = pGame->NewObject(18500, 0.0, 0.0, 0.0, CVector(0.0f), 300.0);
    m_pSkyObject = CreateObjectScaled(18500, 0.8f);

    if(!pSkyTexture || !m_pSkyObject)
    {
        Log("!!!! ERROR CSkyBox::Initialise !!!!");
        Log("obj = %x, tex = %x", m_pSkyObject, pSkyTexture);
      //  exit(0);
    }
}

void CSkyBox::Process() {
//	if (!pSettings->GetReadOnly().iSkyBox)
//		return;

//    if (!m_pSkyObject) {
//        Initialise();
//        return;
//    }
    if (!m_pSkyObject) return;

    CAMERA_AIM *aim = GameGetInternalAim();
    RwMatrix matrix;

    m_pSkyObject->GetMatrix(&matrix);

    matrix.pos.x = aim->pos1x;
    matrix.pos.y = aim->pos1y;
    matrix.pos.z = aim->pos1z;

    CVector axis = { 0.0f, 0.0f, 1.0f };
    ((int(*)(RwMatrix*, CVector*, float, int))(g_libGTASA + 0x001E38F4 + 1))(&matrix, &axis, m_fRotSpeed, 1);
   // RwMatrixRotate(, , );

   // RwMatrixRotate(&matrix, 2, m_fRotSpeed);

    m_bNeedRender = true;

  //  ReTexture();

    m_pSkyObject->UpdateMatrix(matrix);
    m_pSkyObject->Render();

    m_bNeedRender = false;
}

CObject *CSkyBox::CreateObjectScaled(int iModel, float fScale) {
    if(CModelInfo::ms_modelInfoPtrs[iModel] == nullptr)
    {
        Log("No model %d", iModel);
        return nullptr;
    }
    auto *vecScale = new CVector(fScale);

    auto object = pGame->NewObject(iModel, 1.0f, 1.0f, 1.0f, CVector(0.0f), 0.0f);

    object->SetCollisionChecking(false);
    object->m_pEntity->nEntityFlags.m_bUsesCollision = false;
    *(uint8_t *) ((uintptr_t) object->m_pEntity + 29) |= 1;

    object->RemovePhysical();

    RwMatrix matrix;
    object->GetMatrix(&matrix);

    RwMatrixScale(&matrix, vecScale);

    object->SetMatrix(matrix);
    object->UpdateRwMatrixAndFrame();

    *(uint8_t *) ((uintptr_t) object->m_pEntity + 29) |= 1;
    object->AddPhysical();

    return object;
}

void CSkyBox::ReTexture() {
    int iHours = pNetGame->m_byteWorldTime;

    // -- TODO - Accidentally from several
    if (pChangeTime != iHours) {
        pChangeTime = iHours;

      ///  if (iHours >= 6 && iHours <= 21)
            SetTexture("lkskybox");

//        else if (iHours >= 22 || iHours <= 5)
//            SetTexture("saM");
//
//        else {
//            SetTexture("saM");
//        }
        /*if (iHours >= 0 && iHours <= 5)
            SetTexture("night_sky_1");

        if (iHours >= 6 && iHours <= 10)
            SetTexture("afternoon_sky_1");

        if (iHours >= 11 && iHours <= 18)
            SetTexture("daily_sky_1");

        if (iHours >= 19 && iHours <= 24)
            SetTexture("evening_sky_1");*/
    }
    // ---

    auto pObj = m_pSkyObject->m_pEntity->m_pRwObject;
    if (!pObj)
        return;

    ((void (*)()) (g_libGTASA + 0x005D1F98 + 1))(); // -- DeActivateDirectional
    ((void *(*)()) (g_libGTASA + 0x005D204C + 1))(); // -- SetFullAmbient
    ((void (*)()) (g_libGTASA + 0x005D2068 + 1))(); // -- SetAmbientColours
    ((uintptr_t(*)(void*, uintptr_t, CObject *)) (g_libGTASA + 0x001D8858 + 1))(pObj->parent,
                                                                                      (uintptr_t) RwFrameForAllObjectsCallback,
                                                                                      m_pSkyObject); // RwFrameForAllObjects
}

uintptr_t CSkyBox::RwFrameForAllObjectsCallback(uintptr_t object, CObject *pObject) {
    if (*(uint8_t *) object != 1)
        return object;

    uintptr_t pAtomic = object;

    RpGeometry *pGeom = *(RpGeometry **) (pAtomic + 24);
    if (!pGeom)
        return object;

    int numMats = pGeom->matList.numMaterials;
    if (numMats > 16)
        numMats = 16;

    for (int i = 0; i < numMats; i++) {
        RpMaterial *pMaterial = pGeom->matList.materials[i];
        if (!pMaterial)
            continue;

        if (pSkyTexture)
            pMaterial->texture = pSkyTexture;
    }

    return object;
}

void CSkyBox::SetTexture(const char *texName) {
    if (texName == nullptr)
        return;

    pSkyTexture = nullptr;
    pSkyTexture = CUtil::LoadTextureFromDB("txd", texName);
}

void CSkyBox::SetRotSpeed(float speed) {
    m_fRotSpeed = speed;
}

bool CSkyBox::IsNeedRender() {
    return m_bNeedRender;
}

CObject *CSkyBox::GetSkyObject() {
    return m_pSkyObject;
}

