#include "../main.h"
#include "../util/armhook.h"
#include "RW/RenderWare.h"
#include "game.h"
#include "../net/netgame.h"
#include "../gui/gui.h"
#include "santrope-tea-gtasa/encryption/CTinyEncrypt.h"
#include "santrope-tea-gtasa/encryption/encrypt.h"
#include "santrope-tea-gtasa/CGameResourcesDecryptor.h"
#include "../CAudioStream.h"
#include "../chatwindow.h"
#include "../util/patch.h"
#include "../gui/CHUD.h"
#include "crosshair.h"
#include "game.h"
#include "../util/CJavaWrapper.h"
#include "CSettings.h"

#include "../graphics/CRQ_Commands.h"
#include "../graphics/CSkyBox.h"

extern CChatWindow *pChatWindow;
extern CPlayerPed *g_pCurrentFiredPed;
extern BULLET_DATA *g_pCurrentBulletData;
extern CAudioStream *pAudioStream;
extern CCrossHair *pCrossHair;
extern CGame *pGame;
extern CSettings *pSettings;

extern "C"
{
#include "..//santrope-tea-gtasa/encryption/aes.h"
}



char (*CStreaming__ConvertBufferToObject)(int, int, int);
char CStreaming__ConvertBufferToObject_hook(int a1, int a2, int a3)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile("mov %0, lr"
	: "=r"(dwRetAddr));

	uint32_t tickStart = GetTickCount();
	CGameResourcesDecryptor::CStreaming__ConvertBufferToObject_hook((char*)a1, a2, a3);
	if (a2 >= 15000 && a2 <= 15100)
	{
		// pChatWindow->AddDebugMessage("loading time %d", GetTickCount() - tickStart);
	}
	char a12 = CStreaming__ConvertBufferToObject(a1, a2, a3);
	return a12;
}

int __attribute__((noinline)) g_unobfuscate(int a)
{
	return UNOBFUSCATE_DATA(a);
}
#include "..//str_obfuscator_no_template.hpp"
#define MAX_ENCRYPTED_TXD 2
const cryptor::string_encryptor encrArch[MAX_ENCRYPTED_TXD] = {cryptor::create("texdb/samp/samp.txt", 21), cryptor::create("texdb/gui/gui.txt", 19)};
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

stFile *(*NvFOpen)(const char *, const char *, int, int);
stFile *NvFOpen_hook(const char *r0, const char *r1, int r2, int r3)
{
	char path[0xFF] = {0};
	// ----------------------------
	if (!strncmp(r1 + 12, "mainV1.scm", 10))
	{
		sprintf(path, "%sSAMP/main.scm", g_pszStorage);
		Log("Loading mainV1.scm..");
		goto open;
	}
	// ----------------------------
	if (!strncmp(r1 + 12, "SCRIPTV1.IMG", 12))
	{
		sprintf(path, "%sSAMP/script.img", g_pszStorage);
		Log("Loading script.img..");
		goto open;
	}
	// ----------------------------
	if (!strncmp(r1, "DATA/PEDS.IDE", 13))
	{
		sprintf(path, "%s/SAMP/peds.ide", g_pszStorage);
		Log("Loading peds.ide..");
		goto open;
	}
	// ----------------------------
	if (!strncmp(r1, "DATA/VEHICLES.IDE", 17))
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
	stFile *st = (stFile *)malloc(8);
	st->isFileExist = false;

	FILE *f = fopen(path, "rb");
	if (f)
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
		return 0;
	}
}

/* ====================================================== */
void ShowHud()
{
	if (pGame)
	{
		if (pNetGame)
		{
			if (pGame->FindPlayerPed() || GamePool_FindPlayerPed())
			{
				CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
				if (pPlayerPool)
				{
					g_pJavaWrapper->UpdateHudInfo(pGame->FindPlayerPed()->GetHealth(),
												  pGame->FindPlayerPed()->GetArmour(),
												  -1, // hung
												  GamePool_FindPlayerPed()->WeaponSlots[GamePool_FindPlayerPed()->byteCurWeaponSlot].dwType,
												  GamePool_FindPlayerPed()->WeaponSlots[GamePool_FindPlayerPed()->byteCurWeaponSlot].dwAmmo,
												  GamePool_FindPlayerPed()->WeaponSlots[GamePool_FindPlayerPed()->byteCurWeaponSlot].dwAmmoInClip,
												  pGame->GetLocalMoney(), 
												  pGame->GetWantedLevel()
												);
				}
				if (pSettings && pSettings->GetReadOnly().iHud)
				{
					*(uint8_t *)(g_libGTASA + 0x7165E8) = 0;
				}
				else if (pSettings && !pSettings->GetReadOnly().iHud)
				{
					*(uint8_t *)(g_libGTASA + 0x7165E8) = 1;
				}
			}
		}
	}
}

bool bGameStarted = false;
uint32_t bProcessedRender2dstuff = 0;
uint32_t bProcessedCleanStreamPool = 0;
void (*Render2dStuff)();
void Render2dStuff_hook()
{
	bGameStarted = true;
	MAKE_PROFILE(test, test_time);
	// MainLoop();
	LOG_PROFILE(test, test_time);
	bProcessedRender2dstuff = GetTickCount();
	Render2dStuff();

	ShowHud();

	if (pNetGame)
	{
		CLocalPlayer *pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		if (pPlayer)
		{
			if (pPlayer->GetPlayerPed())
			{
				pNetGame->GetTextDrawPool()->Draw();
			}
		}
		if (pNetGame->GetPlayerPool())
		{
			if (pNetGame->GetPlayerPool()->GetLocalPlayer())
			{
				CPlayerPed *pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
				if (pPed)
				{
					CInfoBarText::Draw(pPed->GetHealth(), pPed->GetArmour());
				}
			}
		}
	}

	if (!bProcessedCleanStreamPool)
		bProcessedCleanStreamPool = GetTickCount();

	if (bGameStarted)
	{
		uintptr_t v1 = *(uintptr_t *)(g_libGTASA + 0x61B8C0); // TextureDatabaseRuntime::storedTexels
		bool v2 = v1 > 0x45FFFFF;
		if (v1 <= 0x45FFFFF)
			v2 = *(uintptr_t *)(g_libGTASA + 0x67067C) > 0x45FFFFFu; // CStreaming::ms_memoryUsed
		if (!v2)
			return;
		CPatch::CallFunction<void>(g_libGTASA + 0x293325); // CStreaming::RemoveAllUnusedModels
		return;
	}

	uint32_t result = GetTickCount() - bProcessedCleanStreamPool;
	if (result >= 5000)
	{
		bProcessedCleanStreamPool = GetTickCount();
		result = 0x5EFFFFF;
		uint32_t v5 = *(uintptr_t *)(g_libGTASA + 0x61B8C0); // TextureDatabaseRuntime::storedTexels
		bool v6 = v5 > 0x5EFFFFF;
		if (v5 <= 0x5EFFFFF)
			v6 = *(uint32_t *)(g_libGTASA + 0x67067C) > 0x5EFFFFFu; // CStreaming::ms_memoryUsed
		if (v6)
			return CPatch::CallFunction<void>(g_libGTASA + 0x293325); // CStreaming::RemoveAllUnusedModels
	}

	return;
}

void __fillArray()
{
}

/* ====================================================== */

void (*Render2dStuffAfterFade)();
void Render2dStuffAfterFade_hook()
{
	Render2dStuffAfterFade();
	if (pGUI && bGameStarted)
		pGUI->Render();
	return;
}

/* ====================================================== */

uint16_t gxt_string[0x7F];
uint16_t *(*CText_Get)(uintptr_t thiz, const char *text);
uint16_t *CText_Get_hook(uintptr_t thiz, const char *text)
{
	if (text[0] == 'S' && text[1] == 'A' && text[2] == 'M' && text[3] == 'P')
	{
		const char *code = &text[5];
		if (!strcmp(code, "MP"))
			CFont::AsciiToGxtChar("MultiPlayer", gxt_string);

		return gxt_string;
	}

	return CText_Get(thiz, text);
}

/* ====================================================== */

void MainMenu_OnStartSAMP()
{

	if (g_bPlaySAMP)
		return;

	InitInMenu();

	// StartGameScreen::OnNewGameCheck()
	((void (*)())(g_libGTASA + 0x261C8C + 1))();

	//*(uint32_t*)(g_libGTASA + 0x9E75B8) = 8;

	g_bPlaySAMP = true;
	return;
}

// OsArray<FlowScreen::MenuItem>::Add
void (*MenuItem_add)(int r0, uintptr_t r1);
void MenuItem_add_hook(int r0, uintptr_t r1)
{
	static bool bMenuInited = false;
	char *name = *(char **)(r1 + 4);

	if (!strcmp(name, "FEP_STG") && !bMenuInited)
	{
		Log("Creating \"MultiPlayer\" button.. (struct: 0x%X)", r1);
		// Nicaaai eiiieo "New Game"
		MenuItem_add(r0, r1);

		// eiiiea "MultiPlayer"
		*(char **)(r1 + 4) = "SAMP_MP";
		*(uintptr_t *)r1 = LoadTextureFromDB("samp", "menu_mainmp");
		*(uintptr_t *)(r1 + 8) = (uintptr_t)MainMenu_OnStartSAMP;

		bMenuInited = true;
		goto ret;
	}

	// Eaii?e?oai nicaaiea "Start Game" e "Stats" ec iai? iaocu
	if (g_bPlaySAMP && (!strcmp(name, "FEP_STG") ||
						!strcmp(name, "FEH_STA") ||
						!strcmp(name, "FEH_BRI")))
		return;

ret:
	return MenuItem_add(r0, r1);
}

/* ====================================================== */

// CGame::InitialiseRenderWare
void (*InitialiseRenderWare)();
void InitialiseRenderWare_hook()
{
	Log("Loading \"samp\" cd..");
	InitialiseRenderWare();
	// TextureDatabaseRuntime::Load()
	((void (*)(const char *, int, int))(g_libGTASA + 0x1BF244 + 1))("samp", 0, 5);
	//(( void (*)(const char*, int, int))(g_libGTASA+0x1BF244+1))("flame", 0, 5);
	//((void (*)(const char*, int, int))(g_libGTASA + 0x1BF244 + 1))("gui", 0, 5);

	return;
}

/* ====================================================== */

void RenderSplashScreen();
void (*CLoadingScreen_DisplayPCScreen)();
void CLoadingScreen_DisplayPCScreen_hook()
{
	RwCamera *camera = *(RwCamera **)(g_libGTASA + 0x95B064);

	if (RwCameraBeginUpdate(camera))
	{
		DefinedState2d();
		((void (*)())(g_libGTASA + 0x5519C8 + 1))(); // CSprite2d::InitPerFrame()
		RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void *)rwTEXTUREADDRESSCLAMP);
		((void (*)(bool))(g_libGTASA + 0x198010 + 1))(false); // emu_GammaSet()
		RenderSplashScreen();
		RwCameraEndUpdate(camera);
		RwCameraShowRaster(camera, 0, 0);
	}

	return;
}
int bBlockCWidgetRegionLookUpdate = 0;

/* ====================================================== */
#include "..//keyboard.h"
extern CKeyBoard *pKeyBoard;
void (*TouchEvent)(int, int, int posX, int posY);
void TouchEvent_hook(int type, int num, int posX, int posY)
{
	// Log("TOUCH EVENT HOOK");

	if (*(uint8_t *)(g_libGTASA + 0x8C9BA3) == 1)
	{
		return TouchEvent(type, num, posX, posY);
	}

	if (g_pWidgetManager)
	{
		g_pWidgetManager->OnTouchEventSingle(WIDGET_CHATHISTORY_UP, type, num, posX, posY);
		g_pWidgetManager->OnTouchEventSingle(WIDGET_CHATHISTORY_DOWN, type, num, posX, posY);
		//g_pWidgetManager->OnTouchEventSingle(WIDGET_CAMERA_CYCLE, type, num, posX, posY);
		//g_pWidgetManager->OnTouchEventSingle(WIDGET_MICROPHONE, type, num, posX, posY);
		g_pWidgetManager->OnTouchEventSingle(WIDGET_ENTEREXIT, type, num, posX, posY);
	}
	bool bRet = pGUI->OnTouchEvent(type, num, posX, posY);
	if (!bRet)
	{
		return;
	}

	ImGuiIO &io = ImGui::GetIO();

	if (pKeyBoard && pKeyBoard->IsOpen())
	{
		if (posX >= io.DisplaySize.x - ImGui::GetFontSize() * 4 && posY >= io.DisplaySize.y / 2 - (ImGui::GetFontSize() * 2.5) * 3 && posY <= io.DisplaySize.y / 2) // keys
		{
			return;
		}
	}

	if (bRet)
		return TouchEvent(type, num, posX, posY);
}

/* ====================================================== */

void (*CStreaming_InitImageList)();
void CStreaming_InitImageList_hook()
{
	char *ms_files = (char *)(g_libGTASA + 0x6702FC);
	ms_files[0] = 0;
	*(uint32_t *)&ms_files[44] = 0;
	ms_files[48] = 0;
	*(uint32_t *)&ms_files[92] = 0;
	ms_files[96] = 0;
	*(uint32_t *)&ms_files[140] = 0;
	ms_files[144] = 0;
	*(uint32_t *)&ms_files[188] = 0;
	ms_files[192] = 0;
	*(uint32_t *)&ms_files[236] = 0;
	ms_files[240] = 0;
	*(uint32_t *)&ms_files[284] = 0;
	ms_files[288] = 0;
	*(uint32_t *)&ms_files[332] = 0;
	ms_files[336] = 0;
	*(uint32_t *)&ms_files[380] = 0;

	((uint32_t(*)(char *, uint32_t))(g_libGTASA + 0x28E7B0 + 1))("TEXDB\\GTA3.IMG", 1);	   // CStreaming::AddImageToList
	((uint32_t(*)(char *, uint32_t))(g_libGTASA + 0x28E7B0 + 1))("TEXDB\\GTA_INT.IMG", 1); // CStreaming::AddImageToList
	((uint32_t(*)(char *, uint32_t))(g_libGTASA + 0x28E7B0 + 1))("TEXDB\\SAMP.IMG", 1);	   // CStreaming::AddImageToList
	((uint32_t(*)(char *, uint32_t))(g_libGTASA + 0x28E7B0 + 1))("TEXDB\\SAMPCOL.IMG", 1); // CStreaming::AddImageToList

	//((uint32_t(*)(char*, uint32_t))(g_libGTASA + 0x28E7B0 + 1))("TEXDB\\ARH1.IMG", 1); // CStreaming::AddImageToList
	//((uint32_t(*)(char*, uint32_t))(g_libGTASA + 0x28E7B0 + 1))("TEXDB\\ARH2.IMG", 1); // CStreaming::AddImageToList
	//((uint32_t(*)(char*, uint32_t))(g_libGTASA + 0x28E7B0 + 1))("TEXDB\\ARH3.IMG", 1); // CStreaming::AddImageToList
}

