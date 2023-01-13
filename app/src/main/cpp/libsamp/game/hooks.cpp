#include "../main.h"
#include "RW/RenderWare.h"
#include <sstream>
#include "game.h"

#include "../net/netgame.h"
#include "../gui/gui.h"
#include "../util/CJavaWrapper.h"
#include "../java_systems/CHUD.h"
#include "..///..//santrope-tea-gtasa/encryption/CTinyEncrypt.h"
#include "..///..//santrope-tea-gtasa/encryption/encrypt.h"
extern "C"
{
#include "..//santrope-tea-gtasa/encryption/aes.h"
}

#include "..///..//santrope-tea-gtasa/CGameResourcesDecryptor.cpp"
#include "..///..//santrope-tea-gtasa/CGameResourcesDecryptor.h"
#include "chatwindow.h"
#include "../util/patch.h"

extern CGame* pGame;
#include "..//CSettings.h"
extern CSettings* pSettings;
extern CHUD *pHud;
extern CPlayerPed *g_pCurrentFiredPed;
extern BULLET_DATA *g_pCurrentBulletData;

static uint32_t dwRLEDecompressSourceSize = 0;

char(*CStreaming__ConvertBufferToObject)(int, int, int);
int __attribute__((noinline)) g_unobfuscate(int a)
{
	return UNOBFUSCATE_DATA(a);
}
#include "..//str_obfuscator_no_template.hpp"
#define MAX_ENCRYPTED_TXD 3
const cryptor::string_encryptor encrArch[MAX_ENCRYPTED_TXD] = {
        cryptor::create("texdb/txd/txd.txt", 19),
        cryptor::create("texdb/gta3/gta3.txt", 21),
        cryptor::create("texdb/gta_int/gta_int.txt", 27),
};

extern bool isTestMode;
bool isEncrypted(const char *szArch)
{
    if(isTestMode)return false;
	//return false;
    for (int i = 0; i < MAX_ENCRYPTED_TXD; i++)
    {
        if (!strcmp(encrArch[i].decrypt(), szArch))
            return true;
    }
    return false;
}
void InitCTX(AES_ctx& ctx, const uint8_t* pKey)
{
    uint8_t key[16];
    memcpy(&key[0], &pKey[0], 16);
    for (int i = 0; i < 16; i++)
    {
        key[i] = XOR_UNOBFUSCATE(key[i]);
    }
    uint8_t iv[16];
    memcpy(&iv[0], &g_iIV, 16);
    for (int i = 0; i < 16; i++)
    {
        iv[i] = XOR_UNOBFUSCATE(iv[i]);
    }
    AES_init_ctx_iv(&ctx, &key[0], &iv[0]);
}

static int lastOpenedFile = 0;

extern CNetGame *pNetGame;
extern CGUI *pGUI;
// Neiae/SAMP
bool g_bPlaySAMP = false;

void InitInMenu();
void MainLoop();
void HookCPad();

/* ================ ie?oee aey ani. anoaaie =================== */

extern "C" uintptr_t get_lib() 
{
 	return g_libGTASA;
}

/* ====================================================== */

struct stFile
{
	int isFileExist;
	FILE *f;
};

stFile* (*NvFOpen)(const char*, const char*, int, int);
stFile* NvFOpen_hook(const char* r0, const char* r1, int r2, int r3)
{
	char path[0xFF] = { 0 };
	// ----------------------------
	if(!strncmp(r1+12, "mainV1.scm", 10))
	{
		sprintf(path, "%sSAMP/main.scm", g_pszStorage);
		Log("Loading mainV1.scm..");
		goto open;
	}
	// ----------------------------
	if(!strncmp(r1+12, "SCRIPTV1.IMG", 12))
	{
		sprintf(path, "%sSAMP/script.img", g_pszStorage);
		Log("Loading script.img..");
		goto open;
	}
	// ----------------------------
	if(!strncmp(r1, "DATA/PEDS.IDE", 13))
	{
		sprintf(path, "%s/SAMP/peds.ide", g_pszStorage);
		Log("Loading peds.ide..");
		goto open;
	}
	// ----------------------------
	if(!strncmp(r1, "DATA/VEHICLES.IDE", 17))
	{
		sprintf(path, "%s/SAMP/vehicles.ide", g_pszStorage);
		Log("Loading vehicles.ide..");
		goto open;
	}

	if (!strncmp(r1, "DATA/GTA.DAT", 12))
	{
		sprintf(path, "%s/SAMP/gta.dat", g_pszStorage);
		Log("Loading gta.dat..");
		goto open;
	}

	/*if (!strncmp(r1, "DATA/HANDLING.CFG", 17))
	{
		sprintf(path, "%s/SAMP/handling.cfg", g_pszStorage);
		Log("Loading handling.cfg..");
		goto open;
	}

	if (!strncmp(r1, "DATA/WEAPON.DAT", 15))
	{
		sprintf(path, "%s/SAMP/weapon.dat", g_pszStorage);
		Log("Loading weapon.dat..");
		goto open;
	}*/

orig:
	return NvFOpen(r0, r1, r2, r3);

open:
	stFile *st = (stFile*)malloc(8);
	st->isFileExist = false;

	FILE *f  = fopen(path, "rb");
	if(f)
	{
		st->isFileExist = true;
		st->f = f;
		return st;
	}
	else
	{
		Log("NVFOpen hook | Error: file not found (%s)", path);
		free(st);
		st = nullptr;
		return nullptr;
	}
}

/* ====================================================== */
bool bGameStarted = false;
uint32_t bProcessedCleanStreamPool = 0;
void RenderBackgroundHud();

void (*Render2dStuff)();
void Render2dStuff_hook()
{
	if (pHud->isHudToggle) pHud->UpdateHudInfo();

	bGameStarted = true;
	MAKE_PROFILE(test, test_time);
	MainLoop();
	RenderBackgroundHud();
	LOG_PROFILE(test, test_time);
	Render2dStuff();

	if (!bProcessedCleanStreamPool)
		bProcessedCleanStreamPool = GetTickCount();

	uint32_t result = GetTickCount() - bProcessedCleanStreamPool;
	if (result >= 5000)
	{
		CStreaming::RemoveAllUnusedModels();
	}

}

void __fillArray()
{
}

/* ====================================================== */

void (*Render2dStuffAfterFade)();
void Render2dStuffAfterFade_hook()
{
	Render2dStuffAfterFade();
	if (pGUI && bGameStarted) pGUI->Render();
	return;
}

/* ====================================================== */

uint16_t gxt_string[0x7F];
uint16_t* (*CText_Get)(uintptr_t thiz, const char* text);
uint16_t* CText_Get_hook(uintptr_t thiz, const char* text)
{
	if(text[0] == 'S' && text[1] == 'A' && text[2] == 'M' && text[3] == 'P')
	{
		const char* code = &text[5];
		if(!strcmp(code, "MP")) CFont::AsciiToGxtChar("MultiPlayer", gxt_string);

    	return gxt_string;
	}

	return CText_Get(thiz, text);
}

/* ====================================================== */

void MainMenu_OnStartSAMP()
{

	if(g_bPlaySAMP) return;

	InitInMenu();

	// StartGameScreen::OnNewGameCheck()
	(( void (*)())(g_libGTASA+0x261C8C+1))();

	//*(uint32_t*)(g_libGTASA + 0x009E75B8) = 8;

	g_bPlaySAMP = true;
	return;
}

// OsArray<FlowScreen::MenuItem>::Add
void (*MenuItem_add)(int r0, uintptr_t r1);
void MenuItem_add_hook(int r0, uintptr_t r1)
{
	static bool bMenuInited = false;
	char* name = *(char**)(r1+4);

	// убирает кнопки "Start Game" и "Stats" в меню
	if(g_bPlaySAMP && (
		!strcmp(name, "FEP_STG") ||
		!strcmp(name, "FEH_STA") ||
		!strcmp(name, "FEH_BRI") ))
		return;

	return MenuItem_add(r0, r1);
}

/* ====================================================== */

// CGame::InitialiseRenderWare
void (*InitialiseRenderWare)();
void InitialiseRenderWare_hook()
{
	Log("InitialiseRenderWare ..");

	CPatch::NOP(g_libGTASA + 0x40C57E, 2); // cutscene
	CPatch::NOP(g_libGTASA + 0x40C592, 2); // player

	InitialiseRenderWare();

	TextureDatabaseRuntime::Load("samp", 0, TextureDatabaseRuntime::TextureDatabaseFormat::DF_Default);

}

/* ====================================================== */
void (*CLoadingScreen_DisplayPCScreen)();
void CLoadingScreen_DisplayPCScreen_hook()
{
//	RwCamera *camera = *(RwCamera **)(g_libGTASA + 0x95B064);
//
//	if (RwCameraBeginUpdate(camera))
//	{
//		DefinedState2d();
//		((void (*)())(g_libGTASA + 0x5519C8 + 1))(); // CSprite2d::InitPerFrame()
//		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void *)rwTEXTUREADDRESSCLAMP);
//		((void (*)(bool))(g_libGTASA + 0x198010 + 1))(false); // emu_GammaSet()

		const float percent = *(float*)(g_libGTASA + 0x8F08C0)*2;
		if (percent <= 0.0f) return;

		g_pJavaWrapper->UpdateSplash((int)percent);

//		RwCameraEndUpdate(camera);
//		RwCameraShowRaster(camera, 0, 0);
//	}

	return;
}

int bBlockCWidgetRegionLookUpdate = 0;

/* ====================================================== */
#include "..//keyboard.h"
extern CKeyBoard* pKeyBoard;
void (*TouchEvent)(int, int, int posX, int posY);
void TouchEvent_hook(int type, int num, int posX, int posY)
{
	//Log("TOUCH EVENT HOOK");

	if (*(uint8_t*)(g_libGTASA + 0x008C9BA3) == 1)
	{
		return TouchEvent(type, num, posX, posY);
	}

	if (g_pWidgetManager)
	{
		g_pWidgetManager->OnTouchEventSingle(WIDGET_CHATHISTORY_UP, type, num, posX, posY);
		g_pWidgetManager->OnTouchEventSingle(WIDGET_CHATHISTORY_DOWN, type, num, posX, posY);
	}
	bool bRet = pGUI->OnTouchEvent(type, num, posX, posY);
	if (!bRet)
	{
		return;
	}

	ImGuiIO& io = ImGui::GetIO();

	if (pKeyBoard && pKeyBoard->IsOpen())
	{
		if (posX >= io.DisplaySize.x - ImGui::GetFontSize() * 4 && posY >= io.DisplaySize.y / 2 - (ImGui::GetFontSize() * 2.5) * 3 && posY <= io.DisplaySize.y / 2) // keys
		{
			return;
		}
	}

	/*
	if (g_pWidgetManager)
	{
		g_pWidgetManager->OnTouchEvent(type, num, posX, posY);
		if (g_pWidgetManager->GetWidget(WIDGET_MICROPHONE))
		{
			if (g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->GetState() == 1)
			{
				bBlockCWidgetRegionLookUpdate = 1;
			}
			if (g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->GetState() == 2)
			{
				bBlockCWidgetRegionLookUpdate = 0;
			}
		}
	}
	*/
	if(bRet) 
		return TouchEvent(type, num, posX, posY);
}

/* ====================================================== */

unsigned int (*TextureDatabaseRuntime__Load)(uintptr_t *thiz, const char *a2, const char *a3, bool a4);
unsigned int TextureDatabaseRuntime__Load_hook(uintptr_t *thiz, const char *a2, const char *a3, bool a4)
{
	Log("TextureDatabaseRuntime__Load = %s, %s, %s, %d", (char*)thiz, a2, a3, a4);
	return TextureDatabaseRuntime__Load(thiz, a2, a3, a4);
}
void (*CStreaming_InitImageList)();
void CStreaming_InitImageList_hook()
{
	char* ms_files = (char*)(g_libGTASA+0x6702FC);
	ms_files[0] = 0;
	*(uint32_t*)&ms_files[44] = 0;
	ms_files[48] = 0;
	*(uint32_t*)&ms_files[92] = 0;
	ms_files[96] = 0;
	*(uint32_t*)&ms_files[140] = 0;
	ms_files[144] = 0;
	*(uint32_t*)&ms_files[188] = 0;
	ms_files[192] = 0;
	*(uint32_t*)&ms_files[236] = 0;
	ms_files[240] = 0;
	*(uint32_t*)&ms_files[284] = 0;
	ms_files[288] = 0;
	*(uint32_t*)&ms_files[332] = 0;
	ms_files[336] = 0;
	*(uint32_t*)&ms_files[380] = 0;

	CStreaming::AddImageToList("TEXDB\\GTA3.IMG", true);
	CStreaming::AddImageToList("TEXDB\\SKINS.IMG", true);
    CStreaming::AddImageToList("TEXDB\\CARS.IMG", true);
	CStreaming::AddImageToList("TEXDB\\SAMP.IMG", true);
	CStreaming::AddImageToList("TEXDB\\SAMPCOL.IMG", true);
}

/* ====================================================== */
typedef struct _PED_MODEL
{
	uintptr_t 	vtable;
	uint8_t		data[88];
} PED_MODEL; // SIZE = 92

PED_MODEL PedsModels[370];
int PedsModelsCount = 0;

PED_MODEL* (*CModelInfo_AddPedModel)(int id);
PED_MODEL* CModelInfo_AddPedModel_hook(int id)
{
	PED_MODEL* model = &PedsModels[PedsModelsCount];
	memset(model, 0, sizeof(PED_MODEL));								// initialize by zero

	((void(*)(void* thiz))(g_libGTASA + 0x0033559C + 1))((void*)model); // CBaseModelInfo::CBaseModelInfo();

    model->vtable = (uintptr_t)(g_libGTASA+0x5C6E90);					// assign CPedModelInfo vmt

    (( uintptr_t (*)(PED_MODEL*))(*(void**)(model->vtable+0x1C)))(model);  // CClumpModelInfo::Init()

    *(PED_MODEL**)(g_libGTASA+0x87BF48+(id*4)) = model; // CModelInfo::ms_modelInfoPtrs

	PedsModelsCount++;
	return model;
}
/* ====================================================== */

uint32_t (*CRadar__GetRadarTraceColor)(uint32_t color, uint8_t bright, uint8_t friendly);
uint32_t CRadar__GetRadarTraceColor_hook(uint32_t color, uint8_t bright, uint8_t friendly)
{
	return TranslateColorCodeToRGBA(color);
}

int (*CRadar__SetCoordBlip)(int r0, float X, float Y, float Z, int r4, int r5, char *name);
int CRadar__SetCoordBlip_hook(int r0, float X, float Y, float Z, int r4, int r5, char *name)
{
	if (pNetGame && !strncmp(name, "CODEWAY", 7))
	{
		float findZ = ((float (*)(float, float))(g_libGTASA + 0x3C3DD8 + 1))(X, Y);
		findZ += 1.5f;

		Log("OnPlayerClickMap: %f, %f, %f", X, Y, Z);
		RakNet::BitStream bsSend;
		bsSend.Write(X);
		bsSend.Write(Y);
		bsSend.Write(findZ);
		pNetGame->GetRakClient()->RPC(&RPC_MapMarker, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	}

	return CRadar__SetCoordBlip(r0, X, Y, Z, r4, r5, name);
}

uint8_t bGZ = 0;
void (*CRadar__DrawRadarGangOverlay)(uint8_t v1);
void CRadar__DrawRadarGangOverlay_hook(uint8_t v1)
{
	bGZ = v1;
	if (pNetGame && pNetGame->GetGangZonePool()) 
		pNetGame->GetGangZonePool()->Draw();
}

uint32_t dwParam1, dwParam2;
extern "C" void pickup_ololo()
{
	if(pNetGame && pNetGame->GetPickupPool())
	{
		CPickupPool *pPickups = pNetGame->GetPickupPool();
		pPickups->PickedUp( ((dwParam1-(g_libGTASA+0x70E264))/0x20) );
	}
}

__attribute__((naked)) void PickupPickUp_hook()
{
	//LOGI("PickupPickUp_hook");

	// calculate and save ret address
	__asm__ volatile("push {lr}\n\t"
					"push {r0}\n\t"
					"blx get_lib\n\t"
					"add r0, #0x2D0000\n\t"
					"add r0, #0x009A00\n\t"
					"add r0, #1\n\t"
					"mov r1, r0\n\t"
					"pop {r0}\n\t"
					"pop {lr}\n\t"
					"push {r1}\n\t");
	
	// 
	__asm__ volatile("push {r0-r11, lr}\n\t"
					"mov %0, r4" : "=r" (dwParam1));

	__asm__ volatile("blx pickup_ololo\n\t");


	__asm__ volatile("pop {r0-r11, lr}\n\t");

	// restore
	__asm__ volatile("ldrb r1, [r4, #0x1C]\n\t"
					"sub.w r2, r1, #0xD\n\t"
					"sub.w r2, r1, #8\n\t"
					"cmp r1, #6\n\t"
					"pop {pc}\n\t");
}
//
//extern "C" bool NotifyEnterVehicle(VEHICLE_TYPE *_pVehicle)
//{
//    Log("NotifyEnterVehicle");
//
//    if(!pNetGame) return false;
//
//    CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
//    CVehicle *pVehicle;
//    VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(_pVehicle);
//
//    if(VehicleID == INVALID_VEHICLE_ID) return false;
//    if(!pVehiclePool->GetSlotState(VehicleID)) return false;
//    pVehicle = pVehiclePool->GetAt(VehicleID);
//    if(pVehicle->m_bDoorsLocked) return false;
//    if(pVehicle->m_pVehicle->entity.nModelIndex == TRAIN_PASSENGER) return false;
//
////    if(pVehicle->m_pVehicle->pDriver &&
////        pVehicle->m_pVehicle->pDriver->dwPedType != 0)
////        return false;
//
//    CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
//
//    //if(pLocalPlayer->GetPlayerPed() && pLocalPlayer->GetPlayerPed()->GetCurrentWeapon() == WEAPON_PARACHUTE)
//    //  pLocalPlayer->GetPlayerPed()->SetArmedWeapon(0);
//
//    pLocalPlayer->SendEnterVehicleNotification(VehicleID, false);
//
//    return true;
//}


uint32_t (*CWeapon__FireInstantHit)(WEAPON_SLOT_TYPE *_this, PED_TYPE *pFiringEntity, VECTOR *vecOrigin, VECTOR *muzzlePosn, ENTITY_TYPE *targetEntity,
									VECTOR *target, VECTOR *originForDriveBy, int arg6, int muzzle);
uint32_t CWeapon__FireInstantHit_hook(WEAPON_SLOT_TYPE *_this, PED_TYPE *pFiringEntity, VECTOR *vecOrigin, VECTOR *muzzlePosn, ENTITY_TYPE *targetEntity,
									  VECTOR *target, VECTOR *originForDriveBy, int arg6, int muzzle)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile("mov %0, lr"
	: "=r"(dwRetAddr));

	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x569A84 + 1 ||
		dwRetAddr == 0x569616 + 1 ||
		dwRetAddr == 0x56978A + 1 ||
		dwRetAddr == 0x569C06 + 1)
	{
		// if (GamePool_FindPlayerPed() != pFiringEntity) return 0;

		g_pCurrentFiredPed = pGame->FindPlayerPed();
	}

	return CWeapon__FireInstantHit(_this, pFiringEntity, vecOrigin, muzzlePosn, targetEntity, target, originForDriveBy, arg6, muzzle);
}

