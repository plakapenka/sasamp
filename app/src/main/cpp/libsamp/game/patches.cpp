#include "../main.h"
#include "common.h"

char* PLAYERS_REALLOC = nullptr;
#include "../CSettings.h"
#include "..//CDebugInfo.h"
#include "game.h"
#include "CExtendedCarColors.h"
#include "util/patch.h"

extern CSettings* pSettings;

void WriteUnVerified0();
void InitInGame();

VehicleAudioPropertiesStruct VehicleAudioProperties[20000];

void readVehiclesAudioSettings() 
{

	char vehicleModel[50];
	int16_t pIndex = 0;

	FILE* pFile;

	char line[300];

	// Zero VehicleAudioProperties
	memset(VehicleAudioProperties, 0x00, sizeof(VehicleAudioProperties));

	VehicleAudioPropertiesStruct CurrentVehicleAudioProperties;

	memset(&CurrentVehicleAudioProperties, 0x0, sizeof(VehicleAudioPropertiesStruct));

	char buffer[0xFF];
	sprintf(buffer, "%sSAMP/vehicleAudioSettings.cfg", g_pszStorage);
	pFile = fopen(buffer, "r");
	if (!pFile)
	{
		//Log("Cannot read vehicleAudioSettings.cfg");
		return;
	}

	// File exists
	while (fgets(line, sizeof(line), pFile))
	{
		if (strncmp(line, ";the end", 8) == 0)
			break;

		if (line[0] == ';')
			continue;

		sscanf(line, "%s %d %d %d %d %f %f %d %f %d %d %d %d %f",
			vehicleModel,
			&CurrentVehicleAudioProperties.VehicleType,
			&CurrentVehicleAudioProperties.EngineOnSound,
			&CurrentVehicleAudioProperties.EngineOffSound,
			&CurrentVehicleAudioProperties.field_4,
			&CurrentVehicleAudioProperties.field_5,
			&CurrentVehicleAudioProperties.field_6,
			&CurrentVehicleAudioProperties.HornTon,
			&CurrentVehicleAudioProperties.HornHigh,
			&CurrentVehicleAudioProperties.DoorSound,
			&CurrentVehicleAudioProperties.RadioNum,
			&CurrentVehicleAudioProperties.RadioType,
			&CurrentVehicleAudioProperties.field_14,
			&CurrentVehicleAudioProperties.field_16);

		int result = ((int (*)(const char* thiz, int16_t* a2))(g_libGTASA + 0x00336110 + 1))(vehicleModel, &pIndex);
		memcpy(&VehicleAudioProperties[pIndex-400], &CurrentVehicleAudioProperties, sizeof(VehicleAudioPropertiesStruct));


	}

	fclose(pFile);
}

void DisableAutoAim()
{
    // CPlayerPed::FindWeaponLockOnTarget
	CPatch::WriteMemory(g_libGTASA + 0x4568B0, (uintptr_t)"\x00\x20\xF7\x46", 4);
    // CPlayerPed::FindNextWeaponLockOnTarget
	CPatch::WriteMemory(g_libGTASA + 0x4590E4, (uintptr_t)"\x00\x20\xF7\x46", 4);
	CPatch::WriteMemory(g_libGTASA + 0x438DB4, (uintptr_t)"\x00\x20\xF7\x46", 4);

}

int test_pointsArray[1000];
int test_pointersLibArray[1000];

extern void (*ANDRunThread)(void* a1);
void ANDRunThread_hook(void* a1);