/* ====================================================== */
typedef struct _PED_MODEL
{
	uintptr_t vtable;
	uint8_t data[88];
} PED_MODEL; // SIZE = 92

PED_MODEL PedsModels[370];
int PedsModelsCount = 0;

PED_MODEL *(*CModelInfo_AddPedModel)(int id);
PED_MODEL *CModelInfo_AddPedModel_hook(int id)
{
	PED_MODEL *model = &PedsModels[PedsModelsCount];
	memset(model, 0, sizeof(PED_MODEL)); // initialize by zero

	((void (*)(void *thiz))(g_libGTASA + 0x33559C + 1))((void *)model); // CBaseModelInfo::CBaseModelInfo();

	model->vtable = (uintptr_t)(g_libGTASA + 0x5C6E90); // assign CPedModelInfo vmt

	((uintptr_t(*)(PED_MODEL *))(*(void **)(model->vtable + 0x1C)))(model); // CClumpModelInfo::Init()

	*(PED_MODEL **)(g_libGTASA + 0x87BF48 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

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
	if (pNetGame && pNetGame->GetPickupPool())
	{
		CPickupPool *pPickups = pNetGame->GetPickupPool();
		pPickups->PickedUp(((dwParam1 - (g_libGTASA + 0x70E264)) / 0x20));
	}
}

__attribute__((naked)) void PickupPickUp_hook()
{
	// LOGI("PickupPickUp_hook");

	// calculate and save ret address
	__asm__ volatile("push {lr}\n\t"
					 "push {r0}\n\t"
					 "blx get_lib\n\t"
					 "add r0, #0x2D0000\n\t"
					 "add r0, #0x9A00\n\t"
					 "add r0, #1\n\t"
					 "mov r1, r0\n\t"
					 "pop {r0}\n\t"
					 "pop {lr}\n\t"
					 "push {r1}\n\t");

	//
	__asm__ volatile("push {r0-r11, lr}\n\t"
					 "mov %0, r4"
					 : "=r"(dwParam1));

	__asm__ volatile("blx pickup_ololo\n\t");

	__asm__ volatile("pop {r0-r11, lr}\n\t");

	// restore
	__asm__ volatile("ldrb r1, [r4, #0x1C]\n\t"
					 "sub.w r2, r1, #0xD\n\t"
					 "sub.w r2, r1, #8\n\t"
					 "cmp r1, #6\n\t"
					 "pop {pc}\n\t");
}

extern "C" bool NotifyEnterVehicle(VEHICLE_TYPE *_pVehicle)
{
	// Log("NotifyEnterVehicle");

	if (!pNetGame)
		return false;

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CVehicle *pVehicle;
	VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(_pVehicle);

	if (VehicleID == INVALID_VEHICLE_ID)
		return false;
	if (!pVehiclePool->GetSlotState(VehicleID))
		return false;
	pVehicle = pVehiclePool->GetAt(VehicleID);
	if (pVehicle->m_pVehicle->entity.nModelIndex == TRAIN_PASSENGER)
		return false;

	if (pVehicle->m_pVehicle->pDriver &&
		pVehicle->m_pVehicle->pDriver->dwPedType != 0)
		return false;

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	// if(pLocalPlayer->GetPlayerPed() && pLocalPlayer->GetPlayerPed()->GetCurrentWeapon() == WEAPON_PARACHUTE)
	//   pLocalPlayer->GetPlayerPed()->SetArmedWeapon(0);

	pLocalPlayer->SendEnterVehicleNotification(VehicleID, false);

	return true;
}

void (*CTaskComplexEnterCarAsDriver)(uint32_t thiz, uint32_t pVehicle);
extern "C" void call_taskEnterCarAsDriver(uintptr_t a, uint32_t b)
{
	CTaskComplexEnterCarAsDriver(a, b);
}
void __attribute__((naked)) CTaskComplexEnterCarAsDriver_hook(uint32_t thiz, uint32_t pVehicle)
{
	__asm__ volatile("push {r0-r11, lr}\n\t"
					 "mov r2, lr\n\t"
					 "blx get_lib\n\t"
					 "add r0, #0x3A0000\n\t"
					 "add r0, #0xEE00\n\t"
					 "add r0, #0xF7\n\t"
					 "cmp r2, r0\n\t"
					 "bne 1f\n\t" // !=
					 "mov r0, r1\n\t"
					 "blx NotifyEnterVehicle\n\t" // call NotifyEnterVehicle
					 "1:\n\t"					  // call orig
					 "pop {r0-r11, lr}\n\t"
					 "push {r0-r11, lr}\n\t"
					 "blx call_taskEnterCarAsDriver\n\t"
					 "pop {r0-r11, pc}");
}

void (*CTaskComplexLeaveCar)(uintptr_t **thiz, VEHICLE_TYPE *pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut);
void CTaskComplexLeaveCar_hook(uintptr_t **thiz, VEHICLE_TYPE *pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile("mov %0, lr"
					 : "=r"(dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x3AE905 || dwRetAddr == 0x3AE9CF)
	{
		if (pNetGame)
		{
			if (GamePool_FindPlayerPed()->pVehicle == (uint32_t)pVehicle)
			{
				CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
				VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE *)GamePool_FindPlayerPed()->pVehicle);
				CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
				pLocalPlayer->SendExitVehicleNotification(VehicleID);
			}
		}
	}

	(*CTaskComplexLeaveCar)(thiz, pVehicle, iTargetDoor, iDelayTime, bSensibleLeaveCar, bForceGetOut);
}

unsigned int (*MainMenuScreen__Update)(uintptr_t thiz, float a2);
unsigned int MainMenuScreen__Update_hook(uintptr_t thiz, float a2)
{
	unsigned int ret = MainMenuScreen__Update(thiz, a2);
	// g_pJavaWrapper->HideHud();
	MainMenu_OnStartSAMP();
	return ret;
}
void InitCTX(AES_ctx& ctx, const uint8_t* pKey);
extern signed int (*OS_FileOpen)(unsigned int a1, int *a2, const char *a3, int a4);
signed int OS_FileOpen_hook(unsigned int a1, int *a2, const char *a3, int a4);
static uint32_t dwRLEDecompressSourceSize = 0;

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
	if (calledFrom == 0x1BDD34 + 1)
	{
		size_t retn = OS_FileRead(a1, a2, a3);

		dwRLEDecompressSourceSize = *(uint32_t *)a2;

		return retn;
	}
	else
	{
		size_t retn = OS_FileRead(a1, a2, a3);

		return retn;
	}
}

void RedirectCall(uintptr_t addr, uintptr_t func);

static char szLastBufferedName[40];
int (*cHandlingDataMgr__FindExactWord)(uintptr_t thiz, char *line, char *nameTable, int entrySize, int entryCount);
int cHandlingDataMgr__FindExactWord_hook(uintptr_t thiz, char *line, char *nameTable, int entrySize, int entryCount)
{
	strncpy(&szLastBufferedName[0], line, entrySize);
	return cHandlingDataMgr__FindExactWord(thiz, line, nameTable, entrySize, entryCount);
}

void (*cHandlingDataMgr__ConvertDataToGameUnits)(uintptr_t thiz, tHandlingData *handling);
void cHandlingDataMgr__ConvertDataToGameUnits_hook(uintptr_t thiz, tHandlingData *handling)
{
	int iHandling = ((int (*)(uintptr_t, char *))(g_libGTASA + 0x4FBC4C + 1))(thiz, &szLastBufferedName[0]);

	CHandlingDefault::FillDefaultHandling((uint16_t)iHandling, handling);

	return cHandlingDataMgr__ConvertDataToGameUnits(thiz, handling);
}

#include "..//nv_event.h"
int32_t (*NVEventGetNextEvent_hooked)(NVEvent *ev, int waitMSecs);
int32_t NVEventGetNextEvent_hook(NVEvent *ev, int waitMSecs)
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
				((void (*)(int, int, int posX, int posY))(g_libGTASA + 0x239D5C + 1))(type, 0, x1, y1); // AND_TouchEvent
			}
			else
			{
				((void (*)(int, int, int posX, int posY))(g_libGTASA + 0x239D5C + 1))(NV_MULTITOUCH_MOVE, 0, x1, y1); // AND_TouchEvent
			}
		}

		if ((x2 || y2) || num == 1)
		{
			if (num == 1 && type != NV_MULTITOUCH_MOVE)
			{
				((void (*)(int, int, int posX, int posY))(g_libGTASA + 0x239D5C + 1))(type, 1, x2, y2); // AND_TouchEvent
			}
			else
			{
				((void (*)(int, int, int posX, int posY))(g_libGTASA + 0x239D5C + 1))(NV_MULTITOUCH_MOVE, 1, x2, y2); // AND_TouchEvent
			}
		}
		if ((x3 || y3) || num == 2)
		{
			if (num == 2 && type != NV_MULTITOUCH_MOVE)
			{
				((void (*)(int, int, int posX, int posY))(g_libGTASA + 0x239D5C + 1))(type, 2, x3, y3); // AND_TouchEvent
			}
			else
			{
				((void (*)(int, int, int posX, int posY))(g_libGTASA + 0x239D5C + 1))(NV_MULTITOUCH_MOVE, 2, x3, y3); // AND_TouchEvent
			}
		}
	}

	return ret;
}

void (*CStreaming__Init2)();
void CStreaming__Init2_hook()
{
	CStreaming__Init2();
	*(uint32_t *)(g_libGTASA + 0x5DE734) = 536870912;
}

void (*NvUtilInit)(void);
void NvUtilInit_hook(void)
{
	NvUtilInit();
	CPatch::UnFuck(g_libGTASA + 0x5D1608);
	*(char **)(g_libGTASA + 0x5D1608) = "/storage/emulated/0/Android/data/com.liverussia.cr/files/";
}

void SetUpGLHooks();
void InstallSpecialHooks()
{
	// SetUpGLHooks();

	// OS_FileRead
	CPatch::WriteMemory(g_libGTASA + 0x23BEDC, "\xF8\xB5", 2);
	CPatch::WriteMemory(g_libGTASA + 0x23BEDE, "\x00\x46\x00\x46", 4);

    CPatch::InlineHook(g_libGTASA, 0x291104, &CStreaming__ConvertBufferToObject_hook, &CStreaming__ConvertBufferToObject);
	CPatch::InlineHook(g_libGTASA, 0x241D94, &NvUtilInit_hook, &NvUtilInit);

	CPatch::InlineHook(g_libGTASA, 0x4FBBB0, &cHandlingDataMgr__FindExactWord_hook, &cHandlingDataMgr__FindExactWord);
	CPatch::InlineHook(g_libGTASA, 0x4FBCF4, &cHandlingDataMgr__ConvertDataToGameUnits_hook, &cHandlingDataMgr__ConvertDataToGameUnits);
	CPatch::InlineHook(g_libGTASA, 0x23ACC4, &NVEventGetNextEvent_hook, &NVEventGetNextEvent_hooked);
	CPatch::InlineHook(g_libGTASA, 0x4042A8, &CStreaming__Init2_hook, &CStreaming__Init2); // increase stream memory value
	CPatch::InlineHook(g_libGTASA, 0x269974, &MenuItem_add_hook, &MenuItem_add);
	CPatch::InlineHook(g_libGTASA, 0x4D3864, &CText_Get_hook, &CText_Get);
	CPatch::InlineHook(g_libGTASA, 0x40C530, &InitialiseRenderWare_hook, &InitialiseRenderWare);
	CPatch::InlineHook(g_libGTASA, 0x23BEDC, &OS_FileRead_hook, &OS_FileRead);
	CPatch::InlineHook(g_libGTASA, 0x23B3DC, &NvFOpen_hook, &NvFOpen);
	CPatch::InlineHook(g_libGTASA, 0x25E660, &MainMenuScreen__Update_hook, &MainMenuScreen__Update);
	CPatch::InlineHook(g_libGTASA, 0x23BB84, &OS_FileOpen_hook, &OS_FileOpen);

	// -- Huawei (Y7) crash fix
	if (!*(uintptr_t *)(SA_ADDR(0x61B298)))
		*(uintptr_t *)(SA_ADDR(0x61B298)) = ((uintptr_t(*)(const char *))(SA_ADDR(0x179A20)))("glAlphaFuncQCOM");

	if (!*(uintptr_t *)(SA_ADDR(0x61B298)))
		*(uintptr_t *)(SA_ADDR(0x61B298)) = ((uintptr_t(*)(const char *))(SA_ADDR(0x179A20)))("glAlphaFunc");

	if (!*(uintptr_t *)(SA_ADDR(0x61B298)))
		Log("CRASH IS INEVITABLE!!!");
	// ---

	CPatch::installJMPHook(SA_ADDR(0x1A1ED8), (uintptr_t)CRQ_Commands::rqVertexBufferSelect_HOOK);
}

int (*RwFrameAddChild)(int, int);
int RwFrameAddChild_hook(int a1, int a2)
{
	if (a2 && a1)
	{
		return RwFrameAddChild(a1, a2);
	}
	else
		return 0;
}

void (*CAnimManager__UncompressAnimation)(int, int);
void CAnimManager__UncompressAnimation_hook(int a1, int a2)
{
	if (a1)
	{
		CAnimManager__UncompressAnimation(a1, a2);
	}
}