void ProcessPedDamage(PED_TYPE* pIssuer, PED_TYPE* pDamaged)
{
	if (!pNetGame) return;

	//PED_TYPE* pPedPlayer = GamePool_FindPlayerPed();
	if (pDamaged && (pGame->FindPlayerPed()->m_pPed == pIssuer))
	{
		if (pNetGame->GetPlayerPool()->FindRemotePlayerIDFromGtaPtr((PED_TYPE*)pDamaged) != INVALID_PLAYER_ID)
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			CAMERA_AIM* caAim = pPlayerPool->GetLocalPlayer()->GetPlayerPed()->GetCurrentAim();

			VECTOR aim;
			aim.X = caAim->f1x;
			aim.Y = caAim->f1y;
			aim.Z = caAim->f1z;

			pPlayerPool->GetLocalPlayer()->SendBulletSyncData(pPlayerPool->FindRemotePlayerIDFromGtaPtr((PED_TYPE*)pDamaged), 1, aim);
		}
	}
}

unsigned int (*MainMenuScreen__Update)(uintptr_t thiz, float a2);
unsigned int MainMenuScreen__Update_hook(uintptr_t thiz, float a2)
{
	unsigned int ret = MainMenuScreen__Update(thiz, a2);
	MainMenu_OnStartSAMP();
	return ret;
}

extern char(*CStreaming__ConvertBufferToObject)(int, int, int);
char CStreaming__ConvertBufferToObject_hook(int a1, int a2, int a3);

void RedirectCall(uintptr_t addr, uintptr_t func);

static char szLastBufferedName[40];
int (*cHandlingDataMgr__FindExactWord)(uintptr_t thiz, char* line, char* nameTable, int entrySize, int entryCount);
int cHandlingDataMgr__FindExactWord_hook(uintptr_t thiz, char* line, char* nameTable, int entrySize, int entryCount)
{
	strncpy(&szLastBufferedName[0], line, entrySize);
	return cHandlingDataMgr__FindExactWord(thiz, line, nameTable, entrySize, entryCount);
}

void (*cHandlingDataMgr__ConvertDataToGameUnits)(uintptr_t thiz, tHandlingData* handling);
void cHandlingDataMgr__ConvertDataToGameUnits_hook(uintptr_t thiz, tHandlingData* handling)
{
	int iHandling = ((int(*)(uintptr_t, char*))(g_libGTASA + 0x004FBC4C + 1))(thiz, &szLastBufferedName[0]);

	CHandlingDefault::FillDefaultHandling((uint16_t)iHandling, handling);

	return cHandlingDataMgr__ConvertDataToGameUnits(thiz, handling);
}


#include "..//nv_event.h"
int32_t(*NVEventGetNextEvent_hooked)(NVEvent* ev, int waitMSecs);
int32_t NVEventGetNextEvent_hook(NVEvent* ev, int waitMSecs)
{
	int32_t ret = NVEventGetNextEvent_hooked(ev, waitMSecs);

	if (ret)
	{
		if (ev->m_type == NV_EVENT_MULTITOUCH)
		{
			// process manually
			ev->m_type = (NVEventType)228;
		}

	}

	NVEvent event;
	NVEventGetNextEvent(&event);

	if (event.m_type == NV_EVENT_MULTITOUCH)
	{
		int type = event.m_data.m_multi.m_action & NV_MULTITOUCH_ACTION_MASK;
		int num = (event.m_data.m_multi.m_action & NV_MULTITOUCH_POINTER_MASK) >> NV_MULTITOUCH_POINTER_SHIFT;

		int x1 = event.m_data.m_multi.m_x1;
		int y1 = event.m_data.m_multi.m_y1;

		int x2 = event.m_data.m_multi.m_x2;
		int y2 = event.m_data.m_multi.m_y2;

		int x3 = event.m_data.m_multi.m_x3;
		int y3 = event.m_data.m_multi.m_y3;

		if (type == NV_MULTITOUCH_CANCEL)
		{
			type = NV_MULTITOUCH_UP;
		}

		if ((x1 || y1) || num == 0)
		{
			if (num == 0 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(type, 0, x1, y1); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(NV_MULTITOUCH_MOVE, 0, x1, y1); // AND_TouchEvent
			}
		}

		if ((x2 || y2) || num == 1)
		{
			if (num == 1 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(type, 1, x2, y2); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(NV_MULTITOUCH_MOVE, 1, x2, y2); // AND_TouchEvent
			}
		}
		if ((x3 || y3) || num == 2)
		{
			if (num == 2 && type != NV_MULTITOUCH_MOVE)
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(type, 2, x3, y3); // AND_TouchEvent
			}
			else
			{
				((void(*)(int, int, int posX, int posY))(g_libGTASA + 0x00239D5C + 1))(NV_MULTITOUCH_MOVE, 2, x3, y3); // AND_TouchEvent
			}
		}
	}

	return ret;
}

void(*CStreaming__Init2)();
void CStreaming__Init2_hook()
{
	CStreaming__Init2();
	CPatch::UnFuck(g_libGTASA + 0x5DE734);
	*(uint32_t*)(g_libGTASA + 0x5DE734) = 0x20000000;
}

void (*NvUtilInit)(void);
void NvUtilInit_hook(void)
{	
	NvUtilInit();
	CPatch::UnFuck(g_libGTASA + 0x5D1608);
	*(char**)(g_libGTASA + 0x5D1608) = (char*)g_pszStorage;
}

signed int (*OS_FileOpen)(unsigned int a1, int *a2, const char *a3, int a4);
signed int OS_FileOpen_hook(unsigned int a1, int *a2, const char *a3, int a4)
{
	//if (strstr(a3, ".unc") ) return 0;

  //  Log("%s", a3);
    uintptr_t calledFrom = 0;
    __asm__ volatile("mov %0, lr"
    : "=r"(calledFrom));
    calledFrom -= g_libGTASA;
    signed int retn = OS_FileOpen(a1, a2, a3, a4);

    //if (calledFrom == 0x1BCE9A + 1)
    //{
        if (isEncrypted(a3))
        {
            lastOpenedFile = *a2;
        }
  //  }
    return retn;
}

size_t (*OS_FileRead)(FILE *a1, void *a2, size_t a3);
size_t OS_FileRead_hook(FILE *a1, void *a2, size_t a3)
{
    uintptr_t calledFrom = 0;
    __asm__ volatile("mov %0, lr"
    : "=r"(calledFrom));
    calledFrom -= g_libGTASA;

    if (!a3)
    {
        return 0;
    }

    if (calledFrom == 0x1BCEE0 + 1 && a1 == (void *)lastOpenedFile)
    {
        lastOpenedFile = 0;

        AES_ctx ctx;
		InitCTX(ctx, &g_iEncryptionKeyTXD[0]);

        size_t retv = OS_FileRead(a1, a2, a3);
        int fileSize = a3;
        int iters = fileSize / PART_SIZE_TXD;
        uintptr_t pointer = (uintptr_t)a2;
        for (int i = 0; i < iters; i++)
        {
            AES_CBC_decrypt_buffer(&ctx, (uint8_t *)pointer, PART_SIZE_TXD);
            pointer += PART_SIZE_TXD;
        }
        return retv;
    }
    if (calledFrom == 0x001BDD34 + 1)
    {
        int retn = OS_FileRead(a1, a2, a3);

        dwRLEDecompressSourceSize = *(uint32_t*)a2;

        return retn;
    }
    else
    {
        int retn = OS_FileRead(a1, a2, a3);

        return retn;
    }
}

void (*CTimer__StartUserPause)();
void CTimer__StartUserPause_hook()
{
	Log("CTimer__StartUserPause_hook");
	// process pause event
	if (g_pJavaWrapper)
	{
		if (pKeyBoard)
		{
			if (pKeyBoard->IsNewKeyboard())
			{
				pKeyBoard->Close();
			}
		}
		g_pJavaWrapper->SetPauseState(true);
	}

	*(uint8_t*)(g_libGTASA + 0x008C9BA3) = 1;
}

void (*CTimer__EndUserPause)();
void CTimer__EndUserPause_hook()
{
	// process resume event
	if (g_pJavaWrapper)
	{
		g_pJavaWrapper->SetPauseState(false);
	}

	*(uint8_t*)(g_libGTASA + 0x008C9BA3) = 0;
}

void (*TextureDatabase__LoadThumbs)(int a1, int a2);
void TextureDatabase__LoadThumbs_hook(int a1, int a2)
{// only dxt.tmb
//	char v5[256]; // [sp+Ch] [bp+4h] BYREF
//
//	snprintf(v5, 0x100u, "texdb/%s/%s.dxt.tmb", *(const char **)(a1 + 4), *(const char **)(a1 + 4));
//
//	return ((void(*)(int, int, uint*))(g_libGTASA + 0x001BD128 + 1))(a1, (int)v5, (unsigned int *)(a1 + 0xC * a2 + 0x20));
	return TextureDatabase__LoadThumbs(a1, a2);
}

void (*TextureDatabase__LoadDataOffsets)(int a1, int a2, unsigned int *a3, void **a4, char a5);
void TextureDatabase__LoadDataOffsets_hook(int a1, int a2, unsigned int *a3, void **a4, char a5)
{
	return TextureDatabase__LoadDataOffsets(a1, 1, a3, a4, a5); // only dxt.toc
	//return;
}

extern char g_iLastBlock[512];
int *(*LoadFullTexture)(uintptr_t *thiz, unsigned int a2);
int *LoadFullTexture_hook(uintptr_t *thiz, unsigned int a2)
{
	strcpy(g_iLastBlock, *(const char **)(*((DWORD *)thiz + 7) + 0x17 * a2));
    //*((DWORD *)thiz + 0x1A) = 0;
    //Log("LoadFullTexture_hook %s", *(const char **)(*((DWORD *)thiz + 7) + 0x17 * a2));
//	Log("%d", *((DWORD *)thiz + 0x1A));
//	Log("%d", *((DWORD *)thiz + 0x1B));
//    if(strcmp(*(const char **)(*((DWORD *)thiz + 7) + 0x17 * a2), "hud_bg") == 0){
//
//    }
    return LoadFullTexture(thiz, a2);
}
//
uint8_t* (*RLEDecompress)(uint8_t* pDest, size_t uiDestSize, uint8_t const* pSrc, size_t uiSegSize, uint32_t uiEscape);
uint8_t* RLEDecompress_hook(uint8_t* pDest, size_t uiDestSize, uint8_t const* pSrc, size_t uiSegSize, uint32_t uiEscape) {
    if (!pDest)
    {
        return 0;
    }

    if (!pSrc)
    {
        return pDest;
    }

    uint8_t* pTempDest = pDest;
    const uint8_t* pTempSrc = pSrc;
    uint8_t* pEndOfDest = &pDest[uiDestSize];

    uint8_t* pEndOfSrc = (uint8_t*)&pSrc[dwRLEDecompressSourceSize];

    if (pDest < pEndOfDest)
    {
        do
        {
            if (*pTempSrc == uiEscape)
            {
                uint8_t ucCurSeg = pTempSrc[1];
                if (ucCurSeg)
                {
                    uint8_t* ucCurDest = pTempDest;
                    uint8_t ucCount = 0;
                    do
                    {
                        ++ucCount;
                        //Log("WRITE111 TO 0x%x FROM 0x%x SIZE %d", ucCurDest, pTempSrc + 2, uiSegSize);
                        pDest = (uint8_t*)memcpy(ucCurDest, pTempSrc + 2, uiSegSize);

                        ucCurDest += uiSegSize;
                    } while (ucCurSeg != ucCount);


                    pTempDest += uiSegSize * ucCurSeg;
                }
                pTempSrc += 2 + uiSegSize;
            }

            else
            {
                if (pTempSrc + uiSegSize >= pEndOfSrc)
                {
                    //Log("AVOID CRASH TO 0x%x FROM 0x%x SIZE %d END OF SRC 0x%x", pTempDest, pTempSrc, pEndOfSrc - pTempSrc - 1, pEndOfSrc);
                    return pDest;
                }
                else
                {
                    //Log("WRITE222 TO 0x%x FROM 0x%x SIZE %d END OF SRC 0x%x", pTempDest, pTempSrc, uiSegSize, pEndOfSrc);
                    pDest = (uint8_t*)memcpy(pTempDest, pTempSrc, uiSegSize);
                    pTempDest += uiSegSize;
                    pTempSrc += uiSegSize;
                }
            }
        } while (pEndOfDest > pTempDest);
    }

    dwRLEDecompressSourceSize = 0;

    return pDest;
}

//#include <GLES2/gl2.h>
//void rqVertexBufferSelect_HOOK(unsigned int **result)
//{
//	unsigned int buffer = *(*result)++;
//
//	if (!buffer)
//		return glBindBuffer(0x8892, 0);
//
//	glBindBuffer(0x8892, buffer + 8);
//	*(uint32_t *)(g_libGTASA + 0x617234) = 0;
//}

void (*CPools_Initialise)(void);
void CPools_Initialise_hook(void)
{
	struct PoolAllocator {

		struct Pool {
			void* objects;
			uint8_t* flags;
			uint32_t count;
			uint32_t top;
			uint32_t bInitialized;
		};
		static_assert(sizeof(Pool) == 0x14);

		static Pool* Allocate(size_t count, size_t size) {

			Pool* p = new Pool;

			p->objects = new char[size * count];
			p->flags = new uint8_t[count];
			p->count = count;
			p->top = 0xFFFFFFFF;
			p->bInitialized = 1;

			for (size_t i = 0; i < count; i++) {
				p->flags[i] |= 0x80;
				p->flags[i] &= 0x80;
			}

			return p;
		}
	};

	// 600000 / 75000 = 8
	static auto ms_pPtrNodeSingleLinkPool = PoolAllocator::Allocate(100000, 8);		// 75000
	// 72000 / 6000 = 12
	static auto ms_pPtrNodeDoubleLinkPool = PoolAllocator::Allocate(60000, 12);	// 6000
	// 10000 / 500 = 20
	static auto ms_pEntryInfoNodePool = PoolAllocator::Allocate(20000, 20);	// 500
	// 279440 / 140 = 1996
	static auto ms_pPedPool = PoolAllocator::Allocate(240, 1996);	// 140
	// 286440 / 110 = 2604
	static auto ms_pVehiclePool = PoolAllocator::Allocate(2000, 2604);	// 110
	// 840000 / 14000 = 60
	static auto ms_pBuildingPool = PoolAllocator::Allocate(20000, 60);	// 14000
	// 147000 / 350 = 420
	static auto ms_pObjectPool = PoolAllocator::Allocate(3000, 420);	// 350
	// 210000 / 3500 = 60
	static auto ms_pDummyPool = PoolAllocator::Allocate(40000, 60);	// 3500
	// 487200 / 10150 = 48
	static auto ms_pColModelPool = PoolAllocator::Allocate(50000, 48);	// 10150
	// 64000 / 500 = 128
	static auto ms_pTaskPool = PoolAllocator::Allocate(5000, 128);	// 500
	// 13600 / 200 = 68
	static auto ms_pEventPool = PoolAllocator::Allocate(1000, 68);	// 200
	// 6400 / 64 = 100
	static auto ms_pPointRoutePool = PoolAllocator::Allocate(200, 100);	// 64
	// 13440 / 32 = 420
	static auto ms_pPatrolRoutePool = PoolAllocator::Allocate(200, 420);	// 32
	// 2304 / 64 = 36
	static auto ms_pNodeRoutePool = PoolAllocator::Allocate(200, 36);	// 64
	// 512 / 16 = 32
	static auto ms_pTaskAllocatorPool = PoolAllocator::Allocate(3000, 32);	// 16
	// 92960 / 140 = 664
	static auto ms_pPedIntelligencePool = PoolAllocator::Allocate(240, 664);	// 140
	// 15104 / 64 = 236
	static auto ms_pPedAttractorPool = PoolAllocator::Allocate(200, 236);	// 64

	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93E0) = ms_pPtrNodeSingleLinkPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93DC) = ms_pPtrNodeDoubleLinkPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93D8) = ms_pEntryInfoNodePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93D4) = ms_pPedPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93D0) = ms_pVehiclePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93CC) = ms_pBuildingPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93C8) = ms_pObjectPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93C4) = ms_pDummyPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93C0) = ms_pColModelPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93BC) = ms_pTaskPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93B8) = ms_pEventPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93B4) = ms_pPointRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93B0) = ms_pPatrolRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93AC) = ms_pNodeRoutePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93A8) = ms_pTaskAllocatorPool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93A4) = ms_pPedIntelligencePool;
	*(PoolAllocator::Pool**)(g_libGTASA + 0x8B93A0) = ms_pPedAttractorPool;
}