void ApplyPatches_level0()
{
	Log("ApplyPatches_level0");

	PLAYERS_REALLOC = (( char* (*)(int))(g_libGTASA + 0x179B40))(404*MAX_PLAYERS);
	CPatch::UnFuck(g_libGTASA + 0x5D021C);
	*(char**)(g_libGTASA + 0x5D021C) = PLAYERS_REALLOC;
	Log("CWorld::Player new address = 0x%X", PLAYERS_REALLOC);

	// 3 touch begin
	CPatch::UnFuck(g_libGTASA + 0x005D1E8C);
	memset(test_pointsArray, 0, 999 * sizeof(int));
	*(int**)(g_libGTASA + 0x005D1E8C) = &test_pointsArray[0];

	CPatch::UnFuck(g_libGTASA + 0x0063D4F0);
	memset(test_pointersLibArray, 0, 999 * sizeof(int));
	*(int**)(g_libGTASA + 0x0063D4F0) = &test_pointersLibArray[0];

	CPatch::WriteMemory(g_libGTASA + 0x00238232, (uintptr_t)"\x03\x20", 2);
	CPatch::WriteMemory(g_libGTASA + 0x0025C522, (uintptr_t)"\x02\x2C", 2);
	// 3 touch end

	// col links limits!!!
	CPatch::WriteMemory(g_libGTASA + 0x1859FC, (uintptr_t)"\x01\x22", 2);
	//sizeof(CCollisionLink) = 12 (0xC)
	// we need 200 col links(MALO LI!!!)
	CPatch::WriteMemory(g_libGTASA + 0x0029554A, (uintptr_t)"\x4f\xf4\x61\x60", 4); // allocate memory to 300 * sizeof(CCollisionLink)
	CPatch::WriteMemory(g_libGTASA + 0x00295556, (uintptr_t)"\x4f\xf4\x5b\x62", 4); // BUT MAKE INITIALIZED ONLY 292 DUE TO SHIT ARM ASM!! (292 * sizeof(CCollisionLink)

	// col links limits end

	CExtendedCarColors::ApplyPatches_level0();

	uint8_t fps = 90;
	if (pSettings)
	{
		fps = pSettings->GetReadOnly().iFPS;
	}

	CPatch::WriteMemory(g_libGTASA + 0x463FE8, (uintptr_t)& fps, 1);
	CPatch::WriteMemory(g_libGTASA + 0x56C1F6, (uintptr_t)& fps, 1);
	CPatch::WriteMemory(g_libGTASA + 0x56C126, (uintptr_t)& fps, 1);
	CPatch::WriteMemory(g_libGTASA + 0x95B074, (uintptr_t)& fps, 1);

	CPatch::SetUpHook(g_libGTASA + 0x0023768C, (uintptr_t)ANDRunThread_hook, (uintptr_t*)& ANDRunThread);

    DisableAutoAim();

	//UnFuck(g_libGTASA + 0x0056CA68);
	//*(uint8_t*)(g_libGTASA + 0x0056CA68) = 1;

	//NOP(g_libGTASA + 0x001A869C, 2);

	// nop random pickups
	CPatch::NOP(g_libGTASA + 0x00402472, 2);
	CPatch::NOP(g_libGTASA + 0x003E1AF0, 2);

	// osMutexStuff
	CPatch::WriteMemory(g_libGTASA + 0x001A7ECE, (uintptr_t)"\x4F\xF0\x01\x00\x00\x46", 6);

	// CdStreamInit(6);
	CPatch::WriteMemory(g_libGTASA + 0x3981EC, (uintptr_t)"\x06\x20", 2);

	uintptr_t g_libSCAnd = FindLibrary("libSCAnd.so");
	if (g_libSCAnd)
	{
		CPatch::UnFuck(g_libSCAnd + 0x001E1738);
		CPatch::UnFuck(g_libSCAnd + 0x001E16DC);
		CPatch::UnFuck(g_libSCAnd + 0x001E080C);
		strcpy((char*)(g_libSCAnd + 0x001E16DC), "com/rockstargames/hal/andViewManager");
		strcpy((char*)(g_libSCAnd + 0x001E1738), "staticExitSocialClub");
		strcpy((char*)(g_libSCAnd + 0x001E080C), "()V");
	}

	// fix hook crash
//	CPatch::WriteMemory(g_libGTASA + 0x00336618, (uintptr_t)"\x4F\xF4\x6A\x71", 4);
//	CPatch::WriteMemory(g_libGTASA + 0x33661C, (uintptr_t)"\x1A\x4B", 2);
//	CPatch::WriteMemory(g_libGTASA + 0x00274AB4, (uintptr_t)"\x00\x46", 2);

	//WriteMemory(g_libGTASA + 0x001A7EB6, (uintptr_t)"\x00\x23", 2); // MOVS            R3, #0
	//WriteMemory(g_libGTASA + 0x001A7ED2, (uintptr_t)"\x00\x20", 2); // MOVS				R0, #0

	// From CGame::Init2
	CPatch::NOP(g_libGTASA + 0x00408706, 2); // CClothes::Init
	CPatch::RET(g_libGTASA + 0x003F1030); // CClothes::RebuildPlayer

	CPatch::NOP(g_libGTASA + 0x004087E8, 2); // CGlass::Init
	CPatch::NOP(g_libGTASA + 0x004087EC, 2); // CGarages::Init_AfterRestart
	CPatch::NOP(g_libGTASA + 0x00408800, 2); // CGangs::Initialise
	//CPatch::NOP(g_libGTASA + 0x00408812, 2); // CClock::Initialise(uint)
	CPatch::NOP(g_libGTASA + 0x00408816, 2); // CHeli::InitHelis(void)
//	CPatch::NOP(g_libGTASA + 0x0040881A, 2); // CCranes::InitCranes(void)

	WriteUnVerified0();
}