void (*CCustomRoadsignMgr__RenderRoadsignAtomic)(int, int);
void CCustomRoadsignMgr__RenderRoadsignAtomic_hook(int a1, int a2)
{
	if (a1)
	{
		CCustomRoadsignMgr__RenderRoadsignAtomic(a1, a2);
	}
}

int (*CUpsideDownCarCheck__IsCarUpsideDown)(int, int);
int CUpsideDownCarCheck__IsCarUpsideDown_hook(int a1, int a2)
{
	if (*(uintptr_t *)(a2 + 20))
	{
		return CUpsideDownCarCheck__IsCarUpsideDown(a1, a2);
	}
	return 0;
}

int (*CAnimBlendNode__FindKeyFrame)(int, float, int, int);
int CAnimBlendNode__FindKeyFrame_hook(int a1, float a2, int a3, int a4)
{
	if (*(uintptr_t *)(a1 + 16))
	{
		return CAnimBlendNode__FindKeyFrame(a1, a2, a3, a4);
	}
	else
		return 0;
}

/* ====================================================== */

typedef struct _ATOMIC_MODEL
{
	uintptr_t vtable;
	uint8_t data[52];
} ATOMIC_MODEL; // SIZE = 56

ATOMIC_MODEL AtomicModels[16000];
int AtomicModelsCount = 0;