void (*CPlaceable_InitMatrixArray)();
void CPlaceable_InitMatrixArray_hook()
{
	CPatch::CallFunction<void>(g_libGTASA+0x3AB2D8+1, g_libGTASA+0x8B90A8, 10000);
}


int (*CustomPipeRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int CustomPipeRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
	if (!resEntry)
		return 0;
	uint16_t size = *(uint16_t *)(resEntry + 26);
	if (size)
	{
		RES_ENTRY_OBJ *arr = (RES_ENTRY_OBJ *)(resEntry + 28);
		if (!arr)
			return 0;
		uint32_t validFlag = flags & 0x84;
		for (int i = 0; i < size; i++)
		{
			if (!arr[i].validate)
				break;
			if (validFlag)
			{
				uintptr_t *v4 = *(uintptr_t **)(arr[i].validate);
				if (!v4)
					;
				else
				{
					if ((uintptr_t)v4 > (uintptr_t)0xFFFFFF00)
						return 0;
					else
					{
						if (!*(uintptr_t **)v4)
							return 0;
					}
				}
			}
		}
	}

	return CustomPipeRenderCB(resEntry, object, type, flags);
}

int (*rxOpenGLDefaultAllInOneRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int rxOpenGLDefaultAllInOneRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
	if (!resEntry)
		return 0;
	uint16_t size = *(uint16_t *)(resEntry + 26);
	if (size)
	{
		RES_ENTRY_OBJ *arr = (RES_ENTRY_OBJ *)(resEntry + 28);
		if (!arr)
			return 0;
		uint32_t validFlag = flags & 0x84;
		for (int i = 0; i < size; i++)
		{
			if (!arr[i].validate)
				break;
			if (validFlag)
			{
				uintptr_t *v4 = *(uintptr_t **)(arr[i].validate);
				if (!v4)
					;
				else
				{
					if ((uintptr_t)v4 > (uintptr_t)0xFFFFFF00)
						return 0;
					else
					{
						if (!*(uintptr_t **)v4)
							return 0;
					}
				}
			}
		}
	}
	return rxOpenGLDefaultAllInOneRenderCB(resEntry, object, type, flags);
}

uintptr_t (*GetMeshPriority)(uintptr_t);
uintptr_t GetMeshPriority_hook(uintptr_t rpMesh)
{
	if (rpMesh)
	{
		RpMaterial *rpMeshMat = *(RpMaterial **)(rpMesh + 8);
		if (rpMeshMat)
		{
			if (rpMeshMat->texture)
			{
				if (!rpMeshMat->texture->raster)
					return 0;
			}
		}
	}
	return GetMeshPriority(rpMesh);
}

int (*CTextureDatabaseRuntime__GetEntry)(unsigned int a1, const char *a2, bool *a3, int a4);
int CTextureDatabaseRuntime__GetEntry_hook(unsigned int a1, const char *a2, bool *a3, int a4)
{
	//Log("GetEntry = %s", (char*)a1);
	int result; // r0

	if (a1)
		result = CTextureDatabaseRuntime__GetEntry(a1, a2, a3, a4);
	else
		result = -1;
	return result;
}

int (*CGame__Init2)(uintptr_t *thiz, const char *a2);
int CGame__Init2_hook(uintptr_t *thiz, const char *a2)
{

}


void readVehiclesAudioSettings();
void (*CVehicleModelInfo__SetupCommonData)();
void CVehicleModelInfo__SetupCommonData_hook()
{
	CVehicleModelInfo__SetupCommonData();
	readVehiclesAudioSettings();
}

extern VehicleAudioPropertiesStruct VehicleAudioProperties[20000];
static uintptr_t addr_veh_audio = (uintptr_t)& VehicleAudioProperties[0];

void (*CAEVehicleAudioEntity__GetVehicleAudioSettings)(uintptr_t thiz, int16_t a2, int a3);
void CAEVehicleAudioEntity__GetVehicleAudioSettings_hook(uintptr_t dest, int16_t a2, int ID)
{
	memcpy((void*)dest, &VehicleAudioProperties[(ID - 400)], sizeof(VehicleAudioPropertiesStruct));
}

uintptr_t* (*CCustomRoadsignMgr_RenderRoadsignAtomic)(uintptr_t*, VECTOR const&);
uintptr_t* CCustomRoadsignMgr_RenderRoadsignAtomic_hook(uintptr_t* atomic, VECTOR const& a2)
{
	return atomic;
}

void InstallSpecialHooks()
{
	Log("InstallSpecialHooks");

	// texture Краш если с текстурами какая-то хуйня
	CPatch::InlineHook(g_libGTASA, 0x1bdc3c, &CTextureDatabaseRuntime__GetEntry_hook, &CTextureDatabaseRuntime__GetEntry);

	//CPatch::InlineHook(g_libGTASA, 0x0055B968, &TextureDatabaseRuntime__Load_hook, (uintptr_t*)&TextureDatabaseRuntime__Load);
	//IMG
	CPatch::InlineHook(g_libGTASA, 0x28E83C, &CStreaming_InitImageList_hook, (uintptr_t*)&CStreaming_InitImageList);

	//shadow
	CPatch::RET(g_libGTASA + 0x541AC4);

	CPatch::InlineHook(g_libGTASA, 0x001E4AE4, &GetMeshPriority_hook, &GetMeshPriority);
	//new fix
	CPatch::InlineHook(g_libGTASA, 0x1EEC90, &rxOpenGLDefaultAllInOneRenderCB_hook, &rxOpenGLDefaultAllInOneRenderCB);
	CPatch::InlineHook(g_libGTASA, 0x28AAAC, &CustomPipeRenderCB_hook, &CustomPipeRenderCB);

	//
	// CPlaceable::InitMatrixArray
	CPatch::WriteMemory(g_libGTASA + 0x3ABB0A, "\x4F\xF4\x7A\x61", 4); // MOV.W R1, #4000
	CPatch::InlineHook(g_libGTASA, 0x3ABB08, &CPlaceable_InitMatrixArray_hook, &CPlaceable_InitMatrixArray);

	// RLEDecompress fix
	CPatch::InlineHook(g_libGTASA, 0x001BC314, &RLEDecompress_hook, &RLEDecompress);

	//
	CPatch::InlineHook(g_libGTASA, 0x001BDCC4, &LoadFullTexture_hook, &LoadFullTexture);

	CPatch::NOP(g_libGTASA + 0x00185DE8, 2);//_rwOpenGLNativeTextureReadXBOXPvS_
	//
	CPatch::NOP(g_libGTASA + 0x002F9E5C, 10); 	//LoadCutsceneData
	CPatch::NOP(g_libGTASA + 0x0040E536, 2);		//CCutsceneMgr::Initialise

	CPatch::InlineHook(g_libGTASA, 0x003BF784, &CTimer__StartUserPause_hook, &CTimer__StartUserPause);
	CPatch::InlineHook(g_libGTASA, 0x003BF7A0, &CTimer__EndUserPause_hook, &CTimer__EndUserPause);

	CPatch::NOP(g_libGTASA + 0x00398972, 2); // get out fucking roadblocks

	//crash
	if ( !*(uintptr_t *)(g_libGTASA + 0x61B298) && !((int (*)(const char *))(g_libGTASA + 0x179A20))("glAlphaFuncQCOM") )
	{
		CPatch::NOP(g_libGTASA + 0x1A6164, 4);
		CPatch::WriteMemory(g_libGTASA + 0x1A6164, "\x70\x47", 2);
	}

	//pvr
	CPatch::UnFuck(g_libGTASA + 0x00573670);
	*(char*)(g_libGTASA + 0x00573670 + 12) = 'd';
	*(char*)(g_libGTASA + 0x00573670 + 13) = 'x';
	*(char*)(g_libGTASA + 0x00573670 + 14) = 't';

	CPatch::UnFuck(g_libGTASA + 0x005736CC);
	*(char*)(g_libGTASA + 0x005736CC + 12) = 'd';
	*(char*)(g_libGTASA + 0x005736CC + 13) = 'x';
	*(char*)(g_libGTASA + 0x005736CC + 14) = 't';

	//etc
	CPatch::UnFuck(g_libGTASA + 0x00573684);
	*(char*)(g_libGTASA + 0x00573684 + 12) = 'd';
	*(char*)(g_libGTASA + 0x00573684 + 13) = 'x';
	*(char*)(g_libGTASA + 0x00573684 + 14) = 't';

	CPatch::UnFuck(g_libGTASA + 0x005736DC);
	*(char*)(g_libGTASA + 0x005736DC + 12) = 'd';
	*(char*)(g_libGTASA + 0x005736DC + 13) = 'x';
	*(char*)(g_libGTASA + 0x005736DC + 14) = 't';

	//CPatch::InlineHook(g_libGTASA, 0x0052B810, &VolumetricCloudsInit_hook, &VolumetricCloudsInit);

	CPatch::WriteMemory(g_libGTASA + 0x0023BEDC, "\xF8\xB5", 2);
	CPatch::WriteMemory(g_libGTASA + 0x0023BEDE, "\x00\x46\x00\x46", 4);

	CPatch::InlineHook(g_libGTASA, 0x0023BEDC, &OS_FileRead_hook, &OS_FileRead);
	CPatch::InlineHook(g_libGTASA, 0x23B3DC, &NvFOpen_hook, & NvFOpen);
	CPatch::InlineHook(g_libGTASA, 0x241D94, &NvUtilInit_hook, &NvUtilInit);

	CPatch::InlineHook(g_libGTASA, 0x269974, &MenuItem_add_hook, &MenuItem_add);
	CPatch::InlineHook(g_libGTASA, 0x40C530, &InitialiseRenderWare_hook, &InitialiseRenderWare);
	CPatch::InlineHook(g_libGTASA, 0x0025E660, &MainMenuScreen__Update_hook, &MainMenuScreen__Update);
	CPatch::InlineHook(g_libGTASA, 0x0023BB84, &OS_FileOpen_hook, &OS_FileOpen);

	CPatch::InlineHook(g_libGTASA, 0x004FBBB0, &cHandlingDataMgr__FindExactWord_hook, &cHandlingDataMgr__FindExactWord);
	CPatch::InlineHook(g_libGTASA, 0x004FBCF4, cHandlingDataMgr__ConvertDataToGameUnits_hook, &cHandlingDataMgr__ConvertDataToGameUnits);
	CPatch::InlineHook(g_libGTASA, 0x0023ACC4, NVEventGetNextEvent_hook, &NVEventGetNextEvent_hooked);
	CPatch::InlineHook(g_libGTASA, 0x004042A8, CStreaming__Init2_hook, &CStreaming__Init2);	// increase stream memory value
	CPatch::InlineHook(g_libGTASA, 0x3AF1A0, &CPools_Initialise_hook, &CPools_Initialise);
	CPatch::InlineHook(g_libGTASA, 0x531118, &CCustomRoadsignMgr_RenderRoadsignAtomic_hook, &CCustomRoadsignMgr_RenderRoadsignAtomic);

//	JMPCode(g_libGTASA + 0x1A1ED8, (uintptr_t)rqVertexBufferSelect_HOOK);
}

void ProcessPedDamage(PED_TYPE* pIssuer, PED_TYPE* pPlayer);

/* =========================================== Ped damage handler =========================================== */
enum ePedPieceTypes
{
	PED_PIECE_UNKNOWN = 0,

	PED_PIECE_TORSO = 3,
	PED_PIECE_ASS,
	PED_PIECE_LEFT_ARM,
	PED_PIECE_RIGHT_ARM,
	PED_PIECE_LEFT_LEG,
	PED_PIECE_RIGHT_LEG,
	PED_PIECE_HEAD
};

struct CPedDamageResponseInterface
{
	float fDamageHealth;
	float fDamageArmor;
	bool bUnk;
	bool bForceDeath;
	bool bDamageCalculated;
	bool bUnk3;
};

struct CPedDamageResponseCalculatorInterface
{
	ENTITY_TYPE *pEntity;
	float fDamage;
	ePedPieceTypes bodyPart;
	unsigned int weaponType;
	bool bSpeak; // refers to a CPed::Say call (the dying scream?)
};

uintptr_t (*ComputeDamageResponse)(CPedDamageResponseCalculatorInterface *pPedDamageResponseCalculator, PED_TYPE *pDamagedPed, CPedDamageResponseInterface *pPedDamageResponse, bool bSpeak);
uintptr_t ComputeDamageResponse_hooked(CPedDamageResponseCalculatorInterface *pPedDamageResponseCalculator, PED_TYPE *pDamagedPed, CPedDamageResponseInterface *pPedDamageResponse, bool bSpeak)
{
	// Make sure that everything is not null
	if (!pNetGame || !pPedDamageResponseCalculator || !pDamagedPed || !pPedDamageResponse || !pPedDamageResponseCalculator->pEntity)
		return ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if (!pPlayerPool || !pVehiclePool)
		return ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);

	CLocalPlayer *pLocalPlayer = pPlayerPool->GetLocalPlayer();
	if (!pLocalPlayer)
		return ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);

	PED_TYPE *pInflictor = (PED_TYPE *)pPedDamageResponseCalculator->pEntity;
	PED_TYPE *pLocalPed = GamePool_FindPlayerPed();
	if (!pLocalPed)
		return ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);

	// Damage reason (or weapon)
	uint32_t damageReason = 54; // 54 in case if we couldn't get the reason

	// Convert damage to Multiplayer values (approximately)
	float fDamage = (pPedDamageResponseCalculator->fDamage / 3.0303030303);

	// To get ID in the future.
	CRemotePlayer *pRemotePlayer = nullptr;

	// Change status during the damage processing...
	bool bGiveOrTake = false;

	// Did we damage ourselves?
	if ((pInflictor == pLocalPed) && (pInflictor == pDamagedPed))
	{
		// Check if the inflictor is a vehicle
		// Actually, there are more cases, but I'm too lazy to detect them.
		VEHICLEID vehicleId = pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE *)pPedDamageResponseCalculator->pEntity);
		if (vehicleId != INVALID_VEHICLE_ID)
		{
			// Set the reason that we got damaged by a vehicle
			damageReason = 49;
		}

		// Send that we took damage.
		pLocalPlayer->GiveTakeDamage(true, INVALID_PLAYER_ID, fDamage, damageReason, pPedDamageResponseCalculator->bodyPart);
		return ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);
	}
	else if (pInflictor == pLocalPed)
	{
		// Did we damage someone?
		// Let's try to get this person
		PLAYERID remotePlayerId = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pDamagedPed);
		if (remotePlayerId != INVALID_PLAYER_ID)
		{
			pRemotePlayer = pPlayerPool->GetAt(remotePlayerId);
		}

		bGiveOrTake = false;
	}
	else if (pDamagedPed == pLocalPed)
	{
		// Or did we get damaged by someone?
		// Then let's try to get inflictor
		PLAYERID remotePlayerId = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pInflictor);
		if (remotePlayerId != INVALID_PLAYER_ID)
		{
			pRemotePlayer = pPlayerPool->GetAt(remotePlayerId);
		}

		bGiveOrTake = true;
	}
	else
	{
		// Oh, seems like not our case, sorry. Keep processing...
		return ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);
	}

	if (!pRemotePlayer)
		return ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);

	// Send damage response now
	pLocalPlayer->GiveTakeDamage(bGiveOrTake, pRemotePlayer->GetID(), fDamage, pPedDamageResponseCalculator->weaponType, pPedDamageResponseCalculator->bodyPart);

	return ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);
}

int(*RwFrameAddChild)(int, int);
int RwFrameAddChild_hook(int a1, int a2)
{
	if (a2 && a1)
	{
		return RwFrameAddChild(a1, a2);
	}
	else return 0;
}

void (*CAnimManager__UncompressAnimation)(int, int);
void CAnimManager__UncompressAnimation_hook(int a1, int a2)
{
	if (a1)
	{
		CAnimManager__UncompressAnimation(a1, a2);
	}
}