struct _ATOMIC_MODEL
{
	uintptr_t func_tbl;
	char data[56];
};
struct _ATOMIC_MODEL *ATOMIC_MODELS;

void ApplyShadowPatch(){

	// nop calling CRealTimeShadowManager::ReturnRealTimeShadow from ~CPhysical
	CPatch::NOP(g_libGTASA + 0x3A019C, 2);

	CPatch::NOP(g_libGTASA + 0x2661DA, 2); // Display - Shadows ( настройки )

	//shadow CRealTimeShadowManager::Update
	CPatch::RET(g_libGTASA + 0x541AC4);

	// Fix shadows crash (thx R*)
	CPatch::NOP(g_libGTASA + 0x0039B2C4, 2); // CMirrors::BeforeMainRender(void)

}

void ApplyPatches()
{
	Log("Installing patches..");

	// ped ussles
	CPatch::RET(g_libGTASA + 0x003BFFBC); // CWanted::Initialise
//	CPatch::RET(g_libGTASA + 0x3AD84C); // CPlayerPedData::AllocateData
//	CPatch::RET(g_libGTASA + 0x3F0D8C); // CPedClothesDesc::SetTextureAndModel

	ApplyShadowPatch();

	// allocate Atomic models pool
	ATOMIC_MODELS = new _ATOMIC_MODEL[120000];
	for (int i = 0; i < 120000; i++)
	{
		// CBaseModelInfo::CBaseModelInfo
		((void(*)(_ATOMIC_MODEL*))(g_libGTASA + 0x33559C + 1))(&ATOMIC_MODELS[i]);
		ATOMIC_MODELS[i].func_tbl = g_libGTASA + 0x5C6C68;
		memset(ATOMIC_MODELS[i].data, 0, sizeof(ATOMIC_MODELS->data));
	}

	CPatch::Write<uint32_t>(g_libGTASA + 0x8E87E4, 1);

	CDebugInfo::ApplyDebugPatches();

//	NOP(g_libGTASA + 0x0049052A, 42); // братки залазят в тачку с тобой

	// nop check CWorld::FindPlayerSlotWithPedPointer (fuck*** R*)
	// just left mov r4, r0 and ldr r1 [pc, shit]
	CPatch::NOP(g_libGTASA + 0x0045A4C8, 11);
	CPatch::NOP(g_libGTASA + 0x0045A4E0, 3);

	CPatch::NOP(g_libGTASA + 0x50FF68, 2); // crash FindPlayerPed

	//UnFuck(g_libGTASA + 0x008B8018);
	//*(uint8_t*)(g_libGTASA + 0x008B8018) = 1;

	// CAudioEngine::StartLoadingTune звук загрузочного экрана
	CPatch::NOP(g_libGTASA + 0x56C150, 2);

	// Nop CFileLoader::loadPickups
	CPatch::NOP(g_libGTASA + 0x402472, 2);

	// DefaultPCSaveFileName
	char* DefaultPCSaveFileName = (char*)(g_libGTASA + 0x60EAE8);
	memcpy((char*)DefaultPCSaveFileName, "GTASAMP", 8);

	uint8_t fps = pSettings->GetReadOnly().iFPS;
	CPatch::WriteMemory(g_libGTASA + 0x463FE8, (uintptr_t)& fps, 1);
	CPatch::WriteMemory(g_libGTASA + 0x56C1F6, (uintptr_t)& fps, 1);
	CPatch::WriteMemory(g_libGTASA + 0x56C126, (uintptr_t)& fps, 1);
	CPatch::WriteMemory(g_libGTASA + 0x95B074, (uintptr_t)& fps, 1);

	// CVehicleRecording::Load
	CPatch::RET(g_libGTASA + 0x2DC8E0);

	//CPatch::WriteMemory(g_libGTASA + 0x00458D68, (uintptr_t)"\x00\x46\x00\x46", 4); // cFire::extinguish nopped
	CPatch::NOP(g_libGTASA + 0x458D68, 2);	// CFire::Extinguish from CPlayerPed::SetInitialState

	CPatch::NOP(g_libGTASA + 0x0054D720, 2); //CMotionBlurStreaks::Update
	CPatch::NOP(g_libGTASA + 0x00398A4C, 2); // CEntryExitManager::Update
	CPatch::NOP(g_libGTASA + 0x0039893E, 2); // CGangWars::Update(void)
	CPatch::NOP(g_libGTASA + 0x0039B394, 2); // CCamera::RenderMotionBlur(void)

	CPatch::RET(g_libGTASA + 0x3474E0);	// CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire
	CPatch::RET(g_libGTASA + 0x284BB8); 	// CWidgetRegionSteeringSelection::Draw
	CPatch::RET(g_libGTASA + 0x3BF8B4);	// CPlaceName::Process
	CPatch::RET(g_libGTASA + 0x4BDB18);	// CTaskSimplePlayerOnFoot::PlayIdleAnimations
	CPatch::RET(g_libGTASA + 0x494FE4);	// CCarEnterExit::SetPedInCarDirect
	CPatch::RET(g_libGTASA + 0x3DA500); // CRadar::DrawLgegend
	CPatch::RET(g_libGTASA + 0x3DBB30); // CRadar::AddBlipToLegendList
	CPatch::RET(g_libGTASA + 0x5366D0); // CGlass::Render
	//CPatch::RET(g_libGTASA + 0x5286EC); // CBirds::Render
	CPatch::RET(g_libGTASA + 0x4DD00C); // CAutomobile::CustomCarPlate_BeforeRenderingStart
	CPatch::RET(g_libGTASA + 0x4DD038); // CAutomobile::CustomCarPlate_AfterRenderingStop
	CPatch::RET(g_libGTASA + 0x36E77C); // CCamera::CamShake
	CPatch::RET(g_libGTASA + 0x392A98); // CEntity::PreRenderForGlassWindow
	CPatch::RET(g_libGTASA + 0x315310); // CStuntJumpManager::Update
	CPatch::RET(g_libGTASA + 0x54DCF4); // CMirrors::RenderReflBuffer
	CPatch::RET(g_libGTASA + 0x3B67F8); // CRopes::Update
	CPatch::RET(g_libGTASA + 0x4FDC78); // CHeli::UpdateHelis
	CPatch::RET(g_libGTASA + 0x3D541C); // CHud::DrawVehicleName
	CPatch::RET(g_libGTASA + 0x3D62FC); // CHud::DrawBustedWastedMessage // ПОТРАЧЕНО
	CPatch::RET(g_libGTASA + 0x3D4244); // CHud::SetHelpMessage
	CPatch::RET(g_libGTASA + 0x3D42A8); // CHud::SetHelpMessageStatUpdate
	CPatch::RET(g_libGTASA + 0x2BCD0C); // CCheat::ProcessCheatMenu
	CPatch::RET(g_libGTASA + 0x2BCDB4); // CCheat::ProcessCheats
	CPatch::RET(g_libGTASA + 0x2BC08C); // CCheat::AddToCheatString
	CPatch::RET(g_libGTASA + 0x2BC24C); // CCheat::DoCheats
	CPatch::RET(g_libGTASA + 0x2BA710); // CCheat::WeaponCheat1
	CPatch::RET(g_libGTASA + 0x2BA92C); // CCheat::WeaponCheat2
	CPatch::RET(g_libGTASA + 0x2BAB20); // CCheat::WeaponCheat3
	CPatch::RET(g_libGTASA + 0x2BA68C); // CCheat::WeaponCheat4
	CPatch::RET(g_libGTASA + 0x2C9EEC); // CGarages::TriggerMessage
	CPatch::RET(g_libGTASA + 0x3D4EAC); // CHud_DrawVitalStats

	CPatch::NOP(g_libGTASA + 0x454A88, 2);  // CCamera::ClearPlayerWeaponMode from CPlayerPed::ClearWeaponTarget
	CPatch::NOP(g_libGTASA + 0x2FEE76, 2);	// CGarages::RespraysAreFree = true in CRunningScript::ProcessCommands800To899
	CPatch::NOP(g_libGTASA + 0x50FF64, 2);	// skip playerGifts from CVehicle::SetDriver
	CPatch::NOP(g_libGTASA + 0x39840A, 2);	// CStreaming::Shutdown from CGame::Shutdown
	CPatch::NOP(g_libGTASA + 0x4874E0, 5);  // CPedIntelligence::AddTaskPrimaryMaybeInGroup from CTaskComplexEnterCar::CreateNextSubTask
	CPatch::NOP(g_libGTASA + 0x2E1EDC, 2); 	// CUpsideDownCarCheck::UpdateTimers from CTheScripts::Process

	CPatch::WriteMemory(g_libGTASA + 0x2C3868, "\x00\x20\x70\x47", 4); 					// CGameLogic::IsCoopGameGoingOn
	CPatch::WriteMemory(g_libGTASA + 0x1A7EF2, "\x4F\xF4\x00\x10\x4F\xF4\x80\x00", 8); 	// RenderQueue::RenderQueue
	CPatch::WriteMemory(g_libGTASA + 0x1A7F32, "\x4F\xF4\x00\x10\x4F\xF4\x80\x00", 8); 	// RenderQueue::RenderQueue

	CPatch::WriteMemory(g_libGTASA + 0x45FC20, "\x4F\xF0\x00\x00\xF7\x46", 6);			// CPopulation::AddToPopulation
	CPatch::WriteMemory(g_libGTASA + 0x293218, "\x01\x20\x70\x47", 4); 					// CStreaming::RemoveLoadedVehicle
	CPatch::WriteMemory(g_libGTASA + 0x1BDD4A, "\x10\x46\xA2\xF1\x04\x0B", 6); 			// TextureDatabaseRuntime::LoadFullTexture
	CPatch::WriteMemory(g_libGTASA + 0x3E1A2C, "\x67\xE0", 2);							// Interior_c::Init
	CPatch::WriteMemory(g_libGTASA + 0x27D8D0, "\x4F\xF0\x00\x08", 4);					// CWidgetPlayerInfo::DrawWanted
	CPatch::WriteMemory(g_libGTASA + 0x3C5B58, "\x02\x21", 2); 							// CWorld::Process
	CPatch::NOP(g_libGTASA + 0x45F74C, 16); // CPopulation::AddPedInCar - skip some wtf stuff

	CPatch::NOP(g_libGTASA + 0x00454950, 17); // CAnimManager::RemoveAnimBlockRef

	// CPed pool (old: 140, new: 210)
	/* 	MOVW R0, #0x5EC8
		MOVT R0, #6 */
	CPatch::WriteMemory(g_libGTASA + 0x3AF2D0, (uintptr_t)"\x45\xF6\xC8\x60\xC0\xF2\x06\x00", 8); // MOV  R0, #0x65EC8 | size=0x7C4 (old: 0x43F30)
	CPatch::WriteMemory(g_libGTASA + 0x3AF2DE, (uintptr_t)"\xD2\x20", 2); // MOVS R0, #0xD2
	CPatch::WriteMemory(g_libGTASA + 0x3AF2E4, (uintptr_t)"\xD2\x22", 2); // MOVS R2, #0xD2
	CPatch::WriteMemory(g_libGTASA + 0x3AF310, (uintptr_t)"\xD2\x2B", 2); // CMP  R3, #0xD2

	// CPedIntelligence pool (old: 140, new: 210)
	// movw r0, #0x20B0
	// movt r0, #2
	// nop
	CPatch::WriteMemory(g_libGTASA + 0x3AF7E6, (uintptr_t)"\x42\xF2\xB0\x00\xC0\xF2\x02\x00\x00\x46", 10); // MOVS R0, #0x220B0 | size=0x298 (old: 0x16B20)
	CPatch::WriteMemory(g_libGTASA + 0x3AF7F6, (uintptr_t)"\xD2\x20", 2); // MOVS R0, #0xD2
	CPatch::WriteMemory(g_libGTASA + 0x3AF7FC, (uintptr_t)"\xD2\x22", 2); // MOVS R2, #0xD2
	CPatch::WriteMemory(g_libGTASA + 0x3AF824, (uintptr_t)"\xD2\x2B", 2); // CMP  R3, 0xD2

	// Task pool (old: 500, new: 1524 (1536))
	CPatch::WriteMemory(g_libGTASA + 0x3AF4EA, (uintptr_t)"\x4F\xF4\x40\x30", 4); // MOV.W R0, #30000 | size = 0x80 (old: 0xFA00)
	CPatch::WriteMemory(g_libGTASA + 0x3AF4F4, (uintptr_t)"\x4F\xF4\xC0\x60", 4); // MOV.W R0, #0x600
	CPatch::WriteMemory(g_libGTASA + 0x3AF4FC, (uintptr_t)"\x4F\xF4\xC0\x62", 4); // MOV.W R2, #0x600
	CPatch::WriteMemory(g_libGTASA + 0x3AF52A, (uintptr_t)"\xB3\xF5\xC0\x6F", 4); // CMP.W R3, #0x600

	// TxdStore pool (old: 5000, new: 20000)
	CPatch::WriteMemory(g_libGTASA + 0x0055BA9A, (uintptr_t)"\x4f\xf4\xb8\x50\xc0\xf2\x11\x00", 8); //  MOV.W	R0, #0x1700 | MOVT	R0, #0x11
	CPatch::WriteMemory(g_libGTASA + 0x0055BAA8, (uintptr_t)"\x44\xf6\x20\x60", 4); // MOVW	R0, #0x4E20
	CPatch::WriteMemory(g_libGTASA + 0x0055BAB0, (uintptr_t)"\x44\xf6\x20\x62", 4); // MOVW	R2, #0x4E20

	// Event pool (old: 200, new: 512)

	// Object pool (old: 350, new: 2992)
	CPatch::WriteMemory(g_libGTASA + 0x003AF3D6, (uintptr_t)"\x4f\xf4\x7e\x40\xc0\xf2\x12\x00", 8); // MOV.W	R0, #0x5900 | MOVT	R0, #0x6
	CPatch::WriteMemory(g_libGTASA + 0x003AF3E4, (uintptr_t)"\x4f\xf4\x3b\x60", 4); // MOV.W R0, #0xBB0
	CPatch::WriteMemory(g_libGTASA + 0x003AF3EC, (uintptr_t)"\x4f\xf4\x3b\x62", 4); // MOV.W R2, #0xBB0
	CPatch::WriteMemory(g_libGTASA + 0x003AF41A, (uintptr_t)"\xb3\xf5\x3b\x6f", 4); // CMP.W R3, #0xBB0

	// Building pool (old: 14000, new: 29585)
	CPatch::WriteMemory(g_libGTASA + 0x003AF378, (uintptr_t)"\x4F\xF4\x90\x40\xC0\xF2\x19\x00", 8); // mov.w r0, #0x4800 movt r0, #0x19
	CPatch::WriteMemory(g_libGTASA + 0x003AF386, (uintptr_t)"\x47\xF2\x91\x30", 4); // mov r0, #0x7391
	CPatch::WriteMemory(g_libGTASA + 0x003AF38E, (uintptr_t)"\x47\xF2\x91\x32", 4); //mov r2, #0x7391

	// Dummys pool (old: 3500, new: 4000)
	CPatch::WriteMemory(g_libGTASA + 0x003AF430, (uintptr_t)"\x4f\xf4\xd6\x40\xc0\xf2\x03\x00", 8); // MOV.W	R0, #0x6B00 | MOVT	R0, #0x3
	CPatch::WriteMemory(g_libGTASA + 0x003AF43E, (uintptr_t)"\x40\xf6\xa0\x70", 4); // MOVW            R0, #0xFA0
	CPatch::WriteMemory(g_libGTASA + 0x003AF446, (uintptr_t)"\x40\xf6\xa0\x72", 4); // MOVW            R2, #0xFA0

	// PtrNode Single pool (old: 75000, new: 100000)
	CPatch::WriteMemory(g_libGTASA + 0x003AF1BC, (uintptr_t)"\x4f\xf4\x54\x50\xc0\xf2\x0c\x00", 8); // MOV.W	R0, #0x3500 | MOVT	R0, #0xC
	CPatch::WriteMemory(g_libGTASA + 0x003AF1D6, (uintptr_t)"\x48\xf2\xa0\x62", 4); // MOVW	R2, #0x86A0
	CPatch::WriteMemory(g_libGTASA + 0x003AF1B0, (uintptr_t)"\x48\xf2\xa0\x66", 4); // MOVW	R6, #0x86A0
	CPatch::WriteMemory(g_libGTASA + 0x003AF1CA, (uintptr_t)"\x48\xf2\xa0\x60\xc0\xf2\x01\x00", 8); // MOV.W R0, #0x86A0 | MOVT R0, #1

	//PtrNode Double pool (old: 6000, new: 8000)
	CPatch::WriteMemory(g_libGTASA + 0x003AF21C, (uintptr_t)"\x4f\xf4\xee\x40\xc0\xf2\x01\x00", 8); // MOV.W	R0, #0x7700 | MOVT	R0, #1
	CPatch::WriteMemory(g_libGTASA + 0x003AF22A, (uintptr_t)"\x41\xf6\x40\x70", 4); // MOVW            R0, #8000
	CPatch::WriteMemory(g_libGTASA + 0x003AF232, (uintptr_t)"\x41\xf6\x40\x72", 4); // MOVW            R2, #8000

	// Entry Node Info pool (old: 500, new: 5120)
	CPatch::WriteMemory(g_libGTASA + 0x003AF27A, (uintptr_t)"\x4f\xf4\xc8\x30", 4); // MOV.W	R0, #0x19000 | size = 0x14
	CPatch::WriteMemory(g_libGTASA + 0x003AF284, (uintptr_t)"\x4f\xf4\xa0\x50", 4); // MOV.W R0, #0x1400
	CPatch::WriteMemory(g_libGTASA + 0x003AF28C, (uintptr_t)"\x4f\xf4\xa0\x52", 4); // MOV.W R2, #0x1400
	CPatch::WriteMemory(g_libGTASA + 0x003AF2BA, (uintptr_t)"\xb3\xf5\xa0\x5f", 4); // CMP.W R3, #0x1400

	// ColModel pool (old:10150, new: 32511)
	// mov r0, #0xCFD0
	// movt r0, #0x17
	CPatch::WriteMemory(g_libGTASA + 0x3AF48E, (uintptr_t)"\x4C\xF6\xD0\x70\xC0\xF2\x17\x00", 8); // MOV R0, #0x17CFD0 | size=0x30 (old: 0x76F20)
	CPatch::WriteMemory(g_libGTASA + 0x3AF49C, (uintptr_t)"\x47\xF6\xFF\x60", 4); // MOVW R0, #0x7EFF
	CPatch::WriteMemory(g_libGTASA + 0x3AF4A4, (uintptr_t)"\x47\xF6\xFF\x62", 4); // MOVW R2, #0x7EFF

	// VehicleStruct increase (0x32C*0x50 = 0xFDC0)
	CPatch::WriteMemory(g_libGTASA + 0x405338, (uintptr_t)"\x4F\xF6\xC0\x50", 4);	// MOV  R0, #0xFDC0
	CPatch::WriteMemory(g_libGTASA + 0x405342, (uintptr_t)"\x50\x20", 2);			// MOVS R0, #0x50
	CPatch::WriteMemory(g_libGTASA + 0x405348, (uintptr_t)"\x50\x22", 2);			// MOVS R2, #0x50
	CPatch::WriteMemory(g_libGTASA + 0x405374, (uintptr_t)"\x50\x2B", 2);			// CMP  R3, #0x50


	// stop assign 0xFFFFFF to beam color!
	CPatch::NOP(g_libGTASA + 0x00519116, 2);
	CPatch::NOP(g_libGTASA + 0x0051911C, 4);
	CPatch::NOP(g_libGTASA + 0x00519198, 4);
	CPatch::NOP(g_libGTASA + 0x00519198, 2);
	CPatch::NOP(g_libGTASA + 0x005191D0, 4);
	CPatch::NOP(g_libGTASA + 0x005191DC, 2);

	// Prevent another ped from answering when collision happens (or damage)
	CPatch::NOP(g_libGTASA + 0x327730, 2);
	CPatch::NOP(g_libGTASA + 0x32C488, 2);
	CPatch::NOP(g_libGTASA + 0x32DF94, 2);
	CPatch::NOP(g_libGTASA + 0x32E05E, 2);
	CPatch::NOP(g_libGTASA + 0x32E160, 2);
	CPatch::NOP(g_libGTASA + 0x32E1F2, 2);
	CPatch::NOP(g_libGTASA + 0x32E292, 2);
	CPatch::NOP(g_libGTASA + 0x32EFD0, 2);

	// CPed::Say
	CPatch::NOP(g_libGTASA + 0x43E288, 2);


}
void ApplyInGamePatches()
{
	
	Log("Installing patches (ingame)..");

	/* Разблокировка карты */
	// CTheZones::ZonesVisited[100]
	memset((void*)(g_libGTASA + 0x8EA7B0), 1, 100);
	// CTheZones::ZonesRevealed
	*(uint32_t*)(g_libGTASA + 0x8EA7A8) = 100;

	// CarCtl::GenerateRandomCars nulled from CGame::Process
	CPatch::NOP(g_libGTASA + 0x398A3A, 2);

	// CTheCarGenerators::Process nulled from CGame::Process
	CPatch::NOP(g_libGTASA + 0x398A34, 2);

	// множитель для MaxHealth
	CPatch::UnFuck(g_libGTASA + 0x3BAC68);
	*(float*)(g_libGTASA + 0x3BAC68) = 176.0f;
	// множитель для Armour
	CPatch::UnFuck(g_libGTASA + 0x27D884);
	*(float*)(g_libGTASA + 0x27D884) = 176.0;

	// CEntryExit::GenerateAmbientPeds nulled from CEntryExit::TransitionFinished
	CPatch::NOP(g_libGTASA + 0x2C2C22, 4);

	CPatch::NOP(g_libGTASA + 0x003D6FDC, 2);
	CPatch::NOP(g_libGTASA + 0x0026B214, 2);

	// CPlayerPed::CPlayerPed task fix
	CPatch::WriteMemory(g_libGTASA + 0x458ED1, (uintptr_t)"\xE0", 1);

	// ReapplyPlayerAnimation (хз зачем)
	CPatch::NOP(g_libGTASA + 0x45477E, 5);

	// radar draw blips
	CPatch::NOP(g_libGTASA + 0x3DCA90, 2);
	CPatch::NOP(g_libGTASA + 0x3DD4A4, 2);
	// CCamera::CamShake from CExplosion::AddExplosion
	CPatch::NOP(g_libGTASA + 0x55EFB8, 2);
	CPatch::NOP(g_libGTASA + 0x55F8F8, 2);

	// camera_on_actor path
	CPatch::UnFuck(g_libGTASA + 0x2F7B68);
	*(uint8_t*)(g_libGTASA + 0x2F7B6B) = 0xBE;

	// CPed::RemoveWeaponWhenEnteringVehicle (GetPlayerInfoForThisPlayerPed)
	//UnFuck(g_libGTASA + 0x434D94);
	CPatch::NOP(g_libGTASA + 0x434D94, 6);

	// CBike::ProcessAI
	CPatch::UnFuck(g_libGTASA + 0x4EE200);
	*(uint8_t*)(g_libGTASA + 0x4EE200) = 0x9B;

	// CWidgetPlayerInfo::DrawWanted
	//WriteMemory(g_libGTASA + 0x27D8D0, (uintptr_t)"\x4F\xF0\x00\x08", 4);

	// no vehicle audio processing
	CPatch::NOP(g_libGTASA + 0x4E31A6, 2);
	CPatch::NOP(g_libGTASA + 0x4EE7D2, 2);
	CPatch::NOP(g_libGTASA + 0x4F741E, 2);
	CPatch::NOP(g_libGTASA + 0x50AB4A, 2);
//
	CPatch::NOP(g_libGTASA + 0x00398768, 2); // nop police helis
	CPatch::NOP(g_libGTASA + 0x003987DC, 2);

	CPatch::NOP(g_libGTASA + 0x003688EC, 2); // nop ServiceAmbientGunFire


	// Disable in-game radio
	CPatch::NOP(g_libGTASA + 0x3688DA, 2);
	CPatch::NOP(g_libGTASA + 0x368DF0, 2);
	CPatch::NOP(g_libGTASA + 0x369072, 2);
	CPatch::NOP(g_libGTASA + 0x369168, 2);

	// Stop it trying to load tracks2.dat
	CPatch::NOP(g_libGTASA + 0x508F36, 2);

	// Stop it trying to load tracks4.dat
	CPatch::NOP(g_libGTASA + 0x508F54, 2);

    // Prevent cheats processing
	CPatch::NOP(g_libGTASA + 0x3987BA, 2);

	// мини-карта в меню
	CPatch::NOP(g_libGTASA+0x0026B504, 2); // убирает текст легенды карты
	CPatch::NOP(g_libGTASA+0x0026B514, 2); // убирает значки легенды
	CPatch::NOP(g_libGTASA+0x0026B49C, 2); // fix crash GetNextSpace

	// дефолт кнопка сесть в авто
	CPatch::NOP(g_libGTASA+0x00276512, 32); //CWidgetButtonEnterCar::Draw

	//todo CPlayerPed::ProcessAnimGroups in the end
}