ATOMIC_MODEL *(*CModelInfo_AddAtomicModel)(int id);
ATOMIC_MODEL *CModelInfo_AddAtomicModel_hook(int id)
{
	ATOMIC_MODEL *model = &AtomicModels[AtomicModelsCount];
	memset(model, 0, sizeof(ATOMIC_MODEL));

	((void (*)(void *thiz))(g_libGTASA + 0x33559C + 1))((void *)model); // CBaseModelInfo::CBaseModelInfo();

	model->vtable = (uintptr_t)(g_libGTASA + 0x5C6C68); // assign CAtomicModelInfo vmt

	((uintptr_t(*)(ATOMIC_MODEL *))(*(void **)(model->vtable + 0x1C)))(model); // CClumpModelInfo::Init()

	*(ATOMIC_MODEL **)(g_libGTASA + 0x87BF48 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

	AtomicModelsCount++;
	return model;
}

/* ====================================================== */
typedef struct _VEHICLE_MODEL
{
	uintptr_t vtable;
	uint8_t data[932];
} VEHICLE_MODEL; // SIZE = 936

VEHICLE_MODEL VehicleModels[370];
int VehicleModelsCount = 0;

VEHICLE_MODEL *(*CModelInfo_AddVehicleModel)(int id);
VEHICLE_MODEL *CModelInfo_AddVehicleModel_hook(int id)
{
	VEHICLE_MODEL *model = &VehicleModels[VehicleModelsCount];
	memset(model, 0, sizeof(VEHICLE_MODEL));

	((void (*)(void *thiz))(g_libGTASA + 0x337AA0 + 1))((void *)model); // CVehicleModelInfo::CVehicleModelInfo();

	model->vtable = (uintptr_t)(g_libGTASA + 0x5C6EE0); // assign CVehicleModelInfo vmt

	((uintptr_t(*)(VEHICLE_MODEL *))(*(void **)(model->vtable + 0x1C)))(model); // CVehicleModelInfo::Init()

	*(VEHICLE_MODEL **)(g_libGTASA + 0x87BF48 + (id * 4)) = model; // CModelInfo::ms_modelInfoPtrs

	VehicleModelsCount++;
	return model;
}
/* ====================================================== */

extern CGame *pGame;
#include "..//CSettings.h"
extern CSettings *pSettings;
void (*CHud__DrawScriptText)(uintptr_t, uint8_t);

void CHud__DrawScriptText_hook(uintptr_t thiz, uint8_t unk)
{
	CHud__DrawScriptText(thiz, unk);
	if (pGame && pNetGame)
	{

		if (g_pWidgetManager)
		{
			ImGuiIO &io = ImGui::GetIO();

			if (!g_pWidgetManager->GetSlotState(WIDGET_CHATHISTORY_UP))
			{
				g_pWidgetManager->New(WIDGET_CHATHISTORY_UP, 1700.0f, (io.DisplaySize.y * 0.3) - 180.0f, 130.0f, 130.0f, "menu_up");
				g_pWidgetManager->GetWidget(WIDGET_CHATHISTORY_UP)->SetPosWithoutScale(pGUI->ScaleX(1325.0f), io.DisplaySize.y * 0.3);
			}
			if (!g_pWidgetManager->GetSlotState(WIDGET_CHATHISTORY_DOWN))
			{
				g_pWidgetManager->New(WIDGET_CHATHISTORY_DOWN, 1700.0f, (io.DisplaySize.y * 0.3) - 30, 130.0f, 130.0f, "menu_down");
				g_pWidgetManager->GetWidget(WIDGET_CHATHISTORY_DOWN)->SetPosWithoutScale(pGUI->ScaleX(1515.0f), io.DisplaySize.y * 0.3);
			}

			//if (!g_pWidgetManager->GetSlotState(WIDGET_MICROPHONE))
			//{
			//	g_pWidgetManager->New(WIDGET_MICROPHONE,
			//						  pSettings->GetReadOnly().fButtonMicrophoneX,
			//						  pSettings->GetReadOnly().fButtonMicrophoneY,
			//						  pSettings->GetReadOnly().fButtonMicrophoneSize,
			//						  pSettings->GetReadOnly().fButtonMicrophoneSize, "hud_voice");
			//}

			if (!g_pWidgetManager->GetSlotState(WIDGET_ENTEREXIT))
			{
				g_pWidgetManager->New(WIDGET_ENTEREXIT,
									  pSettings->GetReadOnly().fButtonEnterPassengerX,
									  pSettings->GetReadOnly().fButtonEnterPassengerY,
									  pSettings->GetReadOnly().fButtonEnterPassengerSize,
									  pSettings->GetReadOnly().fButtonEnterPassengerSize, "enterandexit");
			}

			//if (!g_pWidgetManager->GetSlotState(WIDGET_CAMERA_CYCLE))
			//{
			//	g_pWidgetManager->New(WIDGET_CAMERA_CYCLE,
			//						  pSettings->GetReadOnly().fButtonCameraCycleX,
			//						  pSettings->GetReadOnly().fButtonCameraCycleY,
			//						  pSettings->GetReadOnly().fButtonCameraCycleSize,
			//						  pSettings->GetReadOnly().fButtonCameraCycleSize, "cam-toggle");
			//}
		}
	}
}

#include "..//keyboard.h"
// extern CJavaWrapper *pJavaWrapper;
extern CKeyBoard *pKeyBoard;
int (*CWidgetButtonEnterCar_Draw)(uintptr_t);
int CWidgetButtonEnterCar_Draw_hook(uintptr_t thiz)
{
	if (g_pWidgetManager)
	{
		CWidget *pWidget = g_pWidgetManager->GetWidget(WIDGET_CHATHISTORY_UP);
		if (pWidget)
		{
			pWidget->SetDrawState(false);
		}
		pWidget = g_pWidgetManager->GetWidget(WIDGET_CHATHISTORY_DOWN);
		if (pWidget)
		{
			pWidget->SetDrawState(false);
		}

		//pWidget = g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE);
		//if (pWidget)
		//{
		//	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
		//	if (pPlayerPool)
		//	{
		//		CLocalPlayer *pLocalPlayer;
		//		if (pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle())
		//		{
		//			pWidget->SetDrawState(true);
		//		}
		//		else
		//		{
		//			pWidget->SetDrawState(false);
		//		}
		//	}
		//	else
		//	{
		//		pWidget->SetDrawState(false);
		//	}
		//}

		//pWidget = g_pWidgetManager->GetWidget(WIDGET_MICROPHONE);
		//if (pWidget)
		//{
		//	pWidget->SetDrawState(true);
		//}

		pWidget = g_pWidgetManager->GetWidget(WIDGET_ENTEREXIT);
		if (pWidget)
		{
			CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
			if (pVehiclePool)
			{
				VEHICLEID ClosetVehicleID = pVehiclePool->FindNearestToLocalPlayerPed();
				if (ClosetVehicleID < MAX_VEHICLES && pVehiclePool->GetSlotState(ClosetVehicleID))
				{
					CVehicle *pVehicle = pVehiclePool->GetAt(ClosetVehicleID);
					if (pVehicle)
					{
						if (pVehicle->GetDistanceFromLocalPlayerPed() < 4.0f)
						{
							CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
							if (pPlayerPool)
							{
								CLocalPlayer *pLocalPlayer;
								if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
								{
									pWidget->SetDrawState(true);
								}
								else
								{
									pWidget->SetDrawState(false);
								}
							}
							else
							{
								pWidget->SetDrawState(false);
							}
						}
						else
						{
							pWidget->SetDrawState(false);
						}
					}
					else
					{
						pWidget->SetDrawState(false);
					}
				}
				else
				{
					pWidget->SetDrawState(false);
				}
			}
			else
			{
				pWidget->SetDrawState(false);
			}
		}

		if (!pGame->IsToggledHUDElement(HUD_ELEMENT_BUTTONS))
		{
			for (int i = 0; i < MAX_WIDGETS; i++)
			{
				CWidget *pWidget = g_pWidgetManager->GetWidget(i);
				if (pWidget)
				{
					pWidget->SetDrawState(false);
				}
			}
		}

		g_pWidgetManager->Draw();
	}
	return CWidgetButtonEnterCar_Draw(thiz);
}

uint64_t (*CWorld_ProcessPedsAfterPreRender)();
uint64_t CWorld_ProcessPedsAfterPreRender_hook()
{
	uint64_t res = CWorld_ProcessPedsAfterPreRender();
	if (pNetGame && pNetGame->GetPlayerPool())
	{
		for (int i = 0; i < MAX_PLAYERS; i++)
		{
			CPlayerPed *pPed = nullptr;
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
			if (!pPed)
				continue;
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
#include "..//santrope-tea-gtasa/CGameResourcesDecryptor.h"

int (*CFileLoader__LoadObjectInstance)(uintptr_t, uintptr_t);
int CFileLoader__LoadObjectInstance_hook(uintptr_t thiz, uintptr_t name)
{
	for (int i = 0; i < iTotalRemovedObjects; i++)
	{
		if (RemoveModelIDs[i] == *(uint32_t *)(thiz + 28))
		{
			VECTOR pos;
			pos.X = *(float *)(thiz);
			pos.Y = *(float *)(thiz + 4);
			pos.Z = *(float *)(thiz + 8);
			if (GetDistanceBetween3DPoints(&pos, &RemovePos[i]) <= RemoveRad[i])
			{
				*(int *)(thiz + 28) = 19300;
			}
		}
	}
	return CFileLoader__LoadObjectInstance(thiz, name);
}

#include <list>

std::list<std::pair<unsigned int *, unsigned int>> resetEntries;

static uint32_t Color32Reverse(uint32_t x)
{
	return ((x & 0xFF000000) >> 24) |
		   ((x & 0xFF0000) >> 8) |
		   ((x & 0x00FF00) << 8) |
		   ((x & 0x0000FF) << 24);
}

static RwRGBA DWORD2RGBAinternal(uint32_t dwColor)
{
	RwRGBA tmp;

	tmp.blue = dwColor & 0xFF;
	dwColor >>= 8;
	tmp.green = dwColor & 0xFF;
	dwColor >>= 8;
	tmp.red = dwColor & 0xFF;
	dwColor >>= 8;
	tmp.alpha = dwColor & 0xFF; /* dwColor >>= 8; */

	return tmp;
}

RpAtomic *ObjectMaterialCallBack(RpAtomic *rpAtomic, CObject *pObject)
{
	if (!pObject->m_bMaterials || rpAtomic->object.object.type != 1)
		return rpAtomic;

	int iTotalEntries = rpAtomic->geometry->matList.numMaterials;
	if (iTotalEntries >= 16)
		iTotalEntries = 16; // fix fucking bug :|
	for (int i = 0; i < iTotalEntries; i++)
	{
		if (pObject->m_pMaterials[i].m_bCreated && pObject->m_pMaterials[i].pTex)
		{
			rpAtomic->geometry->matList.materials[i]->texture = pObject->m_pMaterials[i].pTex;

			if (pObject->m_pMaterials[i].dwColor)
			{
				//((int(*)())(g_libGTASA + 0x559FC8 + 1))();
				rpAtomic->geometry->flags = rpAtomic->geometry->flags & 0xFFFFFFB7 | 0x40;
				RwRGBA *r = (RwRGBA *)&pObject->m_pMaterials[i].dwColor;
				// r.alpha = 0;

				rpAtomic->geometry->matList.materials[i]->color = *r;
				rpAtomic->geometry->matList.materials[i]->surfaceProps.ambient = 1.0f;
				rpAtomic->geometry->matList.materials[i]->surfaceProps.specular = 0.0f;
				rpAtomic->geometry->matList.materials[i]->surfaceProps.diffuse = 1.0f;
			}
		}
	}

	return rpAtomic;
}

RpLight *LightCallBack(RpLight *rpLight, void *p)
{
	rpLight->color.alpha = 0;
	return rpLight;
}

RpAtomic *ObjectMaterialTextCallBack(RpAtomic *rpAtomic, CObject *pObject)
{

	if (!pObject->m_MaterialTextTexture || rpAtomic->object.object.type != 1)
		return rpAtomic;

	int iTotalEntries = rpAtomic->geometry->matList.numMaterials;
	if (iTotalEntries >= 16)
		iTotalEntries = 16; // fix fucking bug :|
	for (int i = 0; i < iTotalEntries; i++)
	{

		if (pObject->m_MaterialTextTexture[i].m_bCreated && pObject->m_MaterialTextTexture[i].pTex)
		{
			rpAtomic->geometry->matList.materials[i]->texture = pObject->m_MaterialTextTexture[i].pTex;
		}
	}

	return rpAtomic;
}

int g_iLastRenderedObject;
void (*CObject__Render)(ENTITY_TYPE *);
void CObject__Render_hook(ENTITY_TYPE *thiz)
{
	// 0051ABB0 + 1
	// 004353FE + 1
	// 004352C4 + 1

	if (CSkyBox::GetSkyObject())
	{
		if (CSkyBox::GetSkyObject()->m_pEntity == thiz && !CSkyBox::IsNeedRender())
			return;
	}

	uintptr_t dwRetAddr = 0;
	__asm__ volatile("mov %0, lr"
					 : "=r"(dwRetAddr));
	dwRetAddr -= g_libGTASA;

	/*if (dwRetAddr == 0x51ABB0 + 1 || dwRetAddr == 0x4353FE + 1 || dwRetAddr == 0x4352C4 + 1)
	{
		return CObject__Render(thiz);
	}*/

	uintptr_t pAtomic = thiz->m_RwObject;
	if (!pAtomic)
	{
		return CObject__Render(thiz); // CObject::Render(CObject*)
	}
	if (!*(uintptr_t *)(pAtomic + 4))
	{
		return CObject__Render(thiz); // CObject::Render(CObject*)
	}

	if (pNetGame)
	{
		CObjectPool *pObjectPool = pNetGame->GetObjectPool();
		if (pObjectPool)
		{
			CObject *pObject = pObjectPool->GetObjectFromGtaPtr(thiz);
			if (pObject)
			{
				if (pObject->m_pEntity)
				{
					g_iLastRenderedObject = pObject->m_pEntity->nModelIndex;
				}

				((void (*)())(g_libGTASA + 0x559EF8 + 1))(); // DeActivateDirectional

				if (pObject->m_bMaterials)
				{
					// thiz->m_bLightObject = 0;

					((uintptr_t(*)(uintptr_t, uintptr_t, uintptr_t))(g_libGTASA + 0x1AEE2C + 1))(*(uintptr_t *)(pAtomic + 4), (uintptr_t)ObjectMaterialCallBack, (uintptr_t)pObject); // RwFrameForAllObjects
																																													  //((void(*)(ENTITY_TYPE*, bool))(g_libGTASA + 0x3B1F1C + 1))(thiz, 1); // CObject::RemoveLighting(CObject*, bool)
																																													  //((void(*)())(g_libGTASA + 0x559EF8 + 1))(); // DeActivateDirectional
				}

				if (pObject->m_bMaterialsText)
				{
					RwFrameForAllObjects((RwFrame *)*(uintptr_t *)(pAtomic + 4), (RwObject * (*)(RwObject *, void *)) ObjectMaterialTextCallBack, pObject);
				}
			}
		}
	}
	CObject__Render(thiz); // CObject::Render(CObject*)
}

void (*CRenderer__RenderEverythingBarRoads)();
void CRenderer__RenderEverythingBarRoads_hook()
{
	CSkyBox::Process();

	CRenderer__RenderEverythingBarRoads();
}

#pragma optimize("", off)

bool isEncrypted(const char *szArch)
{
	return false;
	for (int i = 0; i < MAX_ENCRYPTED_TXD; i++)
	{
		if (!strcmp(encrArch[i].decrypt(), szArch))
			return true;
	}

	return false;
}

signed int (*OS_FileOpen)(unsigned int a1, int *a2, const char *a3, int a4);
signed int OS_FileOpen_hook(unsigned int a1, int *a2, const char *a3, int a4)
{
	uintptr_t calledFrom = 0;
	__asm__ volatile("mov %0, lr"
					 : "=r"(calledFrom));
	calledFrom -= g_libGTASA;
	signed int retn = OS_FileOpen(a1, a2, a3, a4);

	if (calledFrom == 0x1BCE9A + 1)
	{
		if (isEncrypted(a3))
		{
			lastOpenedFile = *a2;
		}
	}
	return retn;
}

// CGameIntegrity
// CodeObfuscation





#include <sstream>
char (*CFileMgr__ReadLine)(int a1, char*, int);
char CFileMgr__ReadLine_hook(int a1, char *a2, int a3)
{
	char retv = CFileMgr__ReadLine(a1, a2, a3);
	char *pStr = a2;
	int value = *(int *)pStr;
	Log("%d, %d, %d, %d, %d", value, g_unobfuscate(g_iIdentifierVersion2), UNOBFUSCATE_DATA(value), OBFUSCATE_DATA(value) );
	Log("%c %c", a2[0], a2[1]);
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

uintptr_t (*GetTexture_orig)(const char *);
uintptr_t GetTexture_hook(const char *a1)
{
	// 001BE990
	uintptr_t tex = ((uintptr_t(*)(const char *))(g_libGTASA + 0x1BE990 + 1))(a1);
	if (!tex)
	{
		// Log("Texture %s was not found", a1);
		return 0;
	}
	else
	{
		++*(uintptr_t *)(tex + 84);
		return tex;
	}
}

uintptr_t (*CPlayerInfo__FindObjectToSteal)(uintptr_t, uintptr_t);
uintptr_t CPlayerInfo__FindObjectToSteal_hook(uintptr_t a1, uintptr_t a2)
{
	return 0;
}
RwFrame *CClumpModelInfo_GetFrameFromId_Post(RwFrame *pFrameResult, RpClump *pClump, int id)
{
	if (pFrameResult)
		return pFrameResult;

	uintptr_t calledFrom = 0;
	__asm__ volatile("mov %0, lr"
					 : "=r"(calledFrom));
	calledFrom -= g_libGTASA;

	if (calledFrom == 0x515708	   // CVehicle::SetWindowOpenFlag
		|| calledFrom == 0x515730  // CVehicle::ClearWindowOpenFlag
		|| calledFrom == 0x338698  // CVehicleModelInfo::GetOriginalCompPosition
		|| calledFrom == 0x338B2C) // CVehicleModelInfo::CreateInstance
		return NULL;

	for (uint i = 2; i < 40; i++)
	{
		RwFrame *pNewFrameResult = NULL;
		uint uiNewId = id + (i / 2) * ((i & 1) ? -1 : 1);

		pNewFrameResult = ((RwFrame * (*)(RpClump * pClump, int id))(g_libGTASA + 0x335CC0 + 1))(pClump, i);

		if (pNewFrameResult)
		{
			return pNewFrameResult;
		}
	}

	return NULL;
}
RwFrame *(*CClumpModelInfo_GetFrameFromId)(RpClump *, int);
RwFrame *CClumpModelInfo_GetFrameFromId_hook(RpClump *a1, int a2)
{
	return CClumpModelInfo_GetFrameFromId_Post(CClumpModelInfo_GetFrameFromId(a1, a2), a1, a2);
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
uint8_t *(*RLEDecompress)(uint8_t *pDest, size_t uiDestSize, uint8_t const *pSrc, size_t uiSegSize, uint32_t uiEscape);
uint8_t *RLEDecompress_hook(uint8_t *pDest, size_t uiDestSize, uint8_t const *pSrc, size_t uiSegSize, uint32_t uiEscape)
{
	if (!pDest)
	{
		return pDest;
	}

	if (!pSrc)
	{
		return pDest;
	}

	uint8_t *pTempDest = pDest;
	const uint8_t *pTempSrc = pSrc;
	uint8_t *pEndOfDest = &pDest[uiDestSize];

	uint8_t *pEndOfSrc = (uint8_t *)&pSrc[dwRLEDecompressSourceSize];

	if (pDest < pEndOfDest)
	{
		do
		{
			if (*pTempSrc == uiEscape)
			{
				uint8_t ucCurSeg = pTempSrc[1];
				if (ucCurSeg)
				{
					uint8_t *ucCurDest = pTempDest;
					uint8_t ucCount = 0;
					do
					{
						++ucCount;
						// Log("WRITE111 TO 0x%x FROM 0x%x SIZE %d", ucCurDest, pTempSrc + 2, uiSegSize);
						pDest = (uint8_t *)memcpy(ucCurDest, pTempSrc + 2, uiSegSize);

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
					// Log("AVOID CRASH TO 0x%x FROM 0x%x SIZE %d END OF SRC 0x%x", pTempDest, pTempSrc, pEndOfSrc - pTempSrc - 1, pEndOfSrc);
					return pDest;
				}
				else
				{
					// Log("WRITE222 TO 0x%x FROM 0x%x SIZE %d END OF SRC 0x%x", pTempDest, pTempSrc, uiSegSize, pEndOfSrc);
					pDest = (uint8_t *)memcpy(pTempDest, pTempSrc, uiSegSize);
					pTempDest += uiSegSize;
					pTempSrc += uiSegSize;
				}
			}
		} while (pEndOfDest > pTempDest);
	}

	dwRLEDecompressSourceSize = 0;

	return pDest;
}

#include "..//crashlytics.h"

char g_bufRenderQueueCommand[200];
uintptr_t g_dwRenderQueueOffset;

char *(*RenderQueue__ProcessCommand)(uintptr_t thiz, char *a2);
char *RenderQueue__ProcessCommand_hook(uintptr_t thiz, char *a2)
{
	if (thiz && a2)
	{
		uintptr_t *dwRenderQueue = (uintptr_t *)thiz;

		memset(g_bufRenderQueueCommand, 0, sizeof(g_bufRenderQueueCommand));

		g_dwRenderQueueOffset = *(uintptr_t *)a2;
		snprintf(g_bufRenderQueueCommand, 190, "offset: %d | name: %s", g_dwRenderQueueOffset, (const char *)(*(dwRenderQueue + 100 + g_dwRenderQueueOffset)));

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

uintptr_t *CTouchInterface__m_bWidgets;

static bool ShouldBeProcessedButton(int result)
{
	CTouchInterface__m_bWidgets = (uintptr_t *)(g_libGTASA + 0x657E48);

	if (result == CTouchInterface__m_bWidgets[18])
	{
		return false;
	}

	if (result == CTouchInterface__m_bWidgets[26] || result == CTouchInterface__m_bWidgets[27])
	{
		if (pNetGame)
		{
			CLocalPlayer *pLocal = pNetGame->GetPlayerPool()->GetLocalPlayer();
			if (pLocal)
			{
				if (pLocal->GetPlayerPed())
				{
					VEHICLE_TYPE *pVehicle = pLocal->GetPlayerPed()->GetGtaVehicle();
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

void (*CWidgetButton__Update)(int result, int a2, int a3, int a4);
void CWidgetButton__Update_hook(int result, int a2, int a3, int a4)
{
	if (!result)
	{
		return;
	}
	if (ShouldBeProcessedButton(result))
	{
		return CWidgetButton__Update(result, a2, a3, a4);
	}
	else
	{
		return;
	}
}

int (*CWidget__IsTouched)(uintptr_t a1);
int CWidget__IsTouched_hook(uintptr_t a1)
{
	if (ShouldBeProcessedButton(a1))
	{
		return CWidget__IsTouched(a1);
	}
	else
	{
		return 0;
	}
}

void (*CDarkel__RegisterCarBlownUpByPlayer)(void *pVehicle, int arg2);
void CDarkel__RegisterCarBlownUpByPlayer_hook(void *pVehicle, int arg2)
{
	return;
}
void (*CDarkel__ResetModelsKilledByPlayer)(int playerid);
void CDarkel__ResetModelsKilledByPlayer_hook(int playerid)
{
	return;
}
int (*CDarkel__QueryModelsKilledByPlayer)(int, int);
int CDarkel__QueryModelsKilledByPlayer_hook(int player, int modelid)
{
	return 0;
}

int (*CDarkel__FindTotalPedsKilledByPlayer)(int player);
int CDarkel__FindTotalPedsKilledByPlayer_hook(int player)
{
	return 0;
}

void (*CDarkel__RegisterKillByPlayer)(void *pKilledPed, unsigned int damageWeaponID, bool bHeadShotted, int arg4);
void CDarkel__RegisterKillByPlayer_hook(void *pKilledPed, unsigned int damageWeaponID, bool bHeadShotted, int arg4)
{
	return;
}

std::list<std::pair<unsigned int *, unsigned int>> resetEntriesVehicle;

RpMaterial *CVehicle__SetupRenderMatCB(RpMaterial *material, void *data)
{
	if (material)
	{
		if (material->texture)
		{
			CVehicle *pVeh = (CVehicle *)data;
			for (size_t i = 0; i < MAX_REPLACED_TEXTURES; i++)
			{
				if (pVeh->m_bReplaceTextureStatus[i])
				{
					if (!strcmp(&(material->texture->name[0]), &(pVeh->m_szReplacedTextures[i].szOld[0])))
					{
						if (pVeh->m_szReplacedTextures[i].pTexture)
						{
							resetEntriesVehicle.push_back(std::make_pair(reinterpret_cast<unsigned int *>(&(material->texture)), *reinterpret_cast<unsigned int *>(&(material->texture))));
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

uintptr_t CVehicle__SetupRenderCB(uintptr_t atomic, void *data)
{
	if (*(RpGeometry **)(atomic + 24))
	{
		((RpGeometry * (*)(RpGeometry *, uintptr_t, void *))(g_libGTASA + 0x1E284C + 1))(*(RpGeometry **)(atomic + 24), (uintptr_t)CVehicle__SetupRenderMatCB, (void *)data); // RpGeometryForAllMaterials
	}

	return atomic;
}
#include "..//cryptors/MODELINFO_EDITABLE_result.h"
void (*CVehicleModelInfo__SetEditableMaterials)(uintptr_t);
void CVehicleModelInfo__SetEditableMaterials_hook(uintptr_t clump)
{
	PROTECT_CODE_MODELINFO_EDITABLE;
	RpClump *pClump = (RpClump *)clump;

	if (pNetGame && pClump)
	{
		if (pNetGame->GetVehiclePool())
		{
			VEHICLEID vehId = pNetGame->GetVehiclePool()->FindIDFromRwObject((RwObject *)clump);
			CVehicle *pVehicle = pNetGame->GetVehiclePool()->GetAt(vehId);
			if (pVehicle)
			{
				if (pVehicle->m_bReplacedTexture)
				{
					// RpClump* RpClumpForAllAtomics(RpClump* clump, RpAtomicCallBack callback, void* pData);
					((RpClump * (*)(RpClump *, uintptr_t, void *))(g_libGTASA + 0x1E0EA0 + 1))(pClump, (uintptr_t)CVehicle__SetupRenderCB, (void *)pVehicle); // RpClumpForAllAtomics
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

	for (auto &p : resetEntriesVehicle)
	{
		*p.first = p.second;
	}
	resetEntriesVehicle.clear();

	CVehicle__ResetAfterRender(thiz);
}

void (*CRopes__Update)();
void CRopes__Update_hook()
{
}

void *(*CCustomCarEnvMapPipeline__pluginEnvMatDestructorCB)(void *object, RwInt32 offset, RwInt32 size);
void *CCustomCarEnvMapPipeline__pluginEnvMatDestructorCB_hook(void *object, RwInt32 offset, RwInt32 size)
{
	if (pChatWindow)
		pChatWindow->AddDebugMessage("m_objects %x", *(uintptr_t **)(g_libGTASA + 0x669E48));
	return CCustomCarEnvMapPipeline__pluginEnvMatDestructorCB(object, offset, size);
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
	if (pGame->IsGamePaused())
		return;

	// CSkyBox::Process();
	MainLoop();

	if (pNetGame && pNetGame->GetPlayerPool() && pNetGame->GetPlayerPool()->GetLocalPlayer())
	{
		CSnow::Process(pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed(), pGame->GetActiveInterior());
	}

	if (pNetGame)
	{
		CTextDrawPool *pTextDrawPool = pNetGame->GetTextDrawPool();
		if (pTextDrawPool)
		{
			pTextDrawPool->SnapshotProcess();
		}
	}

	if (g_pWidgetManager)
	{
		PED_TYPE *pPed = GamePool_FindPlayerPed();
	//	if (g_pWidgetManager->GetSlotState(WIDGET_CAMERA_CYCLE) && pPed)
	//	{
	//		static uint32_t lastTick = GetTickCount();
	//		bool bPressed = false;
	//		if (g_pWidgetManager->IsTouched(WIDGET_CAMERA_CYCLE) && GetTickCount() - lastTick >= 250)
	//		{
	//			bPressed = true;
	//			lastTick = GetTickCount();
	//		}
	//
	//		if (!CFirstPersonCamera::IsEnabled() && g_bFirstPersonOnFootEnabled)
	//		{
	//			CFirstPersonCamera::SetEnabled(true);
	//		}
	//
	//		if (CFirstPersonCamera::IsEnabled() && !g_bFirstPersonOnFootEnabled)
	//		{
	//			CFirstPersonCamera::SetEnabled(false);
	//		}
	//
	//		if (bPressed && !IN_VEHICLE(pPed))
	//		{
	//			CFirstPersonCamera::Toggle();
	//			if (CFirstPersonCamera::IsEnabled())
	//			{
	//				g_bFirstPersonOnFootEnabled = true;
	//			}
	//			else
	//			{
	//				g_bFirstPersonOnFootEnabled = false;
	//			}
	//		}
	//	}

		if (g_pWidgetManager->GetSlotState(WIDGET_ENTEREXIT) && pPed)
		{
			static uint32_t lastTick = GetTickCount();
			if (g_pWidgetManager->IsTouched(WIDGET_ENTEREXIT) && GetTickCount() - lastTick >= 250)
			{
				CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
				if (pPlayerPool)
				{
					CLocalPlayer *pLocalPlayer;
					if (!pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsInVehicle() && !pPlayerPool->GetLocalPlayer()->GetPlayerPed()->IsAPassenger())
					{
						if (pNetGame)
						{
							if (pPlayerPool)
							{
								pLocalPlayer = pPlayerPool->GetLocalPlayer();
								if (pLocalPlayer)
								{
									pLocalPlayer->HandlePassengerEntryEx();
								}
							}
						}
					}
				}
				lastTick = GetTickCount();
			}
		}
	}

	if (pAudioStream)
		pAudioStream->Process();

	CCustomPlateManager::Process();
}

#include "..//cryptors/AUTOMOBILE_COLLISION_result.h"

uint16_t g_usLastProcessedModelIndexAutomobile = 0;
int g_iLastProcessedModelIndexAutoEnt = 0;
void (*CAutomobile__ProcessEntityCollision)(VEHICLE_TYPE *a1, ENTITY_TYPE *a2, int a3);
void CAutomobile__ProcessEntityCollision_hook(VEHICLE_TYPE *a1, ENTITY_TYPE *a2, int a3)
{
	PROTECT_CODE_AUTOMOBILE_COLLISION;

	g_usLastProcessedModelIndexAutomobile = a1->entity.nModelIndex;
	g_iLastProcessedModelIndexAutoEnt = a2->nModelIndex;

	bool bReplace = false;
	void *pOld = nullptr;
	uint8_t *pColData = nullptr;

	if (pNetGame)
	{
		if (pNetGame->GetVehiclePool())
		{
			uint16_t vehId = pNetGame->GetVehiclePool()->FindIDFromGtaPtr(a1);
			CVehicle *pVeh = pNetGame->GetVehiclePool()->GetAt(vehId);
			if (pVeh)
			{
				if (pVeh->bHasSuspensionLines && pVeh->GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
				{
					pColData = GetCollisionDataFromModel(a1->entity.nModelIndex);
					if (pColData && pVeh->m_pSuspensionLines)
					{
						if (*(void **)(pColData + 16))
						{
							pOld = *(void **)(pColData + 16);
							*(void **)(pColData + 16) = pVeh->m_pSuspensionLines;
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
		*(void **)(pColData + 16) = pOld;
	}
}

bool (*CGame__Shutdown)();
bool CGame__Shutdown_hook()
{
	Log("Exiting game...");
	NOP(g_libGTASA + 0x341FCC, 2); // nop PauseOpenSLES
	NOP(g_libGTASA + 0x46389E, 2); // nop saving
	if (pNetGame)
	{
		if (pNetGame->GetRakClient())
		{
			pNetGame->GetRakClient()->Disconnect(500, 0);
		}
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	return CGame__Shutdown();
}

// TODO: VEHICLE RESET SUSPENSION
void (*CShadows__StoreCarLightShadow)(VEHICLE_TYPE *vehicle, int id, RwTexture *texture, VECTOR *posn, float frontX, float frontY, float sideX, float sideY, unsigned char red, unsigned char green, unsigned char blue, float maxViewAngle);
void CShadows__StoreCarLightShadow_hook(VEHICLE_TYPE *vehicle, int id, RwTexture *texture, VECTOR *posn, float frontX, float frontY, float sideX, float sideY, unsigned char red, unsigned char green, unsigned char blue, float maxViewAngle)
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
			CVehicle *pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
			if (pVeh)
			{
				pVeh->ProcessHeadlightsColor(r, g, b);
			}
		}
	}

	return CShadows__StoreCarLightShadow(vehicle, id, texture, posn, frontX, frontY, sideX, sideY, r, g, b, maxViewAngle);
}

void CVehicle__DoHeadLightReflectionTwin(CVehicle *pVeh, MATRIX4X4 *a2)
{
	VEHICLE_TYPE *v2; // r4
	int v3;			  // r2
	MATRIX4X4 *v4;	  // r5
	float *v5;		  // r3
	float v6;		  // s12
	float v7;		  // s5
	float *v8;		  // r2
	float v9;		  // r0
	float v10;		  // r1
	float v11;		  // r2
	float v12;		  // s14
	float v13;		  // s11
	float v14;		  // s15
	float v15;		  // s13
	float v16;		  // s10
	float v17;		  // s12
	float v18;		  // s15
	float v19;		  // ST08_4

	uintptr_t *dwModelarray = (uintptr_t *)(g_libGTASA + 0x87BF48);

	v2 = pVeh->m_pVehicle;
	v3 = *((uintptr_t *)v2 + 5);
	v4 = a2;
	v5 = *(float **)(dwModelarray[v2->entity.nModelIndex] + 116);
	v6 = *v5;
	v7 = v5[1];
	if (v3)
		v8 = (float *)(v3 + 48);
	else
		v8 = (float *)((char *)v2 + 4);
	v9 = *v8;
	v10 = v8[1];
	v11 = v8[2];
	v12 = *((float *)v4 + 5);
	v13 = *((float *)v4 + 4);
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

void (*CVehicle__DoHeadLightBeam)(VEHICLE_TYPE *vehicle, int arg0, MATRIX4X4 &matrix, unsigned char arg2);
void CVehicle__DoHeadLightBeam_hook(VEHICLE_TYPE *vehicle, int arg0, MATRIX4X4 &matrix, unsigned char arg2)
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
			CVehicle *pVeh = pNetGame->GetVehiclePool()->GetAt(vehid);
			if (pVeh)
			{
				pVeh->ProcessHeadlightsColor(r, g, b);
			}
		}
	}

	*(uint8_t *)(g_libGTASA + 0x9BAA70) = r;
	*(uint8_t *)(g_libGTASA + 0x9BAA71) = g;
	*(uint8_t *)(g_libGTASA + 0x9BAA72) = b;

	*(uint8_t *)(g_libGTASA + 0x9BAA94) = r;
	*(uint8_t *)(g_libGTASA + 0x9BAA95) = g;
	*(uint8_t *)(g_libGTASA + 0x9BAA96) = b;

	*(uint8_t *)(g_libGTASA + 0x9BAB00) = r;
	*(uint8_t *)(g_libGTASA + 0x9BAB01) = g;
	*(uint8_t *)(g_libGTASA + 0x9BAB02) = b;

	CVehicle__DoHeadLightBeam(vehicle, arg0, matrix, arg2);
}

static CVehicle *g_pLastProcessedVehicleMatrix = nullptr;
static int g_iLastProcessedWheelVehicle = -1;

void (*CMatrix__Rotate)(void *thiz, float a1, float a2, float a3);
void CMatrix__Rotate_hook(void *thiz, float a1, float a2, float a3)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile("mov %0, lr"
					 : "=r"(dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x3A9D76 + 1)
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
				((void (*)(void *, float))(g_libGTASA + 0x3E8BE4 + 1))(thiz, 0.0f - g_pLastProcessedVehicleMatrix->m_fWheelAlignmentX); // CMatrix::RotateY
			}
			if (g_iLastProcessedWheelVehicle == 4)
			{
				((void (*)(void *, float))(g_libGTASA + 0x3E8BE4 + 1))(thiz, 0.0f - g_pLastProcessedVehicleMatrix->m_fWheelAlignmentY); // CMatrix::RotateY
			}
			if (g_iLastProcessedWheelVehicle == 5)
			{
				((void (*)(void *, float))(g_libGTASA + 0x3E8BE4 + 1))(thiz, g_pLastProcessedVehicleMatrix->m_fWheelAlignmentX); // CMatrix::RotateY
			}
			if (g_iLastProcessedWheelVehicle == 7)
			{
				((void (*)(void *, float))(g_libGTASA + 0x3E8BE4 + 1))(thiz, g_pLastProcessedVehicleMatrix->m_fWheelAlignmentY); // CMatrix::RotateY
			}
		}
	}
}

void (*CMatrix__SetScale)(void *thiz, float x, float y, float z);
void CMatrix__SetScale_hook(void *thiz, float x, float y, float z)
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

void (*CAutomobile__UpdateWheelMatrix)(VEHICLE_TYPE *thiz, int, int);
void CAutomobile__UpdateWheelMatrix_hook(VEHICLE_TYPE *thiz, int nodeIndex, int flags)
{
	if (g_pLastProcessedVehicleMatrix)
	{
		g_iLastProcessedWheelVehicle = nodeIndex;
	}

	CAutomobile__UpdateWheelMatrix(thiz, nodeIndex, flags);
}

void (*CAutomobile__PreRender)(VEHICLE_TYPE *thiz);
void CAutomobile__PreRender_hook(VEHICLE_TYPE *thiz)
{
	uintptr_t *dwModelarray = (uintptr_t *)(g_libGTASA + 0x87BF48);
	uint8_t *pModelInfoStart = (uint8_t *)dwModelarray[thiz->entity.nModelIndex];

	float fOldFront = *(float *)(pModelInfoStart + 88);
	float fOldRear = *(float *)(pModelInfoStart + 92);
	CVehicle *pVeh = nullptr;
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
					*(float *)(pModelInfoStart + 92) = pVeh->m_fWheelSize;
					*(float *)(pModelInfoStart + 88) = pVeh->m_fWheelSize;
				}
				if (pVeh->m_bShadow && pVeh->m_Shadow.pTexture)
				{
					CVehicle__DoHeadLightReflectionTwin(pVeh, pVeh->m_pVehicle->entity.mat);
				}
			}
		}
	}

	CAutomobile__PreRender(thiz);

	*(float *)(pModelInfoStart + 88) = fOldFront;
	*(float *)(pModelInfoStart + 92) = fOldRear;

	g_pLastProcessedVehicleMatrix = nullptr;
	g_iLastProcessedWheelVehicle = -1;
}

#include "..//cryptors/INSTALLHOOKS_result.h"

void (*CTaskSimpleUseGun__RemoveStanceAnims)(void *thiz, void *ped, float a3);
void CTaskSimpleUseGun__RemoveStanceAnims_hook(void *thiz, void *ped, float a3)
{
	uint32_t v5 = *((uint32_t *)thiz + 11);
	if (v5)
	{
		if (*(uint32_t *)(v5 + 20))
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

float (*CRadar__LimitRadarPoint)(float *a1);
float CRadar__LimitRadarPoint_hook(float *a1)
{
	if (*(uint8_t *)(g_libGTASA + 0x63E0B4))
	{
		return sqrtf((float)(a1[1] * a1[1]) + (float)(*a1 * *a1));
	}

	if (!CRadarRect::IsEnabled())
	{
		return CRadar__LimitRadarPoint(a1);
	}
	float value = CRadarRect::CRadar__LimitRadarPoint_hook(a1);

	if (pChatWindow)
	{
		// pChatWindow->AddDebugMessage("VALUE %f", value);
	}

	return value;
}

void (*CSprite2d__DrawBarChart)(float x, float y, unsigned short width, unsigned char height, float progress, signed char progressAdd,
								unsigned char drawPercentage, unsigned char drawBlackBorder, CRGBA *color, CRGBA *addColor);
void CSprite2d__DrawBarChart_hook(float x, float y, unsigned short width, unsigned char height, float progress, signed char progressAdd,
								  unsigned char drawPercentage, unsigned char drawBlackBorder, CRGBA *color, CRGBA *addColor)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile("mov %0, lr"
					 : "=r"(dwRetAddr));
	dwRetAddr -= g_libGTASA;

	float fX = x;
	float fY = y;

	unsigned short usWidth = width;
	unsigned char usHeight = height;

	if (dwRetAddr == 0x27D524 + 1)
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
	}
	else if (dwRetAddr == 0x27D83E + 1)
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
	}
	CSprite2d__DrawBarChart(fX, fY, usWidth, usHeight, progress, progressAdd, drawPercentage, drawBlackBorder, color, addColor);
}
static int g_iCurrentWanted = 0;
static float g_fInitialPos = 0.0f;
void (*CWidgetPlayerInfo__DrawWanted)(void *);
void CWidgetPlayerInfo__DrawWanted_hook(void *thiz)
{
	g_iCurrentWanted = 0;
	g_fInitialPos = *((float *)thiz + 10);
	CWidgetPlayerInfo__DrawWanted(thiz);
	g_iCurrentWanted = 0;
}

void (*CFont__PrintString)(float x, float y, uint16_t *text);
void CFont__PrintString_hook(float x, float y, uint16_t *text)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile("mov %0, lr"
					 : "=r"(dwRetAddr));
	dwRetAddr -= g_libGTASA;

	float fX = x;
	float fY = y;

	if (dwRetAddr == 0x27E15C + 1) // money
	{
		if (CAdjustableHudColors::IsUsingHudColor(E_HUD_ELEMENT::HUD_MONEY))
		{
			CRGBA col = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_MONEY);
			uint32_t m1 = col.ToInt();
			CFont::SetColor(&m1);
		}

		if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_MONEY).X != -1)
		{
			float value = (float)CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_MONEY).X / 100.0f;
			CFont::SetScale(value, 0.0f);
		}

		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_MONEY).X != -1)
		{
			fX = pGUI->ScaleX(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_MONEY).X);
		}
		if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_MONEY).Y != -1)
		{
			fY = pGUI->ScaleY(CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_MONEY).Y);
		}
	}
	else if (dwRetAddr == 0x27D9E6 + 1) // wanted
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
	else if (dwRetAddr == 0x27D330 + 1) // ammo text
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

void (*CSprite2d__Draw)(CSprite2d *a1, CRect *a2, CRGBA *a3);
void CSprite2d__Draw_hook(CSprite2d *a1, CRect *a2, CRGBA *a3)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile("mov %0, lr"
					 : "=r"(dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (pCrossHair)
		if (pCrossHair->IsNeededFix())
			if (a1 == (CSprite2d *)(g_libGTASA + 0x8F0708))
				return; // fix crosshair

	if (!pGUI)
	{
		return CSprite2d__Draw(a1, a2, a3);
	}

	if (dwRetAddr == 0x3D3796 + 1 || dwRetAddr == 0x3D376C + 1 || dwRetAddr == 0x3D373E + 1 || dwRetAddr == 0x3D3710 + 1)
	{
		if (CRadarRect::m_pDiscTexture == nullptr)
		{
			CRadarRect::m_pDiscTexture = a1->m_pRwTexture;
		}
		if (CRadarRect::IsEnabled() && CRadarRect::m_pRectTexture)
		{
			a1->m_pRwTexture = CRadarRect::m_pRectTexture;
			// pChatWindow->AddDebugMessage("Replacing for rect");
		}
		else
		{
			a1->m_pRwTexture = CRadarRect::m_pDiscTexture;
			// pChatWindow->AddDebugMessage("Replacing for disc");
		}

		if (CAdjustableHudColors::IsUsingHudColor(E_HUD_ELEMENT::HUD_RADAR))
		{
			CRGBA col = CAdjustableHudColors::GetHudColor(E_HUD_ELEMENT::HUD_RADAR);
			a3->A = col.A;
			a3->B = col.B;
			a3->G = col.G;
			a3->R = col.R;
		}
		float *thiz = (float *)*(uintptr_t *)(g_libGTASA + 0x6580C8);
		if (thiz)
		{
			if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_RADAR).X != -1)
			{
				thiz[3] = (float)CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_RADAR).X;
			}
			else
			{
				thiz[3] = 50.0f;
			}

			if (CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_RADAR).Y != -1)
			{
				thiz[4] = (float)CAdjustableHudPosition::GetElementPosition(E_HUD_ELEMENT::HUD_RADAR).Y;
			}
			else
			{
				thiz[4] = 70.0f;
			}

			if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_RADAR).X != -1)
			{
				thiz[5] = 45.0f * ((float)CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_RADAR).X / 100.0f);
			}
			else
			{
				thiz[5] = 45.0f;
			}

			if (CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_RADAR).X != -1)
			{
				thiz[6] = 45.0f * ((float)CAdjustableHudScale::GetElementScale(E_HUD_ELEMENT::HUD_RADAR).X / 100.0f);
			}
			else
			{
				thiz[6] = 45.0f;
			}
		}
	}

	return CSprite2d__Draw(a1, a2, a3);
}

void (*CSprite2d__DrawRect)(RECT *a1, float a2);
void CSprite2d__DrawRect__hook(RECT *a1, float a2)
{
	// siteM16 rect
	float fCHairScreenMultX = RsGlobal->maximumWidth * *(float *)(g_libGTASA + 0x8B07FC);
	float fCHairScreenMultY = RsGlobal->maximumHeight * *(float *)(g_libGTASA + 0x8B07F8);
	if (pCrossHair)
		if (pCrossHair->IsNeededFix())
			if (a1->fLeft == (fCHairScreenMultX - 1.0) && a1->fRight == (fCHairScreenMultX + 1.0))
				return;

	return CSprite2d__DrawRect(a1, a2);
}

void (*CWidgetPlayerInfo__DrawWeaponIcon)(float *thiz, void *ped, CRect rect, float a4);
void CWidgetPlayerInfo__DrawWeaponIcon_hook(float *thiz, void *ped, CRect rect, float a4)
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

void (*CHud__Draw)();
void CHud__Draw_hook()
{
	float *thiz = (float *)*(uintptr_t *)(g_libGTASA + 0x6580C8);
	if (thiz)
	{
		thiz[5] = 45.0f;
		thiz[6] = 45.0f;
	}

	CHud__Draw();
}

void RenderBackgroundHud();
void (*CHudRender__Draw)();
void CHudRender__Draw_hook()
{
	RenderBackgroundHud();
	CHudRender__Draw();
}

void (*CCam__Process)(uintptr_t);
void CCam__Process_hook(uintptr_t thiz)
{
	if (!CFirstPersonCamera::IsEnabled())
		return CCam__Process(thiz);

	if (pChatWindow)
	{
		// pChatWindow->AddDebugMessage("time %d %d %d", *(uint32_t*)(g_libGTASA + 0x8B0808 + 116), *(uint32_t*)(g_libGTASA + 0x8B0808 + 120), *(uint32_t*)(g_libGTASA + 0x8B0808 + 124));
		// pChatWindow->AddDebugMessage("camera %d %f %f", *(uint16_t*)(thiz + 14), *(float*)(thiz + 132), *(float*)(thiz + 148)); // 140 - fov, 132 - vertical, 148 - horizontal angle
	}

	VECTOR vecSpeed;
	CVehicle *pVeh = nullptr;
	float pOld = *(float *)(g_libGTASA + 0x608558);
	if (pNetGame && (*(uint16_t *)(thiz + 14) == 18 || *(uint16_t *)(thiz + 14) == 16) && CFirstPersonCamera::IsEnabled())
	{
		if (pNetGame->GetPlayerPool())
		{
			if (pNetGame->GetPlayerPool()->GetLocalPlayer())
			{
				CPlayerPed *pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
				pVeh = pNetGame->GetVehiclePool()->GetAt(pNetGame->GetPlayerPool()->GetLocalPlayer()->m_CurrentVehicle);
				if (pVeh)
				{
					pVeh->GetMoveSpeedVector(&vecSpeed);
					VECTOR vec;
					vec.X = vecSpeed.X * 6.0f;
					vec.Y = vecSpeed.Y * 6.0f;
					vec.Z = vecSpeed.Z * 6.0f;
					pVeh->SetMoveSpeedVector(vec);
					*(float *)(g_libGTASA + 0x608558) = 200.0f;
				}
			}
		}
	}

	CCam__Process(thiz);
	if (pVeh)
	{
		pVeh->SetMoveSpeedVector(vecSpeed);
		*(float *)(g_libGTASA + 0x608558) = pOld;
	}
	if (*(uint16_t *)(thiz + 14) == 4 || *(uint16_t *)(thiz + 14) == 53) // 53 is weapon
	{
		if (pNetGame)
		{
			if (pNetGame->GetPlayerPool())
			{
				if (pNetGame->GetPlayerPool()->GetLocalPlayer())
				{
					CPlayerPed *pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
					if (pPed)
					{
						*(uint32_t *)(g_libGTASA + 0x8B0808 + 120) = 0xFFFFFFFF;
						*(uint32_t *)(g_libGTASA + 0x8B0808 + 124) = 0xFFFFFFFF;
						*(uint8_t *)(g_libGTASA + 0x8B0808 + 40) = 0;
						CFirstPersonCamera::ProcessCameraOnFoot(thiz, pPed);
					}
				}
			}
		}
	}
	if (*(uint16_t *)(thiz + 14) == 18 || *(uint16_t *)(thiz + 14) == 16) // in car
	{
		if (pNetGame)
		{
			if (pNetGame->GetPlayerPool())
			{
				if (pNetGame->GetPlayerPool()->GetLocalPlayer())
				{
					CPlayerPed *pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
					if (pPed)
					{
						*(uint32_t *)(g_libGTASA + 0x8B0808 + 120) = 0xFFFFFFFF;
						*(uint32_t *)(g_libGTASA + 0x8B0808 + 124) = 0xFFFFFFFF;
						*(uint8_t *)(g_libGTASA + 0x8B0808 + 40) = 0;
						CFirstPersonCamera::ProcessCameraInVeh(thiz, pPed, pVeh);
					}
				}
			}
		}
	}
}
int g_iCounterVehicleCamera = 0;
int (*CPad__CycleCameraModeDownJustDown)(void *);
int CPad__CycleCameraModeDownJustDown_hook(void *thiz)
{
	if (!g_pWidgetManager)
	{
		return 0;
	}
	//if (!g_pWidgetManager->GetSlotState(WIDGET_CAMERA_CYCLE))
	//{
	//	return 0;
	//}
	PED_TYPE *pPed = GamePool_FindPlayerPed();
	if (!pPed)
	{
		return 0;
	}

	static uint32_t lastTick = GetTickCount();
	bool bPressed = false;
	//if (g_pWidgetManager->IsTouched(WIDGET_CAMERA_CYCLE) && GetTickCount() - lastTick >= 250)
	//{
	//	bPressed = true;
	//	lastTick = GetTickCount();
	//}

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

int (*CEntity__GetIsOnScreen)(ENTITY_TYPE *);
int CEntity__GetIsOnScreen_hook(ENTITY_TYPE *thiz)
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

void (*FxEmitterBP_c__Render)(uintptr_t *a1, int a2, int a3, float a4, char a5);
void FxEmitterBP_c__Render_hook(uintptr_t *a1, int a2, int a3, float a4, char a5)
{
	uintptr_t *temp = *((uintptr_t **)a1 + 3);
	if (!temp)
	{
		return;
	}
	FxEmitterBP_c__Render(a1, a2, a3, a4, a5);
}

void (*CStreaming__RemoveModel)(int);
void CStreaming__RemoveModel_hook(int model)
{
	Log("Removing model %d", model);
	CStreaming__RemoveModel(model);
}

int g_iLastProcessedSkinCollision = 228;
int g_iLastProcessedEntityCollision = 228;

void (*CPed__ProcessEntityCollision)(PED_TYPE *thiz, ENTITY_TYPE *ent, void *colPoint);
void CPed__ProcessEntityCollision_hook(PED_TYPE *thiz, ENTITY_TYPE *ent, void *colPoint)
{
	g_iLastProcessedSkinCollision = thiz->entity.nModelIndex;
	g_iLastProcessedEntityCollision = ent->nModelIndex;

	CPed__ProcessEntityCollision(thiz, ent, colPoint);
}

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

void SendBulletSync(VECTOR *vecOrigin, VECTOR *vecEnd, VECTOR *vecPos, ENTITY_TYPE **ppEntity)
{
	BULLET_DATA bulletData;
	memset(&bulletData, 0, sizeof(BULLET_DATA));

	bulletData.vecOrigin.X = vecOrigin->X;
	bulletData.vecOrigin.Y = vecOrigin->Y;
	bulletData.vecOrigin.Z = vecOrigin->Z;
	bulletData.vecPos.X = vecPos->X;
	bulletData.vecPos.Y = vecPos->Y;
	bulletData.vecPos.Z = vecPos->Z;

	if (ppEntity)
	{
		ENTITY_TYPE *pEntity = *ppEntity;
		if (pEntity)
		{
			if (pEntity->mat)
			{
				bulletData.vecOffset.X = vecPos->X - pEntity->mat->pos.X;
				bulletData.vecOffset.Y = vecPos->Y - pEntity->mat->pos.Y;
				bulletData.vecOffset.Z = vecPos->Z - pEntity->mat->pos.Z;
			}

			bulletData.pEntity = pEntity;
		}
	}

	pGame->FindPlayerPed()->ProcessBulletData(&bulletData);
}

uint32_t (*CWorld__ProcessLineOfSight)(VECTOR *, VECTOR *, VECTOR *, PED_TYPE **, bool, bool, bool, bool, bool, bool, bool, bool);
uint32_t CWorld__ProcessLineOfSight_hook(VECTOR *vecOrigin, VECTOR *vecEnd, VECTOR *vecPos, PED_TYPE **ppEntity,
										 bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile("mov %0, lr"
					 : "=r"(dwRetAddr));

	dwRetAddr -= g_libGTASA;
	if (dwRetAddr == 0x55E2FE + 1 ||
		dwRetAddr == 0x5681BA + 1 ||
		dwRetAddr == 0x567AFC + 1)
	{
		ENTITY_TYPE *pEntity = nullptr;
		MATRIX4X4 *pMatrix = nullptr;
		static VECTOR vecPosPlusOffset;

		if (pNetGame->GetLagCompensation() != 2)
		{
			if (g_pCurrentFiredPed != pGame->FindPlayerPed())
			{
				if (g_pCurrentBulletData)
				{
					if (g_pCurrentBulletData->pEntity)
					{
						if (!IsGameEntityArePlaceable(g_pCurrentBulletData->pEntity))
						{
							pMatrix = g_pCurrentBulletData->pEntity->mat;
							if (pMatrix)
							{
								if (pNetGame->GetLagCompensation())
								{
									vecPosPlusOffset.X = pMatrix->pos.X + g_pCurrentBulletData->vecOffset.X;
									vecPosPlusOffset.Y = pMatrix->pos.Y + g_pCurrentBulletData->vecOffset.Y;
									vecPosPlusOffset.Z = pMatrix->pos.Z + g_pCurrentBulletData->vecOffset.Z;
								}
								else
								{
									ProjectMatrix(&vecPosPlusOffset, pMatrix, &g_pCurrentBulletData->vecOffset);
								}

								vecEnd->X = vecPosPlusOffset.X - vecOrigin->X + vecPosPlusOffset.X;
								vecEnd->Y = vecPosPlusOffset.Y - vecOrigin->Y + vecPosPlusOffset.Y;
								vecEnd->Z = vecPosPlusOffset.Z - vecOrigin->Z + vecPosPlusOffset.Z;
							}
						}
					}
				}
			}
		}

		static uint32_t result = 0;
		result = CWorld__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);

		if (pNetGame->GetLagCompensation() == 2)
		{
			if (g_pCurrentFiredPed)
			{
				if (g_pCurrentFiredPed == pGame->FindPlayerPed())
				{
					SendBulletSync(vecOrigin, vecEnd, vecPos, (ENTITY_TYPE **)ppEntity);
				}
			}

			return result;
		}

		if (g_pCurrentFiredPed)
		{
			if (g_pCurrentFiredPed != pGame->FindPlayerPed())
			{
				if (g_pCurrentBulletData)
				{
					if (!g_pCurrentBulletData->pEntity)
					{
						PED_TYPE *pLocalPed = pGame->FindPlayerPed()->GetGtaActor();
						if (*ppEntity == pLocalPed || (IN_VEHICLE(pLocalPed) && *(uintptr_t *)ppEntity == pLocalPed->pVehicle))
						{
							*ppEntity = nullptr;
							vecPos->X = 0.0f;
							vecPos->Y = 0.0f;
							vecPos->Z = 0.0f;
							return 0;
						}
					}
				}
			}
		}

		if (g_pCurrentFiredPed)
		{
			if (g_pCurrentFiredPed == pGame->FindPlayerPed())
			{
				SendBulletSync(vecOrigin, vecEnd, vecPos, (ENTITY_TYPE **)ppEntity);
			}
		}

		return result;
	}

	return CWorld__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);
}

bool (*CAutomobile__BurstTyre)(uintptr_t thiz, int a2, int a3);
bool CAutomobile__BurstTyre_hook(uintptr_t thiz, int a2, int a3)
{
	if (pNetGame)
	{
		// Did we get ped who fired?
		if (g_pCurrentFiredPed)
		{
			// Trying to get vehicle pointer
			CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
			if (pVehiclePool)
			{
				VEHICLEID vehId = pVehiclePool->FindIDFromGtaPtr((VEHICLE_TYPE *)thiz);
				if (vehId != INVALID_VEHICLE_ID)
				{
					CVehicle *pVehicle = pVehiclePool->GetAt(vehId);
					if (pVehicle)
					{
						pVehicle->ProcessDamage();
					}
				}
			}
		}
	}
	return CAutomobile__BurstTyre(thiz, a2, a3);
}

int (*CPlayerPed__ControlButtonSprint)(uintptr_t thiz, int a2);
int CPlayerPed__ControlButtonSprint_hook(uintptr_t thiz, int a2)
{
	if (pNetGame && pGame)
	{
		CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			if (pPlayerPed->IsInVehicle())
				return 0;
		}
	}
	return CPlayerPed__ControlButtonSprint(thiz, a2);
}