int(*CUpsideDownCarCheck__IsCarUpsideDown)(int, int);
int CUpsideDownCarCheck__IsCarUpsideDown_hook(int a1, int a2)
{
	if (*(uintptr_t*)(a2 + 20))
	{
		return CUpsideDownCarCheck__IsCarUpsideDown(a1, a2);
	}
	return 0;
}

int(*CAnimBlendNode__FindKeyFrame)(int, float, int, int);
int CAnimBlendNode__FindKeyFrame_hook(int a1, float a2, int a3, int a4)
{
	if (*(uintptr_t*)(a1 + 16))
	{
		return CAnimBlendNode__FindKeyFrame(a1, a2, a3, a4);
	}
	else return 0;
}


/* ====================================================== */
struct _ATOMIC_MODEL
{
	uintptr_t func_tbl;
	char data[56];
};
extern struct _ATOMIC_MODEL* ATOMIC_MODELS;

uintptr_t(*CModelInfo_AddAtomicModel)(int iModel);
uintptr_t CModelInfo_AddAtomicModel_hook(int iModel)
{
	uint32_t iCount = *(uint32_t*)(g_libGTASA + 0x7802C4);
	_ATOMIC_MODEL* model = &ATOMIC_MODELS[iCount];
	*(uint32_t*)(g_libGTASA + 0x7802C4) = iCount + 1;

	((void(*)(_ATOMIC_MODEL*))(*(uintptr_t*)(model->func_tbl + 0x1C)))(model);
	_ATOMIC_MODEL** ms_modelInfoPtrs = (_ATOMIC_MODEL**)(g_libGTASA + 0x87BF48);
	ms_modelInfoPtrs[iModel] = model;
	return (uintptr_t)model;
}

/* ====================================================== */
typedef struct _VEHICLE_MODEL
{
	uintptr_t 	vtable;
	uint8_t		data[932];
} VEHICLE_MODEL; // SIZE = 936

VEHICLE_MODEL VehicleModels[370];
int VehicleModelsCount = 0;

VEHICLE_MODEL* (*CModelInfo_AddVehicleModel)(int id);
VEHICLE_MODEL* CModelInfo_AddVehicleModel_hook(int id)
{
	VEHICLE_MODEL* model = &VehicleModels[VehicleModelsCount];
	memset(model, 0, sizeof(VEHICLE_MODEL));

	((void(*)(void* thiz))(g_libGTASA + 0x00337AA0 + 1))((void*)model); // CVehicleModelInfo::CVehicleModelInfo();

	model->vtable = (uintptr_t)(g_libGTASA + 0x005C6EE0);			// assign CVehicleModelInfo vmt

	((uintptr_t(*)(VEHICLE_MODEL*))(*(void**)(model->vtable + 0x1C)))(model); // CVehicleModelInfo::Init()

	*(VEHICLE_MODEL * *)(g_libGTASA + 0x87BF48 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

	VehicleModelsCount++;
	return model;
}

#include "..//keyboard.h"
extern CKeyBoard* pKeyBoard;

uint64_t(*CWorld_ProcessPedsAfterPreRender)();
uint64_t CWorld_ProcessPedsAfterPreRender_hook()
{
	uint64_t res = CWorld_ProcessPedsAfterPreRender();
	if (pNetGame && pNetGame->GetPlayerPool())
	{
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			CPlayerPed* pPed = nullptr;
			if (pNetGame->GetPlayerPool()->GetLocalPlayerID() == i)
			{
				pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
			}
			else
			{
				if (pNetGame->GetPlayerPool()->GetSlotState(i))
				{
					pPed = pNetGame->GetPlayerPool()->GetAt(i)->GetPlayerPed();
				}
			}
			if (!pPed) continue;
			else
			{
				pPed->ProcessAttach();
			}
		}
	}

	return res;
}

int RemoveModelIDs[MAX_REMOVE_MODELS];
VECTOR RemovePos[MAX_REMOVE_MODELS];
float RemoveRad[MAX_REMOVE_MODELS];
int iTotalRemovedObjects = 0;


#include "util.h"

int(*CFileLoader__LoadObjectInstance)(uintptr_t, uintptr_t);
int CFileLoader__LoadObjectInstance_hook(uintptr_t thiz, uintptr_t name)
{
	for (int i = 0; i < iTotalRemovedObjects; i++)
	{
		if (RemoveModelIDs[i] == *(uint32_t*)(thiz + 28))
		{
			VECTOR pos;
			pos.X = *(float*)(thiz);
			pos.Y = *(float*)(thiz + 4);
			pos.Z = *(float*)(thiz + 8);
			if (GetDistanceBetween3DPoints(&pos, &RemovePos[i]) <= RemoveRad[i])
			{
				*(int*)(thiz + 28) = 19300;
			}
		}
	}
	return CFileLoader__LoadObjectInstance(thiz, name);
}

#include <list>

std::list<std::pair<unsigned int*, unsigned int>> resetEntries;

static uint32_t Color32Reverse(uint32_t x)
{
	return
		((x & 0xFF000000) >> 24) |
		((x & 0x00FF0000) >> 8) |
		((x & 0x0000FF00) << 8) |
		((x & 0x000000FF) << 24);
}

static RwRGBA DWORD2RGBAinternal(uint32_t dwColor)
{
	RwRGBA tmp;

	tmp.blue = dwColor & 0xFF; dwColor >>= 8;
	tmp.green = dwColor & 0xFF; dwColor >>= 8;
	tmp.red = dwColor & 0xFF; dwColor >>= 8;
	tmp.alpha = dwColor & 0xFF; /* dwColor >>= 8; */

	return tmp;
}

uintptr_t RwFrameForAllObjectsCALLBACK1(uintptr_t object, CObject* pObject)
{
	if (*(uint8_t*)object != 1)
	{
		return object;
	}
	uintptr_t pAtomic = object;
	RpGeometry* pGeom = *(RpGeometry * *)(pAtomic + 24);
	if (!pGeom)
	{
		return object;
	}

	int numMats = pGeom->matList.numMaterials;
	if (numMats > 16)
	{
		numMats = 16;
	}
	for (int i = 0; i < numMats; i++)
	{
		RpMaterial* pMat = pGeom->matList.materials[i];
		if (!pMat)
		{
			continue;
		}
		if (pObject->m_pMaterials[i].m_bCreated)
		{
			RpMaterial* pMat = pGeom->matList.materials[i];
			if (!pMat)
			{
				continue;
			}
			if (pObject->m_pMaterials[i].pTex)
			{
				resetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&pMat->texture), *reinterpret_cast<unsigned int*>(&pMat->texture)));
				pMat->texture = pObject->m_pMaterials[i].pTex;
			}
			if (pObject->m_pMaterials[i].dwColor)
			{
				//((int(*)())(g_libGTASA + 0x00559FC8 + 1))();
				resetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&pGeom->flags), *reinterpret_cast<unsigned int*>(&pGeom->flags)));
				pGeom->flags |= 0x00000040;
				pGeom->flags &= 0xFFFFFFF7;
				RwRGBA r = DWORD2RGBAinternal(pObject->m_pMaterials[i].dwColor);

				resetEntries.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&pMat->color), *reinterpret_cast<unsigned int*>(&pMat->color)));
				pMat->color = r;
				pMat->surfaceProps.ambient = 1.0f;
				pMat->surfaceProps.specular = 0.0f;
				pMat->surfaceProps.diffuse = 1.0f;
			}
		}
	}
	return object;
}

int g_iLastRenderedObject;
void(*CObject__Render)(ENTITY_TYPE*);
void CObject__Render_hook(ENTITY_TYPE* thiz)
{
	// 0051ABB0 + 1
	// 004353FE + 1
	// 004352C4 + 1

	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x0051ABB0 + 1 || dwRetAddr == 0x004353FE + 1 || dwRetAddr == 0x004352C4 + 1)
	{
		return CObject__Render(thiz);
	}

	uintptr_t pAtomic = thiz->m_RwObject;
	if (!pAtomic)
	{
		return CObject__Render(thiz); // CObject::Render(CObject*)
	}
	if (!*(uintptr_t*)(pAtomic + 4))
	{
		return CObject__Render(thiz); // CObject::Render(CObject*)
	}

	if (pNetGame)
	{
		CObjectPool* pObjectPool = pNetGame->GetObjectPool();
		if (pObjectPool)
		{
			CObject* pObject = pObjectPool->GetObjectFromGtaPtr(thiz);
			if (pObject)
			{
				if (pObject->m_pEntity)
				{
					g_iLastRenderedObject = pObject->m_pEntity->nModelIndex;
				}
				
				((void(*)())(g_libGTASA + 0x00559EF8 + 1))(); // DeActivateDirectional
				if (pObject->m_bMaterials)
				{
					//thiz->m_bLightObject = 0;
					((uintptr_t(*)(uintptr_t, uintptr_t, uintptr_t))(g_libGTASA + 0x001AEE2C + 1))(*(uintptr_t*)(pAtomic + 4), (uintptr_t)RwFrameForAllObjectsCALLBACK1, (uintptr_t)pObject); // RwFrameForAllObjects
					//((void(*)(ENTITY_TYPE*, bool))(g_libGTASA + 0x003B1F1C + 1))(thiz, 1); // CObject::RemoveLighting(CObject*, bool)
					//((void(*)())(g_libGTASA + 0x00559EF8 + 1))(); // DeActivateDirectional

				}
			}
			
		}
	}

	CObject__Render(thiz); // CObject::Render(CObject*)
	for (auto& p : resetEntries)
		* p.first = p.second;
	resetEntries.clear();
}
#pragma optimize( "", off )

char CStreaming__ConvertBufferToObject_hook(int a1, int a2, int a3)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	

	uint32_t tickStart = GetTickCount();
	CGameResourcesDecryptor::CStreaming__ConvertBufferToObject_hook((char*)a1, a2, a3);
	if (a2 >= 15000 && a2 <= 15100)
	{
		//CChatWindow::AddDebugMessage("loading time %d", GetTickCount() - tickStart);
	}
	char a12 = CStreaming__ConvertBufferToObject(a1, a2, a3);
	return a12;
}

// CGameIntegrity
// CodeObfuscation

#pragma optimize( "", on )


uintptr_t(*GetTexture_orig)(const char*);
uintptr_t GetTexture_hook(const char* a1)
{
	//001BE990
	uintptr_t tex = ((uintptr_t(*)(const char*))(g_libGTASA + 0x001BE990 + 1))(a1);
	if (!tex)
	{
		Log("Texture %s was not found", a1);
		return 0;
	}
	else
	{
		Log("Texture %s", a1);
		++* (uintptr_t*)(tex + 84);
		return tex;
	}
}

int(*CPlayerInfo__Process)(uintptr_t *thiz, uintptr_t *a2);
int CPlayerInfo__Process_hook(uintptr_t *thiz, uintptr_t *a2)
{
	float *v23; // r3
	v23 = *(float **)thiz;
	if ( !*((BYTE *)thiz + 0xD5)  || (*((BYTE *)v23 + 0x481) & 1) == 0){
		return 0;
	}
	return CPlayerInfo__Process(thiz, a2);
}
typedef uintptr_t RpClump;
RwFrame* CClumpModelInfo_GetFrameFromId_Post(RwFrame* pFrameResult, RpClump* pClump, int id)
{
	if (pFrameResult)
		return pFrameResult;

	uintptr_t calledFrom = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (calledFrom));
	calledFrom -= g_libGTASA;

	if (calledFrom == 0x00515708                // CVehicle::SetWindowOpenFlag
		|| calledFrom == 0x00515730             // CVehicle::ClearWindowOpenFlag
		|| calledFrom == 0x00338698             // CVehicleModelInfo::GetOriginalCompPosition
		|| calledFrom == 0x00338B2C)            // CVehicleModelInfo::CreateInstance
		return NULL;

	for (uint i = 2; i < 40; i++)
	{
		RwFrame* pNewFrameResult = NULL;
		uint     uiNewId = id + (i / 2) * ((i & 1) ? -1 : 1);

		pNewFrameResult = ((RwFrame * (*)(RpClump * pClump, int id))(g_libGTASA + 0x00335CC0 + 1))(pClump, i);

		if (pNewFrameResult)
		{
			return pNewFrameResult;
		}
	}

	return NULL;
}
RwFrame* (*CClumpModelInfo_GetFrameFromId)(RpClump*, int);
RwFrame* CClumpModelInfo_GetFrameFromId_hook(RpClump* a1, int a2)
{
	return CClumpModelInfo_GetFrameFromId_Post(CClumpModelInfo_GetFrameFromId(a1, a2), a1, a2);
}

int (*CEventHandler__HandleEvents)(uintptr_t *thiz);
int CEventHandler__HandleEvents_hook(uintptr_t *thiz)
{
	if(*((DWORD *)thiz + 1)){
		return CEventHandler__HandleEvents(thiz);
	}
	return 0;
}

void (*CWidgetRegionLook__Update)(uintptr_t thiz);
void CWidgetRegionLook__Update_hook(uintptr_t thiz)
{
	if (bBlockCWidgetRegionLookUpdate)
	{
		return;
	}
	else
	{
		return CWidgetRegionLook__Update(thiz);
	}
}

void (*ProcessCommands300To399)(uintptr_t* thiz, int a2);
void ProcessCommands300To399_hook(uintptr_t* thiz, int a2)
{
	//Log("pedHandle = %d, local = %d", pedHandle, pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->m_dwGTAId);
	if(!a2)return;

	return ProcessCommands300To399(thiz, a2);
}

void (*ProcessCommands1400To1499)(uintptr_t* thiz, int a2);
void ProcessCommands1400To1499_hook(uintptr_t* thiz, int a2)
{
	//Log("pedHandle = %d, local = %d", pedHandle, pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->m_dwGTAId);
	if(!a2)return;

	return ProcessCommands1400To1499(thiz, a2);
}

void (*GivePedScriptedTask)(uintptr_t* thiz, int pedHandle, uintptr_t* a3, int commandID);
void GivePedScriptedTask_hook(uintptr_t* thiz, int pedHandle, uintptr_t* a3, int commandID)
{
	//Log("pedHandle = %d, local = %d", pedHandle, pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->m_dwGTAId);
	if(!pedHandle || !commandID || !a3)return;

	if ( pedHandle == 0xFFFFFFFF ){
		return GivePedScriptedTask(thiz, pedHandle, a3, commandID);
	}
	//if(!(pedHandle>>8))return;
	uintptr_t tmp = *(uintptr_t*)(g_libGTASA + 0x008B93D4);

	if(  !(*(const PED_TYPE**) tmp + 0x4 ) ) return; // CPools::ms_pPedPool

	return GivePedScriptedTask(thiz, pedHandle, a3, commandID);
}

int (*CObject__ProcessGarageDoorBehaviour)(uintptr_t, int);
int CObject__ProcessGarageDoorBehaviour_hook(uintptr_t thiz, int a2)
{
	if (thiz)
		if (!*(uintptr_t *)(thiz + 372))
			return 0;
	return CObject__ProcessGarageDoorBehaviour(thiz, a2);
}

int (*CTaskSimpleCarOpenDoorFromOutside__ComputeAnimID)(uintptr_t *thiz, int *a2, int *a3, int a4);
int CTaskSimpleCarOpenDoorFromOutside__ComputeAnimID_hook(uintptr_t *thiz, int *a2, int *a3, int a4)
{
	if( !(*((DWORD *)thiz + 4)) )return 0;

	return CTaskSimpleCarOpenDoorFromOutside__ComputeAnimID(thiz, a2, a3, a4);
}

int (*RpMaterialDestroy)(int a1, int a2, int a3, int a4);
int RpMaterialDestroy_hook(int a1, int a2, int a3, int a4)
{
	if(!a1 || !a2 || !a3 || !a4)return 1;

	return RpMaterialDestroy(a1, a2, a3, a4);
}

#include "..//crashlytics.h"

char g_bufRenderQueueCommand[200];
uintptr_t g_dwRenderQueueOffset;

char* (*RenderQueue__ProcessCommand)(uintptr_t thiz, char* a2);
char* RenderQueue__ProcessCommand_hook(uintptr_t thiz, char* a2)
{
	if (thiz && a2)
	{
		uintptr_t* dwRenderQueue = (uintptr_t*)thiz;

		memset(g_bufRenderQueueCommand, 0, sizeof(g_bufRenderQueueCommand));

		g_dwRenderQueueOffset = *(uintptr_t*)a2;
		snprintf(g_bufRenderQueueCommand, 190, "offset: %d | name: %s", g_dwRenderQueueOffset, (const char*)(*(dwRenderQueue + 100 + g_dwRenderQueueOffset)));

		return RenderQueue__ProcessCommand(thiz, a2);
	}
	else
	{
		return nullptr;
	}
}

int (*_rwFreeListFreeReal)(int a1, unsigned int a2);
int _rwFreeListFreeReal_hook(int a1, unsigned int a2)
{
	if (a1)
	{
		return _rwFreeListFreeReal(a1, a2);
	}
	else
	{
		return 0;
	}
}

uintptr_t* CTouchInterface__m_bWidgets;

static bool ShouldBeProcessedButton(int result)
{
	CTouchInterface__m_bWidgets = (uintptr_t*)(g_libGTASA + 0x00657E48);

	if (result == CTouchInterface__m_bWidgets[26] || result == CTouchInterface__m_bWidgets[27])
	{
		if (pNetGame)
		{
			CLocalPlayer* pLocal = pNetGame->GetPlayerPool()->GetLocalPlayer();
			if (pLocal)
			{
				if (pLocal->GetPlayerPed())
				{
					VEHICLE_TYPE* pVehicle = pLocal->GetPlayerPed()->GetGtaVehicle();
					if (pVehicle)
					{
						uintptr_t this_vtable = pVehicle->entity.vtable;
						this_vtable -= g_libGTASA;

						if (this_vtable == 0x5CCE60)
						{
							return 1;
						}
					}
				}
			}
		}
		return 0;
	}
	return 1;
}

int (*CWidgetButton__Draw)(uintptr_t thiz);
int CWidgetButton__Draw_hook(uintptr_t thiz)
{
	if(!pHud || !pHud->isHudToggle)return 0;

	return CWidgetButton__Draw(thiz);
}

int (*CWidgetButton__IsTouched)(CVector2D* a1);
int CWidgetButton__IsTouched_hook(CVector2D* a1)
{
	if(!pHud->isHudToggle)return 0;
	return CWidgetButton__IsTouched(a1);
}

void (*CWidgetButton__Update)(int result, int a2, int a3, int a4);
void CWidgetButton__Update_hook(int result, int a2, int a3, int a4)
{
	if (!result)
	{
		return;
	}
	CTouchInterface__m_bWidgets = (uintptr_t*)(g_libGTASA + 0x00657E48);

	((void (*)(unsigned int, unsigned int)) (g_libGTASA + 0x00274178 + 1))(CTouchInterface__m_bWidgets[0], 0); // Кнопка сесть в тачку

	if(pNetGame && pNetGame->m_GreenZoneState )
	{
		((void (*)(unsigned int, unsigned int)) (g_libGTASA + 0x00274178 + 1))(CTouchInterface__m_bWidgets[1], 0); // кулак
	}
	return CWidgetButton__Update(result, a2, a3, a4);
}


std::list<std::pair<unsigned int*, unsigned int>> resetEntriesVehicle;


RpMaterial* CVehicle__SetupRenderMatCB(RpMaterial* material, void* data)
{
	if (material)
	{
		if (material->texture)
		{
			CVehicle* pVeh = (CVehicle*)data;

			for (size_t i = 0; i < MAX_REPLACED_TEXTURES; i++)
			{
				if (pVeh->m_bReplaceTextureStatus[i])
				{
					if (!strcmp(&(material->texture->name[0]), &(pVeh->m_szReplacedTextures[i].szOld[0])))
					{
						if (pVeh->m_szReplacedTextures[i].pTexture)
						{
							resetEntriesVehicle.push_back(std::make_pair(reinterpret_cast<unsigned int*>(&(material->texture)), *reinterpret_cast<unsigned int*>(&(material->texture))));
							material->texture = pVeh->m_szReplacedTextures[i].pTexture;
							if (strstr(pVeh->m_szReplacedTextures[i].szOld, "ret_t"))
							{
								material->color.alpha = 255;
							}
						}
					}
				}
			}
		}
	}
	return material;
}

uintptr_t CVehicle__SetupRenderCB(uintptr_t atomic, void* data)
{
	if (*(RpGeometry * *)(atomic + 24))
	{
		((RpGeometry * (*)(RpGeometry*, uintptr_t, void*))(g_libGTASA + 0x001E284C + 1))(*(RpGeometry * *)(atomic + 24), (uintptr_t)CVehicle__SetupRenderMatCB, (void*)data); // RpGeometryForAllMaterials
	}

	return atomic;
}
#include "..//cryptors/MODELINFO_EDITABLE_result.h"
typedef uintptr_t RpClump;
void(*CVehicleModelInfo__SetEditableMaterials)(uintptr_t);
void CVehicleModelInfo__SetEditableMaterials_hook(uintptr_t clump)
{
	PROTECT_CODE_MODELINFO_EDITABLE;
	RpClump* pClump = (RpClump*)clump;

	

	if (pNetGame && pClump)
	{
		if (pNetGame->GetVehiclePool())
		{
			VEHICLEID vehId = pNetGame->GetVehiclePool()->FindIDFromRwObject((RwObject*)clump);
			CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(vehId);
			if (pVehicle )
			{
				VEHICLE_TYPE* ThisVehicleType;
				if((ThisVehicleType = GamePool_Vehicle_GetAt(pVehicle->m_dwGTAId)) == 0) return;
				if (ThisVehicleType->fHealth == 0.0f) return;
				if (ThisVehicleType->entity.vtable == g_libGTASA + 0x5C7358) return;

				if (pVehicle->m_bReplacedTexture)
				{
					// RpClump* RpClumpForAllAtomics(RpClump* clump, RpAtomicCallBack callback, void* pData);
					((RpClump * (*)(RpClump*, uintptr_t, void*))(g_libGTASA + 0x001E0EA0 + 1))(pClump, (uintptr_t)CVehicle__SetupRenderCB, (void*)pVehicle); // RpClumpForAllAtomics
				}
			}
		}
	}

	CVehicleModelInfo__SetEditableMaterials(clump);
}

#include "..//cryptors/RESET_AFTER_RENDER_result.h"

void (*CVehicle__ResetAfterRender)(uintptr_t);
void CVehicle__ResetAfterRender_hook(uintptr_t thiz)
{
	PROTECT_CODE_RESET_AFTER_RENDER;

	for (auto& p : resetEntriesVehicle)
	{
		*p.first = p.second;
	}
	resetEntriesVehicle.clear();

	CVehicle__ResetAfterRender(thiz);
}

#include "CRenderTarget.h"
#include "..//gui/CFontInstance.h"
#include "..//gui/CFontRenderer.h"
#include "CCustomPlateManager.h"

static bool g_bFirstPersonOnFootEnabled = false;
extern bool bDisableTestGovno;
void (*CGame__Process)();
void CGame__Process_hook()
{
	CGame__Process();

	static bool once = false;
	if (!once)
	{
		CCustomPlateManager::Initialise();
		CSnow::Initialise();
		once = true;
	}

	if (pNetGame && pNetGame->GetPlayerPool() && pNetGame->GetPlayerPool()->GetLocalPlayer())
	{
		CSnow::Process(pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed(), pGame->GetActiveInterior());
	}

//	if (pNetGame)
//	{
//		CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
//		if (pTextDrawPool) {
//			pTextDrawPool->SnapshotProcess();
//		}
//	}

	if (g_pWidgetManager)
	{
		PED_TYPE* pPed = GamePool_FindPlayerPed();
		if (g_pWidgetManager->GetSlotState(WIDGET_CAMERA_CYCLE) && pPed)
		{
			static uint32_t lastTick = GetTickCount();
			bool bPressed = false;
			if (g_pWidgetManager->IsTouched(WIDGET_CAMERA_CYCLE) && GetTickCount() - lastTick >= 250)
			{
				bPressed = true;
				lastTick = GetTickCount();
			}

			if (!CFirstPersonCamera::IsEnabled() && g_bFirstPersonOnFootEnabled)
			{
				CFirstPersonCamera::SetEnabled(true);
			}

			if (CFirstPersonCamera::IsEnabled() && !g_bFirstPersonOnFootEnabled)
			{
				CFirstPersonCamera::SetEnabled(false);
			}

			if (bPressed && !IN_VEHICLE(pPed))
			{
				CFirstPersonCamera::Toggle();
				if (CFirstPersonCamera::IsEnabled())
				{
					g_bFirstPersonOnFootEnabled = true;
				}
				else
				{
					g_bFirstPersonOnFootEnabled = false;
				}
			}
		}
	}

	CCustomPlateManager::Process();
}

#include "..//cryptors/AUTOMOBILE_COLLISION_result.h"

uint16_t g_usLastProcessedModelIndexAutomobile = 0;
int g_iLastProcessedModelIndexAutoEnt = 0;
void (*CAutomobile__ProcessEntityCollision)(VEHICLE_TYPE* a1, ENTITY_TYPE* a2, int a3);
void CAutomobile__ProcessEntityCollision_hook(VEHICLE_TYPE* a1, ENTITY_TYPE* a2, int a3)
{
	PROTECT_CODE_AUTOMOBILE_COLLISION;

	g_usLastProcessedModelIndexAutomobile = a1->entity.nModelIndex;
	g_iLastProcessedModelIndexAutoEnt = a2->nModelIndex;

	bool bReplace = false;
	void* pOld = nullptr;
	uint8_t* pColData = nullptr;

	

	if (pNetGame)
	{
		if (pNetGame->GetVehiclePool())
		{
			uint16_t vehId = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(a1);
			CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehId);
			if (pVeh)
			{
				if (pVeh->bHasSuspensionLines && pVeh->GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
				{
					pColData = GetCollisionDataFromModel(a1->entity.nModelIndex);
					if (pColData && pVeh->m_pSuspensionLines)
					{
						if (*(void**)(pColData + 16))
						{
							pOld = *(void**)(pColData + 16);
							*(void**)(pColData + 16) = pVeh->m_pSuspensionLines;
							bReplace = true;
						}
					}
				}
			}
		}
	}
	CAutomobile__ProcessEntityCollision(a1, a2, a3);
	if (bReplace)
	{
		*(void**)(pColData + 16) = pOld;
	}
}


void (*CWidget__DrawHelpIcon)();
void CWidget__DrawHelpIcon_hook()
{
	Log("CWidget__DrawHelpIcon_hook");
	return;
	//return CGame__Shutdown();
}

bool (*CGame__Shutdown)();
bool CGame__Shutdown_hook()
{
	Log("Exiting game...");
	CPatch::NOP(g_libGTASA + 0x00341FCC, 2); // nop PauseOpenSLES
	CPatch::NOP(g_libGTASA + 0x0039B262, 2); // CGame::Process(void)
	CPatch::NOP(g_libGTASA + 0x0046389E, 2); // nop saving

	if (pNetGame)
	{
		if (pNetGame->GetRakClient())
		{
			pNetGame->GetRakClient()->Disconnect(500, 0);
		}
	}

	g_pJavaWrapper->ExitGame();

	//std::this_thread::sleep_for(std::chrono::milliseconds(3000));

	return 0;
	//return CGame__Shutdown();
}

// TODO: VEHICLE RESET SUSPENSION
void (*CShadows__StoreCarLightShadow)(VEHICLE_TYPE* vehicle, int id, RwTexture* texture, VECTOR* posn, float frontX, float frontY, float sideX, float sideY, unsigned char red, unsigned char green, unsigned char blue, float maxViewAngle);
void CShadows__StoreCarLightShadow_hook(VEHICLE_TYPE* vehicle, int id, RwTexture* texture, VECTOR* posn, float frontX, float frontY, float sideX, float sideY, unsigned char red, unsigned char green, unsigned char blue, float maxViewAngle)
{
	uint8_t r, g, b;
	r = red;
	g = green;
	b = blue;
	if (pNetGame)
	{
		if (pNetGame->GetVehiclePool())
		{
			uint16_t vehid = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(vehicle);
			CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
			if (pVeh)
			{
				pVeh->ProcessHeadlightsColor(r, g, b);
			}
		}
	}

	CShadows__StoreCarLightShadow(vehicle, id, texture, posn, frontX, frontY, sideX, sideY, r, g, b, maxViewAngle);
}

void CVehicle__DoHeadLightReflectionTwin(CVehicle* pVeh, MATRIX4X4* a2)
{
	VEHICLE_TYPE* v2; // r4
	int v3; // r2
	MATRIX4X4* v4; // r5
	float* v5; // r3
	float v6; // s12
	float v7; // s5
	float* v8; // r2
	float v9; // r0
	float v10; // r1
	float v11; // r2
	float v12; // s14
	float v13; // s11
	float v14; // s15
	float v15; // s13
	float v16; // s10
	float v17; // s12
	float v18; // s15
	float v19; // ST08_4

	uintptr_t* dwModelarray = (uintptr_t*)(g_libGTASA + 0x87BF48);

	v2 = pVeh->m_pVehicle;
	v3 = *((uintptr_t*)v2 + 5);
	v4 = a2;
	v5 = *(float**)(dwModelarray[v2->entity.nModelIndex] + 116);
	v6 = *v5;
	v7 = v5[1];
	if (v3)
		v8 = (float*)(v3 + 48);
	else
		v8 = (float*)((char*)v2 + 4);
	v9 = *v8;
	v10 = v8[1];
	v11 = v8[2];
	v12 = *((float*)v4 + 5);
	v13 = *((float*)v4 + 4);
	v14 = (float)(v12 * v12) + (float)(v13 * v13);
	if (v14 != 0.0)
		v14 = 1.0 / sqrtf(v14);
	v15 = v6 * 4.0;
	v16 = (float)(v15 + v15) + 1.0;
	v17 = v13 * v14;
	v18 = v12 * v14;

	v19 = v15 * v18;

	VECTOR pos;
	memcpy(&pos, &(v2->entity.mat->pos), sizeof(VECTOR));
	pos.Z += 2.0f;

	CShadows__StoreCarLightShadow(
		v2,
		(uintptr_t)v2 + 24,
		pVeh->m_Shadow.pTexture,
		&pos,
		(float)(v15 + v15) * v17 * pVeh->m_Shadow.fSizeX,
		(float)(v15 + v15) * v18 * pVeh->m_Shadow.fSizeX,
		v19 * pVeh->m_Shadow.fSizeY,
		-(float)(v15 * v17) * pVeh->m_Shadow.fSizeY,
		pVeh->m_Shadow.r, pVeh->m_Shadow.g, pVeh->m_Shadow.b,
		7.0f);
}

void (*CVehicle__DoHeadLightBeam)(VEHICLE_TYPE* vehicle, int arg0, MATRIX4X4& matrix, unsigned char arg2);
void CVehicle__DoHeadLightBeam_hook(VEHICLE_TYPE* vehicle, int arg0, MATRIX4X4& matrix, unsigned char arg2)
{
	uint8_t r, g, b;
	r = 0xFF;
	g = 0xFF;
	b = 0xFF;
	if (pNetGame)
	{
		if (pNetGame->GetVehiclePool())
		{
			uint16_t vehid = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(vehicle);
			CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
			if (pVeh)
			{
				pVeh->ProcessHeadlightsColor(r, g, b);
			}
		}
	}

	*(uint8_t*)(g_libGTASA + 0x9BAA70) = r;
	*(uint8_t*)(g_libGTASA + 0x9BAA71) = g;
	*(uint8_t*)(g_libGTASA + 0x9BAA72) = b;

	*(uint8_t*)(g_libGTASA + 0x9BAA94) = r;
	*(uint8_t*)(g_libGTASA + 0x9BAA95) = g;
	*(uint8_t*)(g_libGTASA + 0x9BAA96) = b;

	*(uint8_t*)(g_libGTASA + 0x9BAB00) = r;
	*(uint8_t*)(g_libGTASA + 0x9BAB01) = g;
	*(uint8_t*)(g_libGTASA + 0x9BAB02) = b;

	CVehicle__DoHeadLightBeam(vehicle, arg0, matrix, arg2);

}

static CVehicle* g_pLastProcessedVehicleMatrix = nullptr;
static int g_iLastProcessedWheelVehicle = -1;