int (*CVehicle__UsesSiren)(uintptr_t thiz);
int CVehicle__UsesSiren_hook(uintptr_t thiz)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile("mov %0, lr"
					 : "=r"(dwRetAddr));

	dwRetAddr -= g_libGTASA;
	if (dwRetAddr == 0x4E53E0 + 1 ||
		dwRetAddr == 0x35BFE4 + 1 ||
		dwRetAddr == 0x518920 + 1)
	{
		if (pNetGame)
		{
			VEHICLE_TYPE *pGtaVehicle = (VEHICLE_TYPE *)thiz;
			if (pGtaVehicle)
			{
				CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
				if (pVehiclePool)
				{
					VEHICLEID vehId = pVehiclePool->FindIDFromGtaPtr(pGtaVehicle);
					if (vehId != INVALID_VEHICLE_ID)
					{
						if (pVehiclePool->GetSlotState(vehId))
						{
							CVehicle *pVehicle = pVehiclePool->GetAt(vehId);
							if (pVehicle)
							{
								if (pVehicle->IsSirenAdded())
									return 1;
							}
						}
					}
				}
			}
		}
	}
	return CVehicle__UsesSiren(thiz);
}

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

uintptr_t (*CPedDamageResponseCalculator__ComputeDamageResponse)(CPedDamageResponseCalculatorInterface *, PED_TYPE *, CPedDamageResponseInterface *, bool);
uintptr_t CPedDamageResponseCalculator__ComputeDamageResponse_hook(CPedDamageResponseCalculatorInterface *pPedDamageResponseCalculator, PED_TYPE *pDamagedPed, CPedDamageResponseInterface *pPedDamageResponse, bool bSpeak)
{
	// Make sure that everything is not null
	if (!pNetGame || !pPedDamageResponseCalculator || !pDamagedPed || !pPedDamageResponse || !pPedDamageResponseCalculator->pEntity)
		return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if (!pPlayerPool || !pVehiclePool)
		return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);

	CLocalPlayer *pLocalPlayer = pPlayerPool->GetLocalPlayer();
	if (!pLocalPlayer)
		return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);

	PED_TYPE *pInflictor = (PED_TYPE *)pPedDamageResponseCalculator->pEntity;
	PED_TYPE *pLocalPed = GamePool_FindPlayerPed();
	if (!pLocalPed)
		return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);

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
		return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);
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
		return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);
	}

	if (!pRemotePlayer)
		return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);

	// Send damage response now
	pLocalPlayer->GiveTakeDamage(bGiveOrTake, pRemotePlayer->GetID(), fDamage, pPedDamageResponseCalculator->weaponType, pPedDamageResponseCalculator->bodyPart);

	return CPedDamageResponseCalculator__ComputeDamageResponse(pPedDamageResponseCalculator, pDamagedPed, pPedDamageResponse, bSpeak);
}
/* ========================================================================================================== */

int (*CWeapon__GenerateDamageEvent)(PED_TYPE *victim, ENTITY_TYPE *creator, unsigned int weaponType, int damageFactor, ePedPieceTypes pedPiece, int direction);
int CWeapon__GenerateDamageEvent_hook(PED_TYPE *victim, ENTITY_TYPE *creator, unsigned int weaponType, int damageFactor, ePedPieceTypes pedPiece, int direction)
{
	if (pedPiece != PED_PIECE_HEAD)
	{
		// Disable ComputeDamageAnim when it's not a head!
		NOP(g_libGTASA + 0x566B30, 2);
		NOP(g_libGTASA + 0x566B88, 2);
	}
	int result = CWeapon__GenerateDamageEvent(victim, creator, weaponType, damageFactor, pedPiece, direction);
	if (pedPiece != PED_PIECE_HEAD)
	{
		// Restore bytes
		WriteMemory(g_libGTASA + 0x566B30, (uintptr_t) "\xC1\xF5\xD8\xF9", 4);
		WriteMemory(g_libGTASA + 0x566B88, (uintptr_t) "\xBB\xF5\xFE\xF8", 4);
	}
	return result;
}

int (*CTaskSimpleUseGun__SetPedPosition)(uintptr_t thiz, PED_TYPE *pPed);
int CTaskSimpleUseGun__SetPedPosition_hook(uintptr_t thiz, PED_TYPE *pPed)
{
	if (pPed && pPed->WeaponSlots && pPed->byteCurWeaponSlot < 13)
	{
		WEAPON_SLOT_TYPE curWeaponSlot = pPed->WeaponSlots[pPed->byteCurWeaponSlot];
		if ((curWeaponSlot.dwType == 37) || (curWeaponSlot.dwType == 41) || (curWeaponSlot.dwType == 42))
			*(uint8_t *)(thiz + 13) |= 1;
	}
	return CTaskSimpleUseGun__SetPedPosition(thiz, pPed);
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

int (*_RwTextureDestroy)(RwTexture *);
int _RwTextureDestroy_hook(RwTexture *texture)
{
	if (!texture)
		return 1;
	return _RwTextureDestroy(texture);
}

int (*CObject__ProcessGarageDoorBehaviour)(uintptr_t, int);
int CObject__ProcessGarageDoorBehaviour_hook(uintptr_t thiz, int a2)
{
	if (thiz)
		if (!*(uintptr_t *)(thiz + 372))
			return 0;
	return CObject__ProcessGarageDoorBehaviour(thiz, a2);
}

void (*CWidget__Update)(WIDGET_TYPE *);
void CWidget__Update_hook(WIDGET_TYPE *pWidget)
{
	if (pNetGame)
	{
		switch (widgets::ProcessFixed(pWidget))
		{
		case STATE_NONE:
			break;
		case STATE_FIXED:
			return;
		}
	}
	return CWidget__Update(pWidget);
}

void (*CWidget__SetEnabled)(WIDGET_TYPE *pWidget, bool bEnabled);
void CWidget__SetEnabled_hook(WIDGET_TYPE *pWidget, bool bEnabled)
{
	if (pNetGame)
	{
		switch (widgets::ProcessFixed(pWidget))
		{
		case STATE_NONE:
			break;
		case STATE_FIXED:
			bEnabled = false;
			break;
		}
	}
	return CWidget__SetEnabled(pWidget, bEnabled);
}

int (*emu_ArraysDelete)(unsigned int a1, int a2, int a3, int a4);
int emu_ArraysDelete_hook(unsigned int a1, int a2, int a3, int a4)
{
	if (a1 > g_libGTASA)
		return emu_ArraysDelete(a1, a2, a3, a4);
	return 0;
}

int V15lastest = 0;
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

int (*_RpMaterialDestroy)(RpMaterial *a1);
int _RpMaterialDestroy_hook(RpMaterial *a1)
{
	int result; // r0

	if (a1)
		result = _RpMaterialDestroy(a1);
	else
		result = 1;
	return result;
}

int (*CPlayerPedDataSaveStructure__Construct)(int, int);
int CPlayerPedDataSaveStructure__Construct_hook(int a1, int a2)
{
	bool v2;	// zf
	int result; // r0

	v2 = a1 == 0;
	if (a1)
		v2 = a2 == 0;
	if (v2 || !*(uint32_t *)a2)
		result = 0;
	else
		result = CPlayerPedDataSaveStructure__Construct(a1, a2);
	return result;
}

int (*CTextureDatabaseRuntime__GetEntry)(unsigned int a1, const char *a2, bool *a3, int a4);
int CTextureDatabaseRuntime__GetEntry_hook(unsigned int a1, const char *a2, bool *a3, int a4)
{
	int result; // r0

	if (a1)
		result = CTextureDatabaseRuntime__GetEntry(a1, a2, a3, a4);
	else
		result = -1;
	return result;
}

int (*CAEVehicleAudioEntity__GetAccelAndBrake)(int, int);
int CAEVehicleAudioEntity__GetAccelAndBrake_hook(int a1, int a2)
{
	bool v2;	// zf
	int result; // r0

	v2 = a1 == 0;
	if (a1)
		v2 = a2 == 0;
	if (v2 || !*(uint32_t *)(a1 + 12))
		result = 0;
	else
		result = CAEVehicleAudioEntity__GetAccelAndBrake(a1, a2);
	return result;
}

int (*CAudioEngine__Service)(unsigned int a1);
int CAudioEngine__Service_hook(unsigned int a1)
{
	int result; // r0

	if (a1)
		result = CAudioEngine__Service(a1);
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

void InstallHooks()
{
	PROTECT_CODE_INSTALLHOOKS;

	CPatch::InlineHook(g_libGTASA, 0x3961C8, &CFileMgr__ReadLine_hook, &CFileMgr__ReadLine);
	CPatch::InlineHook(g_libGTASA, 0x273338, &CWidget__Update_hook, &CWidget__Update);
	CPatch::InlineHook(g_libGTASA, 0x274178, &CWidget__SetEnabled_hook, &CWidget__SetEnabled);

	// Ped damage handler
	CPatch::InlineHook(g_libGTASA, 0x327528, &CPedDamageResponseCalculator__ComputeDamageResponse_hook, &CPedDamageResponseCalculator__ComputeDamageResponse);
	CPatch::InlineHook(g_libGTASA, 0x5669D8, &CWeapon__GenerateDamageEvent_hook, &CWeapon__GenerateDamageEvent);

	// Fire extingusher fix
	CPatch::InlineHook(g_libGTASA, 0x46D6AC, &CTaskSimpleUseGun__SetPedPosition_hook, &CTaskSimpleUseGun__SetPedPosition);

	CPatch::InlineHook(g_libGTASA, 0x281398, &CWidgetRegionLook__Update_hook, &CWidgetRegionLook__Update);
	CPatch::InlineHook(g_libGTASA, 0x3D7CA8, &CLoadingScreen_DisplayPCScreen_hook, &CLoadingScreen_DisplayPCScreen);
	CPatch::InlineHook(g_libGTASA, 0x39AEF4, &Render2dStuff_hook, &Render2dStuff);
	CPatch::InlineHook(g_libGTASA, 0x39B098, &Render2dStuffAfterFade_hook, &Render2dStuffAfterFade);
	CPatch::InlineHook(g_libGTASA, 0x239D5C, &TouchEvent_hook, &TouchEvent);
	CPatch::InlineHook(g_libGTASA, 0x28E83C, &CStreaming_InitImageList_hook, &CStreaming_InitImageList);
	CPatch::InlineHook(g_libGTASA, 0x336690, &CModelInfo_AddPedModel_hook, &CModelInfo_AddPedModel);		 // hook is dangerous
	CPatch::InlineHook(g_libGTASA, 0x3DBA88, &CRadar__GetRadarTraceColor_hook, &CRadar__GetRadarTraceColor); // dangerous
	CPatch::InlineHook(g_libGTASA, 0x3DAF84, &CRadar__SetCoordBlip_hook, &CRadar__SetCoordBlip);
	CPatch::InlineHook(g_libGTASA, 0x3DE9A8, &CRadar__DrawRadarGangOverlay_hook, &CRadar__DrawRadarGangOverlay);
	CPatch::InlineHook(g_libGTASA, 0x3C70C0, &CWorld__ProcessLineOfSight_hook, &CWorld__ProcessLineOfSight);
	CPatch::InlineHook(g_libGTASA, 0x482E60, &CTaskComplexEnterCarAsDriver_hook, &CTaskComplexEnterCarAsDriver);
	CPatch::InlineHook(g_libGTASA, 0x4833CC, &CTaskComplexLeaveCar_hook, &CTaskComplexLeaveCar);
	CodeInject(g_libGTASA + 0x2D99F4, (uintptr_t)PickupPickUp_hook, 1);

	// CPatch::InlineHook(g_libGTASA, 0x44A4CC, &PointGunInDirection_hook, &PointGunInDirection);
	// CPatch::InlineHook(g_libGTASA, 0x327528, &ComputeDamageResponse_hooked, (uintptr_t*)(&ComputeDamageResponse));

	CPatch::InlineHook(g_libGTASA, 0x336268, &CModelInfo_AddAtomicModel_hook, &CModelInfo_AddAtomicModel);
	CPatch::InlineHook(g_libGTASA, 0x567964, &CWeapon__FireInstantHit_hook, &CWeapon__FireInstantHit);
	CPatch::InlineHook(g_libGTASA, 0x336618, &CModelInfo_AddVehicleModel_hook, &CModelInfo_AddVehicleModel); // dangerous

	CPatch::InlineHook(g_libGTASA, 0x33DA5C, &CAnimManager__UncompressAnimation_hook, &CAnimManager__UncompressAnimation);
	CPatch::InlineHook(g_libGTASA, 0x531118, &CCustomRoadsignMgr__RenderRoadsignAtomic_hook, &CCustomRoadsignMgr__RenderRoadsignAtomic);
	CPatch::InlineHook(g_libGTASA, 0x1AECC0, &RwFrameAddChild_hook, &RwFrameAddChild);
	CPatch::InlineHook(g_libGTASA, 0x2DFD30, &CUpsideDownCarCheck__IsCarUpsideDown_hook, &CUpsideDownCarCheck__IsCarUpsideDown);
	CPatch::InlineHook(g_libGTASA, 0x33AD78, &CAnimBlendNode__FindKeyFrame_hook, &CAnimBlendNode__FindKeyFrame);

	CPatch::InlineHook(g_libGTASA, 0x457B60, &CPlayerPed__ControlButtonSprint_hook, &CPlayerPed__ControlButtonSprint);
	CPatch::InlineHook(g_libGTASA, 0x510B08, &CVehicle__UsesSiren_hook, &CVehicle__UsesSiren);							 // Add custom siren to our vehicle
	CPatch::InlineHook(g_libGTASA, 0x3D5894, &CHud__DrawScriptText_hook, &CHud__DrawScriptText);						 // TextDraw render
	CPatch::InlineHook(g_libGTASA, 0x276510, &CWidgetButtonEnterCar_Draw_hook, &CWidgetButtonEnterCar_Draw);			 // widgets
	CPatch::InlineHook(g_libGTASA, 0x3C1BF8, &CWorld_ProcessPedsAfterPreRender_hook, &CWorld_ProcessPedsAfterPreRender); // attached objects
	CPatch::InlineHook(g_libGTASA, 0x395994, &CFileLoader__LoadObjectInstance_hook, &CFileLoader__LoadObjectInstance);	 // remove building
	CPatch::InlineHook(g_libGTASA, 0x391E20, &CObject__Render_hook, &CObject__Render);
	CPatch::InlineHook(g_libGTASA, 0x258910, &GetTexture_hook, &GetTexture_orig);
	CPatch::InlineHook(g_libGTASA, 0x3AC114, &CPlayerInfo__FindObjectToSteal_hook, &CPlayerInfo__FindObjectToSteal);
	CPatch::InlineHook(g_libGTASA, 0x335CC0, &CClumpModelInfo_GetFrameFromId_hook, &CClumpModelInfo_GetFrameFromId);
	CPatch::InlineHook(g_libGTASA, 0x1BC314, &RLEDecompress_hook, &RLEDecompress);
	CPatch::InlineHook(g_libGTASA, 0x1A8530, &RenderQueue__ProcessCommand_hook, &RenderQueue__ProcessCommand);
	CPatch::InlineHook(g_libGTASA, 0x1B9D74, &_rwFreeListFreeReal_hook, &_rwFreeListFreeReal);
	CPatch::InlineHook(g_libGTASA, 0x274AB4, &CWidgetButton__Update_hook, &CWidgetButton__Update);
	CPatch::InlineHook(g_libGTASA, 0x274218, &CWidget__IsTouched_hook, &CWidget__IsTouched);
	CPatch::InlineHook(g_libGTASA, 0x2C0304, &CDarkel__RegisterCarBlownUpByPlayer_hook, &CDarkel__RegisterCarBlownUpByPlayer);
	CPatch::InlineHook(g_libGTASA, 0x2C072C, &CDarkel__ResetModelsKilledByPlayer_hook, &CDarkel__ResetModelsKilledByPlayer);
	CPatch::InlineHook(g_libGTASA, 0x2C0758, &CDarkel__QueryModelsKilledByPlayer_hook, &CDarkel__QueryModelsKilledByPlayer);
	CPatch::InlineHook(g_libGTASA, 0x2C0778, &CDarkel__FindTotalPedsKilledByPlayer_hook, &CDarkel__FindTotalPedsKilledByPlayer);
	CPatch::InlineHook(g_libGTASA, 0x2C0D04, &CDarkel__RegisterKillByPlayer_hook, &CDarkel__RegisterKillByPlayer);
	CPatch::InlineHook(g_libGTASA, 0x338CBC, &CVehicleModelInfo__SetEditableMaterials_hook, &CVehicleModelInfo__SetEditableMaterials);
	CPatch::InlineHook(g_libGTASA, 0x50DEF4, &CVehicle__ResetAfterRender_hook, &CVehicle__ResetAfterRender);
	CPatch::InlineHook(g_libGTASA, 0x3B67F8, &CRopes__Update_hook, &CRopes__Update);
	CPatch::InlineHook(g_libGTASA, 0x3986CC, &CGame__Process_hook, &CGame__Process);
	CPatch::InlineHook(g_libGTASA, 0x4D4A6C, &CAutomobile__ProcessEntityCollision_hook, &CAutomobile__ProcessEntityCollision);
	CPatch::InlineHook(g_libGTASA, 0x398334, &CGame__Shutdown_hook, &CGame__Shutdown);

	WriteMemory(g_libGTASA + 0x3DA86C,
				(uintptr_t) "\x80\xB4"
							"\x00\xAF"
							"\x1B\x4B"
							"\x7B\x44"
							"\x1B\x68",
				10);

	NOP(g_libGTASA + 0x3DA876, 3);

	// headlights color, wheel size, wheel align
	CPatch::InlineHook(g_libGTASA, 0x5466EC, &CShadows__StoreCarLightShadow_hook, &CShadows__StoreCarLightShadow);
	CPatch::InlineHook(g_libGTASA, 0x518EC4, &CVehicle__DoHeadLightBeam_hook, &CVehicle__DoHeadLightBeam);
	CPatch::InlineHook(g_libGTASA, 0x4E671C, &CAutomobile__PreRender_hook, &CAutomobile__PreRender);
	CPatch::InlineHook(g_libGTASA, 0x4DC6E8, &CAutomobile__UpdateWheelMatrix_hook, &CAutomobile__UpdateWheelMatrix);
	CPatch::InlineHook(g_libGTASA, 0x3E8D48, &CMatrix__Rotate_hook, &CMatrix__Rotate);
	CPatch::InlineHook(g_libGTASA, 0x3E8884, &CMatrix__SetScale_hook, &CMatrix__SetScale);
	CPatch::InlineHook(g_libGTASA, 0x46CEF4, &CTaskSimpleUseGun__RemoveStanceAnims_hook, &CTaskSimpleUseGun__RemoveStanceAnims);
	CPatch::InlineHook(g_libGTASA, 0x3DA86C, &CRadar__LimitRadarPoint_hook, &CRadar__LimitRadarPoint); // TO FIX
	CPatch::InlineHook(g_libGTASA, 0x5529FC, &CSprite2d__DrawBarChart_hook, &CSprite2d__DrawBarChart);
	CPatch::InlineHook(g_libGTASA, 0x5353B4, &CFont__PrintString_hook, &CFont__PrintString);
	CPatch::InlineHook(g_libGTASA, 0x55265C, &CSprite2d__Draw_hook, &CSprite2d__Draw);
	CPatch::InlineHook(g_libGTASA, 0x5529AC, &CSprite2d__DrawRect__hook, &CSprite2d__DrawRect); // fix crosshair
	CPatch::InlineHook(g_libGTASA, 0x27D8A8, &CWidgetPlayerInfo__DrawWanted_hook, &CWidgetPlayerInfo__DrawWanted);
	CPatch::InlineHook(g_libGTASA, 0x27CE88, &CWidgetPlayerInfo__DrawWeaponIcon_hook, &CWidgetPlayerInfo__DrawWeaponIcon);
	CPatch::InlineHook(g_libGTASA, 0x389D74, &CCam__Process_hook, &CCam__Process);
	CPatch::InlineHook(g_libGTASA, 0x3D6E6C, &CHud__Draw_hook, &CHud__Draw);
	CPatch::InlineHook(g_libGTASA, 0x39DC68, &CPad__CycleCameraModeDownJustDown_hook, &CPad__CycleCameraModeDownJustDown);
	CPatch::InlineHook(g_libGTASA, 0x391FE0, &CEntity__GetIsOnScreen_hook, &CEntity__GetIsOnScreen);
	CPatch::InlineHook(g_libGTASA, 0x31B164, &FxEmitterBP_c__Render_hook, &FxEmitterBP_c__Render);
	CPatch::InlineHook(g_libGTASA, 0x43A17C, &CPed__ProcessEntityCollision_hook, &CPed__ProcessEntityCollision);
	CPatch::InlineHook(g_libGTASA, 0x3AA3C0, &CPhysical__Add_hook, &CPhysical__Add);

	CPatch::InlineHook(g_libGTASA, 0x3D4ED8, &CHudRender__Draw_hook, &CHudRender__Draw);

	// new fixes
	CPatch::InlineHook(g_libGTASA, 0x1EEC90, &rxOpenGLDefaultAllInOneRenderCB_hook, &rxOpenGLDefaultAllInOneRenderCB);
	CPatch::InlineHook(g_libGTASA, 0x2CE788, &CObject__ProcessGarageDoorBehaviour_hook, &CObject__ProcessGarageDoorBehaviour);
	CPatch::InlineHook(g_libGTASA, 0x28AAAC, &CustomPipeRenderCB_hook, &CustomPipeRenderCB);
	CPatch::InlineHook(g_libGTASA, 0x1B1808, &_RwTextureDestroy_hook, &_RwTextureDestroy);
	CPatch::InlineHook(g_libGTASA, 0x194B04, &emu_ArraysDelete_hook, &emu_ArraysDelete);
	CPatch::InlineHook(g_libGTASA, 0x1BA580, &RwResourcesFreeResEntry_hook, &RwResourcesFreeResEntry);
	// CPatch::InlineHook(g_libGTASA, 0x1E3C54, &_RpMaterialDestroy_hook, &_RpMaterialDestroy);
	CPatch::InlineHook(g_libGTASA, 0x194B20, &emu_ArraysGetID_hook, &emu_ArraysGetID);
	CPatch::InlineHook(g_libGTASA, 0x50C5F8, &SetCompAlphaCB_hook, &SetCompAlphaCB);
	CPatch::InlineHook(g_libGTASA, 0x41EAB4, &CPlayerPedDataSaveStructure__Construct_hook, &CPlayerPedDataSaveStructure__Construct);
	CPatch::InlineHook(g_libGTASA, 0x1bdc3c, &CTextureDatabaseRuntime__GetEntry_hook, &CTextureDatabaseRuntime__GetEntry);
	CPatch::InlineHook(g_libGTASA, 0x368850, &CAudioEngine__Service_hook, &CAudioEngine__Service);
	CPatch::InlineHook(g_libGTASA, 0x35AC44, &CAEVehicleAudioEntity__GetAccelAndBrake_hook, &CAEVehicleAudioEntity__GetAccelAndBrake);
	CPatch::InlineHook(g_libGTASA, 0x29947C, &CCollision__ProcessVerticalLine_hook, &CCollision__ProcessVerticalLine);

	CPatch::InlineHook(g_libGTASA, 0x3B1778, &CRenderer__RenderEverythingBarRoads_hook, &CRenderer__RenderEverythingBarRoads);
	HookCPad();
}