void (*CMatrix__Rotate)(void* thiz, float a1, float a2, float a3);
void CMatrix__Rotate_hook(void* thiz, float a1, float a2, float a3)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x003A9D76 + 1)
	{
		CMatrix__Rotate(thiz, a1, a2, a3);
		return;
	}

	CMatrix__Rotate(thiz, a1, a2, a3);
	if (g_pLastProcessedVehicleMatrix && g_iLastProcessedWheelVehicle != -1)
	{
		if (g_pLastProcessedVehicleMatrix->m_bWheelAlignmentX || g_pLastProcessedVehicleMatrix->m_bWheelAlignmentY)
		{
			if (g_iLastProcessedWheelVehicle == 2)
			{
				((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, 0.0f - g_pLastProcessedVehicleMatrix->m_fWheelAlignmentX); // CMatrix::RotateY
			}
			if (g_iLastProcessedWheelVehicle == 4)
			{
				((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, 0.0f - g_pLastProcessedVehicleMatrix->m_fWheelAlignmentY); // CMatrix::RotateY
			}
			if (g_iLastProcessedWheelVehicle == 5)
			{
				((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, g_pLastProcessedVehicleMatrix->m_fWheelAlignmentX); // CMatrix::RotateY
			}
			if (g_iLastProcessedWheelVehicle == 7)
			{
				((void(*)(void*, float))(g_libGTASA + 0x003E8BE4 + 1))(thiz, g_pLastProcessedVehicleMatrix->m_fWheelAlignmentY); // CMatrix::RotateY
			}
		}
	}
}

void (*CMatrix__SetScale)(void* thiz, float x, float y, float z);
void CMatrix__SetScale_hook(void* thiz, float x, float y, float z)
{
	if (g_pLastProcessedVehicleMatrix && g_iLastProcessedWheelVehicle != -1)
	{
		if (g_iLastProcessedWheelVehicle >= 2 || g_iLastProcessedWheelVehicle <= 7)
		{
			// front wheel
			if (g_pLastProcessedVehicleMatrix->m_bWheelSize)
			{
				y *= g_pLastProcessedVehicleMatrix->m_fWheelSize * 1.3f;
				z *= g_pLastProcessedVehicleMatrix->m_fWheelSize * 1.3f;
			}
			if (g_pLastProcessedVehicleMatrix->m_bWheelWidth)
			{
				x = g_pLastProcessedVehicleMatrix->m_fWheelWidth;
			}
		}
	}

	CMatrix__SetScale(thiz, x, y, z);
}

void (*CAutomobile__UpdateWheelMatrix)(VEHICLE_TYPE* thiz, int, int);
void CAutomobile__UpdateWheelMatrix_hook(VEHICLE_TYPE* thiz, int nodeIndex, int flags)
{
	if (g_pLastProcessedVehicleMatrix)
	{
		g_iLastProcessedWheelVehicle = nodeIndex;
	}

	CAutomobile__UpdateWheelMatrix(thiz, nodeIndex, flags);
}

void (*CAutomobile__PreRender)(VEHICLE_TYPE* thiz);
void CAutomobile__PreRender_hook(VEHICLE_TYPE* thiz)
{
	uintptr_t* dwModelarray = (uintptr_t*)(g_libGTASA + 0x87BF48);
	uint8_t* pModelInfoStart = (uint8_t*)dwModelarray[thiz->entity.nModelIndex];

	float fOldFront = *(float*)(pModelInfoStart + 88);
	float fOldRear = *(float*)(pModelInfoStart + 92);
	CVehicle* pVeh = nullptr;
	if (pNetGame)
	{
		if (pNetGame->GetVehiclePool())
		{
			uint16_t vehid = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(thiz);
			pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
			if (pVeh)
			{
				pVeh->ProcessWheelsOffset();
				g_pLastProcessedVehicleMatrix = pVeh;

				if (pVeh->m_bWheelSize)
				{
					*(float*)(pModelInfoStart + 92) = pVeh->m_fWheelSize;
					*(float*)(pModelInfoStart + 88) = pVeh->m_fWheelSize;
				}
				if (pVeh->m_bShadow && pVeh->m_Shadow.pTexture)
				{
					CVehicle__DoHeadLightReflectionTwin(pVeh, pVeh->m_pVehicle->entity.mat);
				}
			}
		}
	}

	CAutomobile__PreRender(thiz);

	*(float*)(pModelInfoStart + 88) = fOldFront;
	*(float*)(pModelInfoStart + 92) = fOldRear;

	g_pLastProcessedVehicleMatrix = nullptr;
	g_iLastProcessedWheelVehicle = -1;
}

#include "..//cryptors/INSTALLHOOKS_result.h"

void (*CTaskSimpleUseGun__RemoveStanceAnims)(void* thiz, void* ped, float a3);
void CTaskSimpleUseGun__RemoveStanceAnims_hook(void* thiz, void* ped, float a3)
{
	uint32_t v5 = *((uint32_t*)thiz + 11);
	if (v5)
	{
		if (*(uint32_t*)(v5 + 20))
		{
			CTaskSimpleUseGun__RemoveStanceAnims(thiz, ped, a3);
			return;
		}
		else
		{
			return;
		}
	}
	else
	{
		CTaskSimpleUseGun__RemoveStanceAnims(thiz, ped, a3);
	}
}

float (*CRadar__LimitRadarPoint)(float* a1);
float CRadar__LimitRadarPoint_hook(float* a1)
{
	if (*(uint8_t*)(g_libGTASA + 0x0063E0B4))
	{
		return sqrtf((float)(a1[1] * a1[1]) + (float)(*a1 * *a1));
	}

	if (!CRadarRect::IsEnabled())
	{
		return CRadar__LimitRadarPoint(a1);
	}
	float value = CRadarRect::CRadar__LimitRadarPoint_hook(a1);

	return value;
}

void (*CSprite2d__DrawBarChart)(float x, float y, unsigned short width, unsigned char height, float progress, signed char progressAdd,
	unsigned char drawPercentage, unsigned char drawBlackBorder, CRGBA* color, CRGBA* addColor);
void CSprite2d__DrawBarChart_hook(float x, float y, unsigned short width, unsigned char height, float progress, signed char progressAdd,
	unsigned char drawPercentage, unsigned char drawBlackBorder, CRGBA* color, CRGBA* addColor)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	float fX = x;
	float fY = y;

	unsigned short usWidth = width;
	unsigned char usHeight = height;

	if (dwRetAddr == 0x0027D524 + 1)
	{
		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_HP).X != -1)
		{
			fX = pGUI->ScaleX(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_HP).X);
		}
		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_HP).Y != -1)
		{
			fY = pGUI->ScaleY(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_HP).Y);
		}

		if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_HP).X != -1)
		{
			usWidth = pGUI->ScaleX(CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_HP).X);
		}
		if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_HP).Y != -1)
		{
			usHeight = pGUI->ScaleY(CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_HP).Y);
		}

		if (CAdjustableHudColors::IsUsingHudColor(E_HUD_ELEMENT::HUD_HP))
		{
			color->A = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).A;
			color->R = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).R;
			color->G = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).G;
			color->B = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).B;
		}
		color->A = 0;
		color->R = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).R;
		color->G = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).G;
		color->B = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_HP).B;
	}
	else if (dwRetAddr == 0x0027D83E + 1)
	{
		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_ARMOR).X != -1)
		{
			fX = pGUI->ScaleX(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_ARMOR).X);
		}
		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_ARMOR).Y != -1)
		{
			fY = pGUI->ScaleY(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_ARMOR).Y);
		}

		if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_ARMOR).X != -1)
		{
			usWidth = pGUI->ScaleX(CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_ARMOR).X);
		}
		if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_ARMOR).Y != -1)
		{
			usHeight = pGUI->ScaleY(CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_ARMOR).Y);
		}

		if (CAdjustableHudColors::IsUsingHudColor(E_HUD_ELEMENT::HUD_ARMOR))
		{
			color->A = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).A;
			color->R = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).R;
			color->G = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).G;
			color->B = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).B;
		}
		color->A = 0;
		color->R = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).R;
		color->G = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).G;
		color->B = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_ARMOR).B;
	}
	CSprite2d__DrawBarChart(fX, fY, usWidth, usHeight, progress, progressAdd, drawPercentage, drawBlackBorder, color, addColor);
}
static int g_iCurrentWanted = 0;
static float g_fInitialPos = 0.0f;
void (*CWidgetPlayerInfo__DrawWanted)(void*);
void CWidgetPlayerInfo__DrawWanted_hook(void* thiz)
{
	g_iCurrentWanted = 0;
	g_fInitialPos = *((float*)thiz + 10);
	CWidgetPlayerInfo__DrawWanted(thiz);
	g_iCurrentWanted = 0;
}

void (*CFont__PrintString)(float x, float y, uint16_t* text);
void CFont__PrintString_hook(float x, float y, uint16_t* text)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	float fX = x;
	float fY = y;

//	if (dwRetAddr == 0x0027E15C + 1) // money
//	{
//		if (CAdjustableHudColors::IsUsingHudColor(E_HUD_ELEMENT::HUD_MONEY))
//		{
//			CRGBA col = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_MONEY);
//			uint32_t m1 = col.ToInt();
//			CFont::SetColor(&m1);
//		}
//
//		if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_MONEY).X != -1)
//		{
//			float value = (float)CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_MONEY).X / 100.0f;
//			CFont::SetScale(value, 0.0f);
//		}
//
//		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_MONEY).X != -1)
//		{
//			fX = pGUI->ScaleX(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_MONEY).X);
//		}
//		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_MONEY).Y != -1)
//		{
//			fY = pGUI->ScaleY(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_MONEY).Y);
//		}
//	}
	if (dwRetAddr == 0x0027D9E6 + 1) // wanted
	{
		if (CAdjustableHudColors::IsUsingHudColor(E_HUD_ELEMENT::HUD_WANTED))
		{
			CRGBA col = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_WANTED);
			uint32_t m1 = col.ToInt();
			CFont::SetColor(&m1);
		}

		if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_WANTED).X != -1)
		{
			float value = (float)CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_WANTED).X / 100.0f;
			CFont::SetScale(value, 0.0f);
		}
		
		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_WANTED).X != -1)
		{
			fX -= g_fInitialPos;
			fX += pGUI->ScaleX(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_WANTED).X);
		}
		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_WANTED).Y != -1)
		{
			fY = pGUI->ScaleY(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_WANTED).Y);
		}
		g_iCurrentWanted++;

	}
	else if (dwRetAddr == 0x0027D330 + 1) // ammo text
	{
		if (CAdjustableHudColors::IsUsingHudColor(E_HUD_ELEMENT::HUD_AMMO))
		{
			CRGBA col = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_AMMO);
			uint32_t m1 = col.ToInt();
			CFont::SetColor(&m1);
		}

		if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_AMMO).X != -1)
		{
			float value = (float)CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_AMMO).X / 100.0f;
			CFont::SetScale(value, 0.0f);
		}

		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_AMMO).X != -1)
		{
			fX = pGUI->ScaleX(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_AMMO).X);
		}
		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_AMMO).Y != -1)
		{
			fY = pGUI->ScaleY(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_AMMO).Y);
		}
	}
	CFont__PrintString(fX, fY, text);
}

void (*CSprite2d__Draw)(CSprite2d* a1, CRect* a2, CRGBA* a3);
void CSprite2d__Draw_hook(CSprite2d* a1, CRect* a2, CRGBA* a3)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (!pGUI)
	{
		return CSprite2d__Draw(a1, a2, a3);
	}

	if (dwRetAddr == 0x003D3796 + 1 || dwRetAddr == 0x003D376C + 1 || dwRetAddr == 0x003D373E + 1 || dwRetAddr == 0x003D3710 + 1)
	{
		if(!pHud->isHudToggle)return;
		if (CRadarRect::m_pDiscTexture == nullptr)
		{
			CRadarRect::m_pDiscTexture = a1->m_pRwTexture;
		}
		if (CRadarRect::IsEnabled() && CRadarRect::m_pRectTexture)
		{
			a1->m_pRwTexture = CRadarRect::m_pRectTexture;
			//CChatWindow::AddDebugMessage("Replacing for rect");
		}
		else
		{
			a1->m_pRwTexture = CRadarRect::m_pDiscTexture;
			//CChatWindow::AddDebugMessage("Replacing for disc");
		}

		if (CAdjustableHudColors::IsUsingHudColor(E_HUD_ELEMENT::HUD_RADAR))
		{
			CRGBA col = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_RADAR);
			a3->A = 255;
			a3->B = 155;
			a3->G = 5;
			a3->R = 7;
		}
		float* thiz = (float*) * (uintptr_t*)(g_libGTASA + 0x6580C8);

		//thiz[0] = (float)100;
		if (thiz)
		{

			thiz[3] = pHud->radarx1;
			thiz[4] = pHud->radary1;

			thiz[5] = 38.0f;
			thiz[6] = 38.0f;
		}
		
	}

	return CSprite2d__Draw(a1, a2, a3);
}

void (*CWidgetPlayerInfo__DrawWeaponIcon)(float* thiz, void* ped, CRect rect, float a4);
void CWidgetPlayerInfo__DrawWeaponIcon_hook(float* thiz, void* ped, CRect rect, float a4)
{
	float diffX = rect.right - rect.left;
	float diffY = rect.bottom - rect.top;
	if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIST).X != -1)
	{
		rect.left = pGUI->ScaleX(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIST).X);
		rect.right = pGUI->ScaleX(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIST).X) + diffX;

		thiz[38] = rect.left;
		thiz[40] = rect.right;
	}
	if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIST).Y != -1)
	{
		rect.top = pGUI->ScaleY(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIST).Y);
		rect.bottom = pGUI->ScaleY(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_FIST).Y) + diffY;

		thiz[39] = rect.bottom;
		thiz[41] = rect.top;
	}

	if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_FIST).X != -1)
	{
		float coef = (float)CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_FIST).X / 100.0f;
		float diffX = rect.right - rect.left;
		float diffY = rect.bottom - rect.top;
		diffX *= coef;
		diffY *= coef;

		rect.right = rect.left + diffX;
		rect.bottom = rect.top + diffY;
	}

	return CWidgetPlayerInfo__DrawWeaponIcon(thiz, ped, rect, a4);
}

void(*CHud__Draw)();
void CHud__Draw_hook()
{
	float* thiz = (float*) * (uintptr_t*)(g_libGTASA + 0x6580C8);
	if (thiz)
	{
		thiz[5] = 45.0f;
		thiz[6] = 45.0f;
	}
	CHud__Draw();
}

void (*CCam__Process)(uintptr_t);
void CCam__Process_hook(uintptr_t thiz)
{

	VECTOR vecSpeed;
	CVehicle* pVeh = nullptr;
	float pOld = *(float*)(g_libGTASA + 0x00608558);
	if (pNetGame && (*(uint16_t*)(thiz + 14) == 18 || *(uint16_t*)(thiz + 14) == 16) && CFirstPersonCamera::IsEnabled())
	{
		if (pNetGame->GetPlayerPool())
		{
			if (pNetGame->GetPlayerPool()->GetLocalPlayer())
			{
				CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
				pVeh = pNetGame->GetVehiclePool()->GetAt(pNetGame->GetPlayerPool()->GetLocalPlayer()->m_CurrentVehicle);
				if (pVeh)
				{
					pVeh->GetMoveSpeedVector(&vecSpeed);
					VECTOR vec;
					vec.X = vecSpeed.X * 6.0f;
					vec.Y = vecSpeed.Y * 6.0f;
					vec.Z = vecSpeed.Z * 6.0f;
					pVeh->SetMoveSpeedVector(vec);
					*(float*)(g_libGTASA + 0x00608558) = 200.0f;
				}
			}
		}
	}

	CCam__Process(thiz);
	if (pVeh)
	{
		pVeh->SetMoveSpeedVector(vecSpeed);
		*(float*)(g_libGTASA + 0x00608558) = pOld;
	}
	if (*(uint16_t*)(thiz + 14) == 4 || *(uint16_t*)(thiz + 14) == 53) // 53 is weapon
	{
		if (pNetGame)
		{
			if (pNetGame->GetPlayerPool())
			{
				if (pNetGame->GetPlayerPool()->GetLocalPlayer())
				{
					CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
					if (pPed)
					{
						*(uint32_t*)(g_libGTASA + 0x008B0808 + 120) = 0xFFFFFFFF;
						*(uint32_t*)(g_libGTASA + 0x008B0808 + 124) = 0xFFFFFFFF;
						*(uint8_t*)(g_libGTASA + 0x008B0808 + 40) = 0;
						CFirstPersonCamera::ProcessCameraOnFoot(thiz, pPed);
					}
				}
			}
		}
	}
	if(* (uint16_t*)(thiz + 14) == 18 || *(uint16_t*)(thiz + 14) == 16) // in car
	{
		if (pNetGame)
		{
			if (pNetGame->GetPlayerPool())
			{
				if (pNetGame->GetPlayerPool()->GetLocalPlayer())
				{
					CPlayerPed* pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
					if (pPed)
					{
						*(uint32_t*)(g_libGTASA + 0x008B0808 + 120) = 0xFFFFFFFF;
						*(uint32_t*)(g_libGTASA + 0x008B0808 + 124) = 0xFFFFFFFF;
						*(uint8_t*)(g_libGTASA + 0x008B0808 + 40) = 0;
						CFirstPersonCamera::ProcessCameraInVeh(thiz, pPed, pVeh);
					}
				}
			}
		}
	}
}
int g_iCounterVehicleCamera = 0;
int (*CPad__CycleCameraModeDownJustDown)(void*);
int CPad__CycleCameraModeDownJustDown_hook(void* thiz)
{
	if (!g_pWidgetManager)
	{
		return 0;
	}
	if (!g_pWidgetManager->GetSlotState(WIDGET_CAMERA_CYCLE))
	{
		return 0;
	}
	PED_TYPE* pPed = GamePool_FindPlayerPed();
	if (!pPed)
	{
		return 0;
	}

	static uint32_t lastTick = GetTickCount();
	bool bPressed = false;
	if (g_pWidgetManager->IsTouched(WIDGET_CAMERA_CYCLE) && GetTickCount() - lastTick >= 250)
	{
		bPressed = true;
		lastTick = GetTickCount();
	}

	if (IN_VEHICLE(pPed))
	{
		if (bPressed)
		{
			g_iCounterVehicleCamera++;
		}
		if (g_iCounterVehicleCamera == 6)
		{
			CFirstPersonCamera::SetEnabled(true);
			return 0;
		}
		else if (g_iCounterVehicleCamera >= 7)
		{
			g_iCounterVehicleCamera = 0;
			CFirstPersonCamera::SetEnabled(false);
			return 1;
			
		}
		else
		{
			CFirstPersonCamera::SetEnabled(false);
		}

		return bPressed;
	}
	return 0;
}

int (*CEntity__GetIsOnScreen)(ENTITY_TYPE*);
int CEntity__GetIsOnScreen_hook(ENTITY_TYPE* thiz)
{
	int retn = CEntity__GetIsOnScreen(thiz);
	
	/*if (CSkyBox::m_pSkyObjectDaily)
	{
		if (thiz == CSkyBox::m_pSkyObjectAfternoon->m_pEntity || thiz == CSkyBox::m_pSkyObjectDaily->m_pEntity ||
			thiz == CSkyBox::m_pSkyObjectEvening->m_pEntity || thiz == CSkyBox::m_pSkyObjectNight->m_pEntity)
		{
			return 1;
		}
	}*/

	return retn;
}

void (*FxEmitterBP_c__Render)(uintptr_t* a1, int a2, int a3, float a4, char a5);
void FxEmitterBP_c__Render_hook(uintptr_t* a1, int a2, int a3, float a4, char a5)
{
	if(!a1 || !a2) return;
	uintptr_t* temp = *((uintptr_t**)a1 + 3);
	if (!temp)
	{
		return;
	}
	FxEmitterBP_c__Render(a1, a2, a3, a4, a5);
}

void(*CStreaming__RemoveModel)(int);
void CStreaming__RemoveModel_hook(int model)
{
	Log("Removing model %d", model);
	CStreaming__RemoveModel(model);
}

int g_iLastProcessedSkinCollision = 228;
int g_iLastProcessedEntityCollision = 228;

void (*CPed__ProcessEntityCollision)(PED_TYPE* thiz, ENTITY_TYPE* ent, void* colPoint);
void CPed__ProcessEntityCollision_hook(PED_TYPE* thiz, ENTITY_TYPE* ent, void* colPoint)
{
	g_iLastProcessedSkinCollision = thiz->entity.nModelIndex;
	g_iLastProcessedEntityCollision = ent->nModelIndex;

	CPed__ProcessEntityCollision(thiz, ent, colPoint);
}

int (*CTaskSimpleGetUp__ProcessPed)(uintptr_t* thiz, PED_TYPE* ped);
int CTaskSimpleGetUp__ProcessPed_hook(uintptr_t* thiz, PED_TYPE* ped)
{
	if(!ped)return 0;

	return CTaskSimpleGetUp__ProcessPed(thiz, ped);
}

int (*emu_ArraysDelete)(unsigned int a1, int a2, int a3, int a4);
int emu_ArraysDelete_hook(unsigned int a1, int a2, int a3, int a4)
{
	if (a1 > g_libGTASA)
		return emu_ArraysDelete(a1, a2, a3, a4);
	return 0;
}

char (*CFileMgr__ReadLine)(int a1, char*, int);
char CFileMgr__ReadLine_hook(int a1, char *a2, int a3)
{
	char retv = CFileMgr__ReadLine(a1, a2, a3);
	char *pStr = a2;
	int value = *(int *)pStr;
	//Log("%d, %d, %d, %d, %d", value, g_unobfuscate(g_iIdentifierVersion2), UNOBFUSCATE_DATA(value), OBFUSCATE_DATA(value) );
	//Log("%c %c", a2[0], a2[1]);
	if (value == g_unobfuscate(g_iIdentifierVersion2))
	{
		Log("Helloo");
		pStr += 4;
		int length = *(int *)pStr;
		pStr -= 4;
		memcpy((void *)pStr, (const void *)&pStr[8], length);

		pStr[length] = 0;
		std::stringstream ss;

		uint32_t keyi = g_unobfuscate(g_i64Encrypt);

		ss << keyi;

		std::string key(ss.str());
		std::string val(pStr);

		std::string decr = decrypt(val, key);

		strcpy((char *)a2, decr.c_str());
	}
	return retv;
}
int (*_RwTextureDestroy)(RwTexture *);
int _RwTextureDestroy_hook(RwTexture *texture)
{
	if (!texture)
		return 1;
	return _RwTextureDestroy(texture);
}

// баня едет
//int (*TextureDatabaseRuntime__SortEntries)(uintptr_t *thiz, int a2);
//int TextureDatabaseRuntime__SortEntries_hook(uintptr_t *thiz, int a2)
//{
//
//	int v84 = *((DWORD *)v4 + 0x2A);
//	int v68 = v86[2 * v66++];
//	int v79 = *(DWORD *)(v84 + 4 * v68);
//
//	return TextureDatabaseRuntime__SortEntries(thiz, a2);
//}

int (*RwResourcesFreeResEntry)(int);
int RwResourcesFreeResEntry_hook(int a1)
{
	int result; // r0

	if (a1)
		result = RwResourcesFreeResEntry(a1);
	else
		result = 0;
	return result;
}

int (*emu_ArraysGetID)(unsigned int);
int emu_ArraysGetID_hook(unsigned int a1)
{
	bool v1;	// zf
	int result; // r0

	v1 = a1 == 0;
	if (a1)
		v1 = *(uint32_t *)(a1 + 36) == 0;
	if (v1)
		result = 0;
	else
		result = emu_ArraysGetID(a1);
	return result;
}

int (*SetCompAlphaCB)(int, int);
int SetCompAlphaCB_hook(int a1, char a2)
{
	int result; // r0

	if (a1)
		result = SetCompAlphaCB(a1, a2);
	else
		result = 0;
	return result;
}

int (*CEntity__RegisterReference)(ENTITY_TYPE *thiz, ENTITY_TYPE **a2);
int CEntity__RegisterReference_hook(ENTITY_TYPE *thiz, ENTITY_TYPE **a2)
{
	if(!thiz)return false;
	if(!a2)return false;
//    if( !*((DWORD *)thiz + 4)  )return false;
//	if(! thiz + 0x36)return false;

	return CEntity__RegisterReference(thiz, a2);
}

float (*CDraw__SetFOV)(float thiz, float a2);
float CDraw__SetFOV_hook(float thiz, float a2)
{
	float tmp = (float)((float)((float)(*(float *)&*(float *)(g_libGTASA + 0x0098525C) - 1.3333) * 11.0) / 0.44444) + thiz;
	if(tmp > 100) tmp = 100.0;
	*(float *)(g_libGTASA + 0x00610968) = tmp;
	return thiz;
}

int (*MobileSettings__GetMaxResWidth)();
int MobileSettings__GetMaxResWidth_hook()
{
	//Log("res = %d", ((int(*)())(g_libGTASA + 0x0023816C + 1))());
	return (int)( ((int(*)())(g_libGTASA + 0x0023816C + 1))()/1.1 );
}

int (*CCollision__ProcessLineTriangle)(float *a1, int a2, uint16_t *a3, int16_t *a4, int a5, float *a6, int a7);
int CCollision__ProcessLineTriangle_hook(float *a1, int a2, uint16_t *a3, int16_t *a4, int a5, float *a6, int a7)
{
	if(!a4 || !a3)
	{
		CChatWindow::AddDebugMessage("Crash collision %d %d", g_iLastProcessedEntityCollision, g_iLastProcessedModelIndexAutoEnt);
		return 0;
	}
	return CCollision__ProcessLineTriangle(a1, a2, a3, a4, a5, a6, a7);
}

char **(*CPhysical__Add)(uintptr_t thiz);
char **CPhysical__Add_hook(uintptr_t thiz)
{
	// char **result = 0;

	if (pNetGame)
	{
		CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			for (size_t i = 0; i < 10; i++)
			{
				if (pPlayerPed->m_aAttachedObjects[i].bState)
				{
					if (pPlayerPed->m_aAttachedObjects[i].pObject)
						if ((uintptr_t)pPlayerPed->m_aAttachedObjects[i].pObject->m_pEntity == thiz)
						{
							CObject *pObject = pPlayerPed->m_aAttachedObjects[i].pObject;
							if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
								pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
							{
								/*if(pChatWindow)
								{
									pChatWindow->AddDebugMessage("WARNING!!! WARNING!!! WARNING!!! CRASH EXCEPTED!!!");
								}*/
								return 0;
							}
							// Log("Processing local attached object");
							// result = CPhysical__Add(thiz);
						}
				}
			}
		}

		CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();

		if (pVehiclePool)
		{
			for (size_t i = 0; i < MAX_VEHICLES; i++)
			{
				if (pVehiclePool->GetSlotState(i))
				{
					CVehicle *pVehicle = pVehiclePool->GetAt(i);
					if (pVehicle && pVehicle->IsAdded())
					{
						/*CObject* pObject = pVehicle->Att((ENTITY_TYPE*)thiz);
						if (pObject != nullptr)
						{
							if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
								pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
							{
								/*if(pChatWindow)
								{
									pChatWindow->AddDebugMessage("WARNING!!! WARNING!!! WARNING!!! CRASH EXCEPTED!!!");
								}
								return 0;
							}
						}*/
					}
				}
			}
		}

		CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

		if (pPlayerPool)
		{
			for (size_t i = 0; i < MAX_PLAYERS; i++)
			{
				if (pPlayerPool->GetSlotState(i))
				{
					CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(i);
					if (pRemotePlayer)
					{
						if (pRemotePlayer->GetPlayerPed() && pRemotePlayer->GetPlayerPed()->IsAdded())
						{
							pPlayerPed = pRemotePlayer->GetPlayerPed();
							for (size_t i = 0; i < 10; i++)
							{
								if (pPlayerPed->m_aAttachedObjects[i].bState)
								{
									if ((uintptr_t)pPlayerPed->m_aAttachedObjects[i].pObject->m_pEntity == thiz)
									{
										CObject *pObject = pPlayerPed->m_aAttachedObjects[i].pObject;
										if (pObject->m_pEntity->mat->pos.X > 20000.0f || pObject->m_pEntity->mat->pos.Y > 20000.0f || pObject->m_pEntity->mat->pos.Z > 20000.0f ||
											pObject->m_pEntity->mat->pos.X < -20000.0f || pObject->m_pEntity->mat->pos.Y < -20000.0f || pObject->m_pEntity->mat->pos.Z < -20000.0f)
										{
											/*if(pChatWindow)
											{
												pChatWindow->AddDebugMessage("WARNING!!! WARNING!!! WARNING!!! CRASH EXCEPTED!!!");
											}*/
											return 0;
										}
										// Log("Processing remote attached object. Player: %d", i);
										// Log("is added: %d | model index: %d | gta id: %d | x: %.2f | y: %.2f | z: %.2f | flags: %d | vtable: %d | unk: %d", pObject->IsAdded(), pObject->GetModelIndex(), pObject->m_dwGTAId, pObject->m_pEntity->mat->pos.X, pObject->m_pEntity->mat->pos.Y, pObject->m_pEntity->mat->pos.Z, pObject->m_pEntity->dwProcessingFlags, pObject->m_pEntity->vtable, pObject->m_pEntity->dwUnkModelRel);
										// result = CPhysical__Add(thiz);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	/*if(result == 0)
	{
		Log("Processing unknown object.");
		result = CPhysical__Add(thiz);
	}*/

	// Log("Processed.");

	return CPhysical__Add(thiz);
}


int (*CCollision__ProcessVerticalLine)(float *a1, float *a2, int a3, int a4, int *a5, int a6, int a7, int a8);
int CCollision__ProcessVerticalLine_hook(float *a1, float *a2, int a3, int a4, int *a5, int a6, int a7, int a8)
{
	int result; // r0

	if (a3)
		result = CCollision__ProcessVerticalLine(a1, a2, a3, a4, a5, a6, a7, a8);
	else
		result = 0;
	return result;
}


int (*CWeapon__GenerateDamageEvent)(PED_TYPE *victim, ENTITY_TYPE *creator, unsigned int weaponType, int damageFactor, ePedPieceTypes pedPiece, int direction);
int CWeapon__GenerateDamageEvent_hook(PED_TYPE *victim, ENTITY_TYPE *creator, unsigned int weaponType, int damageFactor, ePedPieceTypes pedPiece, int direction)
{
	if (pedPiece != PED_PIECE_HEAD)
	{
		// Disable ComputeDamageAnim when it's not a head!
		CPatch::NOP(g_libGTASA + 0x566B30, 2);
		CPatch::NOP(g_libGTASA + 0x566B88, 2);
	}
	int result = CWeapon__GenerateDamageEvent(victim, creator, weaponType, damageFactor, pedPiece, direction);
	if (pedPiece != PED_PIECE_HEAD)
	{
		// Restore bytes
		CPatch::WriteMemory(g_libGTASA + 0x566B30, (uintptr_t) "\xC1\xF5\xD8\xF9", 4);
		CPatch::WriteMemory(g_libGTASA + 0x566B88, (uintptr_t) "\xBB\xF5\xFE\xF8", 4);
	}
	return result;
}


int (*CTaskSimpleUseGun__SetPedPosition)(uintptr_t thiz, uintptr_t a2);
int CTaskSimpleUseGun__SetPedPosition_hook(uintptr_t thiz, uintptr_t a2)
{
	unsigned char v1 = *((unsigned char*)thiz + 13);
	bool bChangeTheResult = false;

	PED_TYPE* pPedPlayer = (PED_TYPE*)a2;
	if(pPedPlayer && pNetGame)
	{
		if(v1 == 0)
		{
			CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
			if(pPlayerPool)
			{
				CPlayerPed *pPlayerPed;
				if(pPedPlayer == GamePool_FindPlayerPed())
					pPlayerPed = pGame->FindPlayerPed();
				else
				{
					PLAYERID playerId = pPlayerPool->FindRemotePlayerIDFromGtaPtr(pPedPlayer);
					if(playerId != INVALID_PLAYER_ID)
					{
						CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(playerId);
						if(pRemotePlayer)
							pPlayerPed = pRemotePlayer->GetPlayerPed();
					}
				}

				if(pPlayerPed)
				{
					if((pPlayerPed->GetCurrentWeapon() == 42 || pPlayerPed->GetCurrentWeapon() == 41) && pPlayerPed->GetGtaVehicle() == 0)
					{
						CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
						if(pVehiclePool)
						{
							for(VEHICLEID veh = 0; veh < MAX_VEHICLES; veh++)
							{
								if(pVehiclePool->GetSlotState(veh))
								{
									CVehicle* pVehicle = pVehiclePool->GetAt(veh);
									if(pVehicle)
									{
										MATRIX4X4 vehicleMat, playerMat;
										pVehicle->GetMatrix(&vehicleMat);
										pPlayerPed->GetMatrix(&playerMat);

										float fSX = (vehicleMat.pos.X - playerMat.pos.X) * (vehicleMat.pos.X - playerMat.pos.X);
										float fSY = (vehicleMat.pos.Y - playerMat.pos.Y) * (vehicleMat.pos.Y - playerMat.pos.Y);
										float fSZ = (vehicleMat.pos.Z - playerMat.pos.Z) * (vehicleMat.pos.Z - playerMat.pos.Z);

										float fDistance = (float)sqrt(fSX + fSY + fSZ);

										if(fDistance <= 100.0f)
											*((unsigned char*)thiz + 13) |= 1;
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return CTaskSimpleUseGun__SetPedPosition(thiz, a2);
}

int (*CTaskSimpleCarSetPedInAsDriver__ProcessPed)(uintptr_t *thiz, PED_TYPE *a2);
int CTaskSimpleCarSetPedInAsDriver__ProcessPed_hook(uintptr_t *thiz, PED_TYPE *a2)
{
	if(!a2)return false;
    if( !*((DWORD *)thiz + 4)  )return false;

	return CTaskSimpleCarSetPedInAsDriver__ProcessPed(thiz, a2);
}

bool (*CVehicle__GetVehicleLightsStatus)(VEHICLE_TYPE *_pVehicle);
bool CVehicle__GetVehicleLightsStatus_hook(VEHICLE_TYPE *_pVehicle)
{
	*(bool*)(g_libGTASA + 0x97180D) = false;
	if(!pNetGame) return false;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
    if(!pVehiclePool) return false;

    CVehicle *pVehicle = pVehiclePool->FindVehicle(_pVehicle);
	if(!pVehicle)return false;

	return pVehicle->GetLightsState();

}


void (*RenderEffects)();
void RenderEffects_hook()
{
	// fix render spheres
//	if(pSettings->Get().bnoFX)
//	{
		RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)0);
		RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)5);
		RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)6);
		RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)1);
		RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
		RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);

		CPatch::CallFunction<void>(g_libGTASA + 0x54C330 + 1);
		CPatch::CallFunction<void>(g_libGTASA + 0x54D738 + 1);
//	}
//	else
//	{
//		RenderEffects();
//	}
}

int (*CAudioEngine__Service)(uintptr_t a1);
int CAudioEngine__Service_hook(uintptr_t a1)
{
	if(!a1) return 0;
	return CAudioEngine__Service(a1);
}

int (*CAEVehicleAudioEntity__GetAccelAndBrake)(int a1, int a2);
int CAEVehicleAudioEntity__GetAccelAndBrake_hook(int a1, int a2)
{
	if(!a1 || !a2) return 0;
	if(!*(uintptr_t *)(a1 + 12)) return 0;
	return CAEVehicleAudioEntity__GetAccelAndBrake(a1, a2);
}

uint32_t (*CWeapon__FireSniper)(WEAPON_SLOT_TYPE *pWeaponSlot, PED_TYPE *pFiringEntity, ENTITY_TYPE *a3, VECTOR *vecOrigin);
uint32_t CWeapon__FireSniper_hook(WEAPON_SLOT_TYPE *pWeaponSlot, PED_TYPE *pFiringEntity, ENTITY_TYPE *a3, VECTOR *vecOrigin)
{
	if(GamePool_FindPlayerPed() == pFiringEntity)
	{
		if(pGame)
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if(pPlayerPed)
				pPlayerPed->FireInstant();
		}
	}

	return 1;
}

VECTOR& (*FindPlayerSpeed)(int a1);
VECTOR& FindPlayerSpeed_hook(int a1)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr":"=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(dwRetAddr == 0x3DF20E + 1)
	{
		if(pNetGame)
		{
			CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
			if(pPlayerPed &&
			   pPlayerPed->IsInVehicle() &&
			   pPlayerPed->IsAPassenger())
			{
				VECTOR vec = {0.1f, 0.1f, 0.1f};
				return vec;
			}
		}
	}

	return FindPlayerSpeed(a1);
}


void InstallHooks()
{
	Log("InstallHooks");

	PROTECT_CODE_INSTALLHOOKS;

	CPatch::InlineHook(g_libGTASA, 0x56668C, &CWeapon__FireSniper_hook, &CWeapon__FireSniper);
	// audio
//	CPatch::InlineHook(g_libGTASA, 0x368850, &CAudioEngine__Service_hook, &CAudioEngine__Service);
	//CPatch::InlineHook(g_libGTASA, 0x35AC44, &CAEVehicleAudioEntity__GetAccelAndBrake_hook, &CAEVehicleAudioEntity__GetAccelAndBrake);

	CPatch::InlineHook(g_libGTASA, 0x29947C, &CCollision__ProcessVerticalLine_hook, &CCollision__ProcessVerticalLine);
	CPatch::InlineHook(g_libGTASA, 0x5669D8, &CWeapon__GenerateDamageEvent_hook, &CWeapon__GenerateDamageEvent);
	// Fire extingusher fix
	CPatch::InlineHook(g_libGTASA, 0x46D6AC, &CTaskSimpleUseGun__SetPedPosition_hook, &CTaskSimpleUseGun__SetPedPosition);

	//SetUpHook(g_libGTASA+0x291104, (uintptr_t)CStreaming__ConvertBufferToObject_hook, (uintptr_t*)&CStreaming__ConvertBufferToObject);
	//SetUpHook(g_libGTASA+0x3961C8, (uintptr_t)CFileMgr__ReadLine_hook, (uintptr_t*)&CFileMgr__ReadLine);

	CPatch::InlineHook(g_libGTASA, 0x0032217C, &CEventHandler__HandleEvents_hook, &CEventHandler__HandleEvents);
	CPatch::InlineHook(g_libGTASA, 0x00281398, &CWidgetRegionLook__Update_hook, &CWidgetRegionLook__Update);

	CPatch::InlineHook(g_libGTASA, 0x3D7CA8, &CLoadingScreen_DisplayPCScreen_hook, &CLoadingScreen_DisplayPCScreen);

	CPatch::InlineHook(g_libGTASA, 0x39AEF4, &Render2dStuff_hook, &Render2dStuff);
	CPatch::InlineHook(g_libGTASA, 0x39B098, &Render2dStuffAfterFade_hook, &Render2dStuffAfterFade);
	CPatch::InlineHook(g_libGTASA, 0x239D5C, &TouchEvent_hook, &TouchEvent);
    //

	CPatch::InlineHook(g_libGTASA, 0x336690, &CModelInfo_AddPedModel_hook, &CModelInfo_AddPedModel); // hook is dangerous
	CPatch::InlineHook(g_libGTASA, 0x3DBA88, &CRadar__GetRadarTraceColor_hook, &CRadar__GetRadarTraceColor); // dangerous
	CPatch::InlineHook(g_libGTASA, 0x3DAF84, &CRadar__SetCoordBlip_hook, &CRadar__SetCoordBlip);
	CPatch::InlineHook(g_libGTASA, 0x3DE9A8, &CRadar__DrawRadarGangOverlay_hook, &CRadar__DrawRadarGangOverlay);

	CPatch::CodeInject(g_libGTASA+0x2D99F4, (uintptr_t)PickupPickUp_hook, 1);
	CPatch::InlineHook(g_libGTASA, 0x00327528, &ComputeDamageResponse_hooked, (&ComputeDamageResponse));
	CPatch::InlineHook(g_libGTASA, 0x567964, &CWeapon__FireInstantHit_hook, (&CWeapon__FireInstantHit));

	CPatch::InlineHook(g_libGTASA, 0x00336268, &CModelInfo_AddAtomicModel_hook, &CModelInfo_AddAtomicModel);

	CPatch::InlineHook(g_libGTASA, 0x00336618, &CModelInfo_AddVehicleModel_hook, &CModelInfo_AddVehicleModel); // dangerous

	CPatch::InlineHook(g_libGTASA, 0x0033DA5C, &CAnimManager__UncompressAnimation_hook, &CAnimManager__UncompressAnimation);
	CPatch::InlineHook(g_libGTASA, 0x001AECC0, &RwFrameAddChild_hook, &RwFrameAddChild);
	CPatch::InlineHook(g_libGTASA, 0x002DFD30, &CUpsideDownCarCheck__IsCarUpsideDown_hook, &CUpsideDownCarCheck__IsCarUpsideDown);
	CPatch::InlineHook(g_libGTASA, 0x0033AD78, &CAnimBlendNode__FindKeyFrame_hook, &CAnimBlendNode__FindKeyFrame);

	// TextDraw render
	//SetUpHook(g_libGTASA + 0x003D5894, (uintptr_t)CHud__DrawScriptText_hook, (uintptr_t*)& CHud__DrawScriptText);

	// attached objects
	CPatch::InlineHook(g_libGTASA, 0x003C1BF8, &CWorld_ProcessPedsAfterPreRender_hook, &CWorld_ProcessPedsAfterPreRender);

	//remove building
	//SetUpHook(g_libGTASA + 0x00395994, (uintptr_t)CFileLoader__LoadObjectInstance_hook, (uintptr_t*)& CFileLoader__LoadObjectInstance);

	// retexture
	//SetUpHook(g_libGTASA + 0x00391E20, (uintptr_t)CObject__Render_hook, (uintptr_t*)& CObject__Render);

	// gettexture fix crash
	CPatch::InlineHook(g_libGTASA, 0x00258910, &GetTexture_hook, &GetTexture_orig);

	// steal objects fix
	CPatch::InlineHook(g_libGTASA, 0x003AD8E0, &CPlayerInfo__Process_hook, &CPlayerInfo__Process);

	// GetFrameFromID fix
	CPatch::InlineHook(g_libGTASA, 0x00335CC0, &CClumpModelInfo_GetFrameFromId_hook, &CClumpModelInfo_GetFrameFromId);

	// random crash fix
	CPatch::InlineHook(g_libGTASA, 0x001A8530, &RenderQueue__ProcessCommand_hook, &RenderQueue__ProcessCommand);
	// fix
	CPatch::InlineHook(g_libGTASA, 0x001B9D74, &_rwFreeListFreeReal_hook, &_rwFreeListFreeReal);

	//SetUpHook(g_libGTASA + 0x0027548C, (DWORD)CWidgetButtonAttack_hook, (DWORD*)&CWidgetButtonAttack);
	CPatch::InlineHook(g_libGTASA, 0x00274AB4, &CWidgetButton__Update_hook, &CWidgetButton__Update);

	CPatch::SetUpHook(g_libGTASA + 0x00274748, (uintptr_t)CWidgetButton__Draw_hook, (uintptr_t*)&CWidgetButton__Draw);
	CPatch::InlineHook(g_libGTASA, 0x00274218, &CWidgetButton__IsTouched_hook, &CWidgetButton__IsTouched);
	//SetUpHook(g_libGTASA + 0x0027455C, (uintptr_t)CWidget__IsTouched_hook, (uintptr_t*)& CWidget__IsTouched);

	CPatch::RET(g_libGTASA + 0x002C0304); // CDarkel__RegisterCarBlownUpByPlayer_hook
	CPatch::RET(g_libGTASA + 0x002C072C); // CDarkel__ResetModelsKilledByPlayer_hook
	CPatch::RET(g_libGTASA + 0x002C0758); // CDarkel__QueryModelsKilledByPlayer_hook
	CPatch::RET(g_libGTASA + 0x002C0778); // CDarkel__FindTotalPedsKilledByPlayer_hook
	CPatch::RET(g_libGTASA + 0x002C0D04); // CDarkel__RegisterKillByPlayer_hook

	//CPatch::InlineHook(g_libGTASA, 0x00338CBC, &CVehicleModelInfo__SetEditableMaterials_hook, &CVehicleModelInfo__SetEditableMaterials);
	//CPatch::InlineHook(g_libGTASA, 0x0050DEF4, &CVehicle__ResetAfterRender_hook, &CVehicle__ResetAfterRender);

	CPatch::InlineHook(g_libGTASA, 0x3986CC, &CGame__Process_hook, &CGame__Process);

	CPatch::InlineHook(g_libGTASA, 0x004D4A6C, &CAutomobile__ProcessEntityCollision_hook, &CAutomobile__ProcessEntityCollision);

	CPatch::InlineHook(g_libGTASA, 0x00398334, &CGame__Shutdown_hook, &CGame__Shutdown);

	CPatch::WriteMemory(g_libGTASA + 0x003DA86C,
		"\x80\xB4"\
		"\x00\xAF"\
		"\x1B\x4B"\
		"\x7B\x44"\
		"\x1B\x68", 10); // CRadar::LimitRadarPoint

	CPatch::NOP(g_libGTASA + 0x003DA876, 3); // CRadar::LimitRadarPoint

	// headlights color, wheel size, wheel align
	CPatch::InlineHook(g_libGTASA, 0x005466EC, &CShadows__StoreCarLightShadow_hook, &CShadows__StoreCarLightShadow);
	CPatch::InlineHook(g_libGTASA, 0x00518EC4, &CVehicle__DoHeadLightBeam_hook, &CVehicle__DoHeadLightBeam);

	CPatch::InlineHook(g_libGTASA, 0x004E671C, &CAutomobile__PreRender_hook, &CAutomobile__PreRender);
	CPatch::InlineHook(g_libGTASA, 0x004DC6E8, &CAutomobile__UpdateWheelMatrix_hook, &CAutomobile__UpdateWheelMatrix);
	CPatch::InlineHook(g_libGTASA, 0x003E8D48, &CMatrix__Rotate_hook, &CMatrix__Rotate);
	CPatch::InlineHook(g_libGTASA, 0x003E8884, &CMatrix__SetScale_hook, &CMatrix__SetScale);

	CPatch::InlineHook(g_libGTASA, 0x0046CEF4, &CTaskSimpleUseGun__RemoveStanceAnims_hook, &CTaskSimpleUseGun__RemoveStanceAnims);

	CPatch::InlineHook(g_libGTASA, 0x003DA86C, &CRadar__LimitRadarPoint_hook, &CRadar__LimitRadarPoint); // TO FIX
	//CPatch::InlineHook(g_libGTASA, 0x005529FC, &CSprite2d__DrawBarChart_hook, &CSprite2d__DrawBarChart);
	//SetUpHook(g_libGTASA + 0x005353B4, (uintptr_t)CFont__PrintString_hook, (uintptr_t*)& CFont__PrintString);
	CPatch::InlineHook(g_libGTASA, 0x0055265C, &CSprite2d__Draw_hook, &CSprite2d__Draw);

	//SetUpHook(g_libGTASA + 0x0027D8A8, (uintptr_t)CWidgetPlayerInfo__DrawWanted_hook, (uintptr_t*)& CWidgetPlayerInfo__DrawWanted);

	//SetUpHook(g_libGTASA + 0x0027CE88, (uintptr_t)CWidgetPlayerInfo__DrawWeaponIcon_hook, (uintptr_t*)& CWidgetPlayerInfo__DrawWeaponIcon);
	CPatch::InlineHook(g_libGTASA, 0x00389D74, (uintptr_t)CCam__Process_hook, (uintptr_t*)& CCam__Process);

	CPatch::InlineHook(g_libGTASA, 0x003D6E6C, (uintptr_t)CHud__Draw_hook, (uintptr_t*)& CHud__Draw);
	CPatch::InlineHook(g_libGTASA, 0x0039DC68, (uintptr_t)CPad__CycleCameraModeDownJustDown_hook, (uintptr_t*)& CPad__CycleCameraModeDownJustDown);

	CPatch::InlineHook(g_libGTASA, 0x00391FE0, (uintptr_t)CEntity__GetIsOnScreen_hook, (uintptr_t*)& CEntity__GetIsOnScreen);
	CPatch::InlineHook(g_libGTASA, 0x0031B164, (uintptr_t)FxEmitterBP_c__Render_hook, (uintptr_t*)& FxEmitterBP_c__Render);
	CPatch::InlineHook(g_libGTASA, 0x0043A17C, (uintptr_t)CPed__ProcessEntityCollision_hook, (uintptr_t*)&CPed__ProcessEntityCollision);
	CPatch::InlineHook(g_libGTASA, 0x3AA3C0, (uintptr_t)CPhysical__Add_hook, (uintptr_t*)&CPhysical__Add);

	CPatch::InlineHook(g_libGTASA, 0x004B97FC, &CTaskSimpleGetUp__ProcessPed_hook, &CTaskSimpleGetUp__ProcessPed); // CTaskSimpleGetUp::ProcessPed

	//==================
	CPatch::InlineHook(g_libGTASA, 0x194B04, &emu_ArraysDelete_hook, &emu_ArraysDelete);
	CPatch::InlineHook(g_libGTASA, 0x1BA580, &RwResourcesFreeResEntry_hook, &RwResourcesFreeResEntry);
	CPatch::InlineHook(g_libGTASA, 0x194B20, &emu_ArraysGetID_hook, &emu_ArraysGetID);
	CPatch::InlineHook(g_libGTASA, 0x50C5F8, &SetCompAlphaCB_hook, &SetCompAlphaCB);

	//================================
	// ================== plaka ======
	//================================

	// vehicle light processing
	//CPatch::NOP(g_libGTASA + 0x005198DC, 4);
	CPatch::InlineHook(g_libGTASA, 0x5189C4, &CVehicle__GetVehicleLightsStatus_hook, &CVehicle__GetVehicleLightsStatus);
	//
	CPatch::NOP(g_libGTASA + 0x003989C8, 2);//живность в воде WaterCreatureManager_c::Update
	// дефолтный худ
	CPatch::NOP(g_libGTASA + 0x0027E21A, 2); // CWidgetPlayerInfo::DrawWeaponIcon
	CPatch::NOP(g_libGTASA + 0x0027E24E, 2); // CWidgetPlayerInfo::DrawWanted
	CPatch::NOP(g_libGTASA + 0x0027E1E8, 2); // CWidgetPlayerInfo::RenderBreathBar
	CPatch::NOP(g_libGTASA + 0x0027E1AE, 2); // CWidgetPlayerInfo::RenderArmorBar
	CPatch::NOP(g_libGTASA + 0x0027E188, 2); // CWidgetPlayerInfo::RenderHealthBar
	CPatch::NOP(g_libGTASA + 0x27E158, 2); // PrintMoney
	CPatch::NOP(g_libGTASA + 0x27E056, 2); // PrintTime

	//RpMaterialDestroy fix ? не точно
	CPatch::InlineHook(g_libGTASA, 0x001E3C54, &RpMaterialDestroy_hook, &RpMaterialDestroy);
	CPatch::InlineHook(g_libGTASA, 0x1B1808, &_RwTextureDestroy_hook, &_RwTextureDestroy);

	//CRunningScript fix ? не точно
	CPatch::InlineHook(g_libGTASA, 0x002E5400, &GivePedScriptedTask_hook, &GivePedScriptedTask);

	CPatch::InlineHook(g_libGTASA, 0x00308640, &ProcessCommands1400To1499_hook, &ProcessCommands1400To1499);
	CPatch::InlineHook(g_libGTASA, 0x002F7910, &ProcessCommands300To399_hook, &ProcessCommands300To399);

	//
	CPatch::NOP(g_libGTASA + 0x0039ADE6, 2);//CCoronas::RenderSunReflection crash

	CPatch::NOP(g_libGTASA+0x0051018A, 2);// не давать ган при выходе из тачки
	CPatch::NOP(g_libGTASA+0x005101A6, 2);// не давать ган при выходе из тачки

	// Настройки
	CPatch::NOP(g_libGTASA + 0x266460, 2); // Game - TrafficMode
	CPatch::NOP(g_libGTASA + 0x266496, 2); // Game - AimMode
	CPatch::NOP(g_libGTASA + 0x261A50, 2); // Language
	CPatch::NOP(g_libGTASA + 0x2661DA, 2); // Display - Shadows
	CPatch::NOP(g_libGTASA + 0x2665EE, 2); // Game - SocialClub
	//
	CPatch::InlineHook(g_libGTASA, 0x004904AC, &CTaskSimpleCarSetPedInAsDriver__ProcessPed_hook, &CTaskSimpleCarSetPedInAsDriver__ProcessPed);

	// passenger radar speed fix
	CPatch::InlineHook(g_libGTASA, 0x3AC45C, &FindPlayerSpeed_hook, &FindPlayerSpeed);

	// fix разрешения экрана
	CPatch::InlineHook(g_libGTASA, 0x0026CE30, &MobileSettings__GetMaxResWidth_hook, &MobileSettings__GetMaxResWidth);

	//
	//CPatch::InlineHook(g_libGTASA, 0x39AE28, &RenderEffects_hook, &RenderEffects);

	//размытие на скорости
	CPatch::InlineHook(g_libGTASA, 0x005311D0, &CDraw__SetFOV_hook, &CDraw__SetFOV);

	CPatch::InlineHook(g_libGTASA, 0x003B0E6C, &CEntity__RegisterReference_hook, &CEntity__RegisterReference);

	HookCPad();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_startPause(JNIEnv *env, jobject thiz) {
	CTimer__StartUserPause_hook();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_nvidia_devtech_NvEventQueueActivity_endPause(JNIEnv *env, jobject thiz) {
	CTimer__EndUserPause_hook();
}