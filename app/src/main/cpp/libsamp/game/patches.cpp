#include "../main.h"
#include "common.h"
#include "CPlayerInfoGta.h"
#include "../CSettings.h"
#include "..//CDebugInfo.h"
#include "game.h"
#include "CExtendedCarColors.h"
#include "util/patch.h"
#include "chatwindow.h"
#include "CWorld.h"

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
	CHook::RET(g_libGTASA + 0x4568B0);
//    // CPlayerPed::FindNextWeaponLockOnTarget
	CHook::RET(g_libGTASA + 0x4590E4);
	CHook::RET(g_libGTASA + 0x438DB4);

}

void ApplyFPSPatch(uint8_t fps)
{
	CHook::WriteMemory(g_libGTASA + 0x463FE8, (uintptr_t)& fps, 1);
	CHook::WriteMemory(g_libGTASA + 0x56C1F6, (uintptr_t)& fps, 1);
	CHook::WriteMemory(g_libGTASA + 0x56C126, (uintptr_t)& fps, 1);
	CHook::WriteMemory(g_libGTASA + 0x95B074, (uintptr_t)& fps, 1);

	Log("New fps limit = %d", fps);
}

int test_pointsArray[1000];
int test_pointersLibArray[1000];

extern void (*ANDRunThread)(void* a1);
void ANDRunThread_hook(void* a1);

void (*FindPlayerPed_orig)(int32_t Player);

void ApplyPatches_level0()
{
	Log("ApplyPatches_level0");

	CWorld::Players = new CPlayerInfoGta[MAX_PLAYERS]{nullptr};
	CHook::Write(g_libGTASA + 0x006783C0, CWorld::Players);

    CHook::Write(g_libGTASA + 0x00679B5C, &CWorld::PlayerInFocus);

//	CHook::Redirect(g_libGTASA, 0x0040B2D8, &CUtil::FindPlayerPed);
    CHook::Redirect(g_libGTASA, 0x0042CA1C, &CUtil::FindPlayerSlotWithPedPointer);

//	CWorld__Players = new CPlayerInfo[MAX_PLAYERS] {nullptr};
//	CHook::Write<CPlayerInfo*>(g_libGTASA + 0x006783C0, CWorld__Players);

 //

//
//	// 3 touch begin
//	CHook::UnFuck(g_libGTASA + 0x005D1E8C);
//	memset(test_pointsArray, 0, 999 * sizeof(int));
//	*(int**)(g_libGTASA + 0x005D1E8C) = &test_pointsArray[0];
//
//	CHook::UnFuck(g_libGTASA + 0x0063D4F0);
//	memset(test_pointersLibArray, 0, 999 * sizeof(int));
//	*(int**)(g_libGTASA + 0x0063D4F0) = &test_pointersLibArray[0];
//
//	CHook::WriteMemory(g_libGTASA + 0x00238232, (uintptr_t)"\x03\x20", 2);
//	CHook::WriteMemory(g_libGTASA + 0x0025C522, (uintptr_t)"\x02\x2C", 2);
//	// 3 touch end
//
//	// col links limits!!!
//	CHook::WriteMemory(g_libGTASA + 0x1859FC, (uintptr_t)"\x01\x22", 2);
//	//sizeof(CCollisionLink) = 12 (0xC)
//	// we need 200 col links(MALO LI!!!)
//	CHook::WriteMemory(g_libGTASA + 0x0029554A, (uintptr_t)"\x4f\xf4\x61\x60", 4); // allocate memory to 300 * sizeof(CCollisionLink)
//	CHook::WriteMemory(g_libGTASA + 0x00295556, (uintptr_t)"\x4f\xf4\x5b\x62", 4); // BUT MAKE INITIALIZED ONLY 292 DUE TO SHIT ARM ASM!! (292 * sizeof(CCollisionLink)
//
//	// col links limits end
//
//	CExtendedCarColors::ApplyPatches_level0();
//
	//CHook::SetUpHook(g_libGTASA + 0x0026BEA0, (uintptr_t)ANDRunThread_hook, (uintptr_t*)& ANDRunThread);
//
//    DisableAutoAim();
//
//	//UnFuck(g_libGTASA + 0x0056CA68);
//	//*(uint8_t*)(g_libGTASA + 0x0056CA68) = 1;
//
//	//NOP(g_libGTASA + 0x001A869C, 2);
//
//	// nop random pickups
//	CHook::NOP(g_libGTASA + 0x00402472, 2);
//	CHook::NOP(g_libGTASA + 0x003E1AF0, 2);

	// osMutexStuff
//	CHook::WriteMemory(g_libGTASA + 0x001A7ECE, (uintptr_t)"\x4F\xF0\x01\x00\x00\x46", 6);
//
//	uintptr_t g_libSCAnd = FindLibrary("libSCAnd.so");
//	if (g_libSCAnd)
//	{
//		CHook::UnFuck(g_libSCAnd + 0x001E1738);
//		CHook::UnFuck(g_libSCAnd + 0x001E16DC);
//		CHook::UnFuck(g_libSCAnd + 0x001E080C);
//		strcpy((char*)(g_libSCAnd + 0x001E16DC), "com/rockstargames/hal/andViewManager");
//		strcpy((char*)(g_libSCAnd + 0x001E1738), "staticExitSocialClub");
//		strcpy((char*)(g_libSCAnd + 0x001E080C), "()V");
//	}

	// fix hook crash
	// CHook::WriteMemory(g_libGTASA + 0x003860E2, "\x4F\xF4\x6A\x71", 4);
	//CHook::WriteMemory(g_libGTASA + 0x3860E4, "\x1A\x4B", 2);
	//CHook::WriteMemory(g_libGTASA + 0x00274AB4, "\x00\x46", 2);

	//WriteMemory(g_libGTASA + 0x001A7EB6, (uintptr_t)"\x00\x23", 2); // MOVS            R3, #0
	//WriteMemory(g_libGTASA + 0x001A7ED2, (uintptr_t)"\x00\x20", 2); // MOVS				R0, #0

	// From CGame::Init2
//	CHook::RET(g_libGTASA + 0x00457A28); // CClothes::Init
	CHook::RET(g_libGTASA + 0x00457580); // CClothes::RebuildPlayer

	//CHook::NOP(g_libGTASA + 0x004087E8, 2); // CGlass::Init
	//CHook::NOP(g_libGTASA + 0x004087EC, 2); // CGarages::Init_AfterRestart
//	CHook::RET(g_libGTASA + 0x0049EF08); // CGangs::Initialise
	//CHook::NOP(g_libGTASA + 0x00408812, 2); // CClock::Initialise(uint)
//	CHook::NOP(g_libGTASA + 0x00408816, 2); // CHeli::InitHelis(void)

	//CHook::NOP(g_libGTASA + 0x402472, 2);
	CHook::RET(g_libGTASA + 0x0046B5B8); // CFileLoader::LoadPickup

	//CHook::RET(g_libGTASA + 0x0036E88C); // CCamera::ClearPlayerWeaponMode не понятен эффект. крашит
//	CHook::NOP(g_libGTASA + 0x0040881A, 2); // CCranes::InitCranes(void)
}

void ApplyShadowPatch()
{
	CHook::RET(g_libGTASA + 0x541AA8); // CRealTimeShadowManager::ReturnRealTimeShadow from ~CPhysical
	CHook::NOP(g_libGTASA + 0x2661DA, 2); // Display - Shadows ( настройки )
	CHook::RET(g_libGTASA + 0x541AC4); //shadow CRealTimeShadowManager::Update
	CHook::RET(g_libGTASA + 0x543B04); // CShadows::RenderStaticShadows
	CHook::NOP(g_libGTASA + 0x0039B2C4, 2); // CMirrors::BeforeMainRender(void)
	CHook::RET(g_libGTASA + 0x5471F4); // CShadows::RenderStoredShadows
	CHook::RET(g_libGTASA + 0x545C04); // CShadows::StoreStaticShadow
	CHook::NOP(g_libGTASA + 0x39B2C0, 2);	// CRealTimeShadowManager::Update from Idle
}

void ApplyDefaultPlatePatch()
{
	//CCustomCarPlateMgr all meth
	// убрать дефолтные номера
	CHook::RET(g_libGTASA + 0x0052FF5C); //CCustomCarPlateMgr::GeneratePlateText

	CHook::RET(g_libGTASA + 0x0052FE90);
	CHook::RET(g_libGTASA + 0x00530098);
	CHook::RET(g_libGTASA + 0x00530104);
	CHook::RET(g_libGTASA + 0x0053012C);
	CHook::RET(g_libGTASA + 0x00530158);
	CHook::RET(g_libGTASA + 0x00530190);

	CHook::RET(g_libGTASA + 0x0053021C);
	CHook::RET(g_libGTASA + 0x0053038C);
	CHook::RET(g_libGTASA + 0x00530420);

	CHook::RET(g_libGTASA + 0x0050E37C);
}

void ApplyPatches()
{
	Log("Installing patches..");

	// CdStreamInit(6);
	CHook::WriteMemory(g_libGTASA + 0x003F3698, (uintptr_t)"\x06\x20", 2);

	CHook::RET(g_libGTASA + 0x004CF2DC); // CPopulation::AddPed
	CHook::RET(g_libGTASA + 0x003E33C8); // CClock::Update
    CHook::RET(g_libGTASA + 0x0056E3C0); // CTheCarGenerators::Process()
    CHook::RET(g_libGTASA + 0x002E833C); // CCarCtrl::GenerateRandomCars()
    CHook::RET(g_libGTASA + 0x0040B078); // CPlayerInfo::FindObjectToSteal
    CHook::RET(g_libGTASA + 0x00306F40); // CEntryExit::GenerateAmbientPeds
    CHook::RET(g_libGTASA + 0x002BCCF8);// Весь худ одной строкой?

	CHook::WriteMemory(g_libGTASA + 0x00427D5C, "\x02\x21", 2);

//	ApplyShadowPatch();
//    ApplyDefaultPlatePatch();
//
//	//CHook::Write<uint32_t>(g_libGTASA + 0x8E87E4, 1);
//
//	CDebugInfo::ApplyDebugPatches();
//
////	NOP(g_libGTASA + 0x0049052A, 42); // братки залазят в тачку с тобой
//
//	// nop check CWorld::FindPlayerSlotWithPedPointer (fuck*** R*)
//	// just left mov r4, r0 and ldr r1 [pc, shit]
//	CHook::NOP(g_libGTASA + 0x0045A4C8, 11);
//	CHook::NOP(g_libGTASA + 0x0045A4E0, 3);
//
//	//UnFuck(g_libGTASA + 0x008B8018);
//	//*(uint8_t*)(g_libGTASA + 0x008B8018) = 1;
//
//	// CAudioEngine::StartLoadingTune звук загрузочного экрана
//	CHook::NOP(g_libGTASA + 0x56C150, 2);
//
//	// DefaultPCSaveFileName
//	char* DefaultPCSaveFileName = (char*)(g_libGTASA + 0x60EAE8);
//	memcpy((char*)DefaultPCSaveFileName, "GTASAMP", 8);
//
//	// live russia
//	CHook::RET(g_libGTASA + 0x0051DEC4);			// живность в воде WaterCreatureManager_c::Update

//	CHook::NOP(g_libGTASA + 0x0039ADE6, 2);	// CCoronas::RenderSunReflection crash
//	CHook::NOP(g_libGTASA + 0x0051018A, 2);	// не давать ган при выходе из тачки 	( клюшка, дробовик and etc )
//	CHook::NOP(g_libGTASA + 0x005101A6, 2);	// не давать ган при выходе из тачки	( клюшка, дробовик and etc )
//
//	// CVehicleRecording::Load
//	CHook::RET(g_libGTASA + 0x2DC8E0);
//
//	//CHook::WriteMemory(g_libGTASA + 0x00458D68, (uintptr_t)"\x00\x46\x00\x46", 4); // cFire::extinguish nopped
//	CHook::NOP(g_libGTASA + 0x458D68, 2);	// CFire::Extinguish from CPlayerPed::SetInitialState
//
//	CHook::NOP(g_libGTASA + 0x0054D720, 2); //CMotionBlurStreaks::Update
//	CHook::NOP(g_libGTASA + 0x00398A4C, 2); // CEntryExitManager::Update
//	CHook::NOP(g_libGTASA + 0x0039893E, 2); // CGangWars::Update(void)
//	CHook::NOP(g_libGTASA + 0x0039B394, 2); // CCamera::RenderMotionBlur(void)
//
//	CHook::RET(g_libGTASA + 0x3474E0);	// CAEGlobalWeaponAudioEntity::ServiceAmbientGunFire
//	CHook::RET(g_libGTASA + 0x284BB8); 	// CWidgetRegionSteeringSelection::Draw
//	CHook::RET(g_libGTASA + 0x3BF8B4);	// CPlaceName::Process
//	CHook::RET(g_libGTASA + 0x4BDB18);	// CTaskSimplePlayerOnFoot::PlayIdleAnimations
//	CHook::RET(g_libGTASA + 0x494FE4);	// CCarEnterExit::SetPedInCarDirect
//	CHook::RET(g_libGTASA + 0x3DA500); // CRadar::DrawLgegend
//	CHook::RET(g_libGTASA + 0x3DBB30); // CRadar::AddBlipToLegendList
//	CHook::RET(g_libGTASA + 0x5366D0); // CGlass::Render
//	//CHook::RET(g_libGTASA + 0x5286EC); // CBirds::Render
//	CHook::RET(g_libGTASA + 0x4DD00C); // CAutomobile::CustomCarPlate_BeforeRenderingStart
//	CHook::RET(g_libGTASA + 0x4DD038); // CAutomobile::CustomCarPlate_AfterRenderingStop
//	CHook::RET(g_libGTASA + 0x36E77C); // CCamera::CamShake
//	CHook::RET(g_libGTASA + 0x392A98); // CEntity::PreRenderForGlassWindow
//	CHook::RET(g_libGTASA + 0x315310); // CStuntJumpManager::Update
//	CHook::RET(g_libGTASA + 0x54DCF4); // CMirrors::RenderReflBuffer
//	CHook::RET(g_libGTASA + 0x3B67F8); // CRopes::Update
//	CHook::RET(g_libGTASA + 0x4FDC78); // CHeli::UpdateHelis
//	CHook::RET(g_libGTASA + 0x3D541C); // CHud::DrawVehicleName
//	CHook::RET(g_libGTASA + 0x3D62FC); // CHud::DrawBustedWastedMessage // ПОТРАЧЕНО
//	CHook::RET(g_libGTASA + 0x3D4244); // CHud::SetHelpMessage
//	CHook::RET(g_libGTASA + 0x3D42A8); // CHud::SetHelpMessageStatUpdate
//	CHook::RET(g_libGTASA + 0x2BCD0C); // CCheat::ProcessCheatMenu
//	CHook::RET(g_libGTASA + 0x2BCDB4); // CCheat::ProcessCheats
//	CHook::RET(g_libGTASA + 0x2BC08C); // CCheat::AddToCheatString
//	CHook::RET(g_libGTASA + 0x2BC24C); // CCheat::DoCheats
//	CHook::RET(g_libGTASA + 0x2BA710); // CCheat::WeaponCheat1
//	CHook::RET(g_libGTASA + 0x2BA92C); // CCheat::WeaponCheat2
//	CHook::RET(g_libGTASA + 0x2BAB20); // CCheat::WeaponCheat3
//	CHook::RET(g_libGTASA + 0x2BA68C); // CCheat::WeaponCheat4
//	CHook::RET(g_libGTASA + 0x2C9EEC); // CGarages::TriggerMessage
//	CHook::RET(g_libGTASA + 0x3D4EAC); // CHud_DrawVitalStats
//
//	//
//	CHook::RET(g_libGTASA + 0x4F90AC); // CTheCarGenerators::Process
//	CHook::RET(g_libGTASA + 0x2E82CC); // CCarCtrl::GenerateRandomCars
//	CHook::RET(g_libGTASA + 0x504DB8); // CPlane::DoPlaneGenerationAndRemoval
//
//	CHook::RET(g_libGTASA + 0x2C1CB0); // CEntryExit::GenerateAmbientPeds
//	CHook::RET(g_libGTASA + 0x2B055C); // CCarCtrl::GenerateOneEmergencyServicesCar
//	CHook::NOP(g_libGTASA + 0x4612A0, 9); 	// CPedIntelligence::SetPedDecisionMakerType from CPopulation::AddPedAtAttractor
//	CHook::NOP(g_libGTASA + 0x4045D2, 1);	// CStreaming::ms_memoryAvailable = (int)v24
//	CHook::NOP(g_libGTASA + 0x39872A, 2);	// CCover::Update from CGame::Process
//	CHook::NOP(g_libGTASA + 0x3AC8B2, 2); 	// CMessages::AddBigMessage from CPlayerInfo::KillPlayer
//	CHook::NOP(g_libGTASA + 0x4F75B4, 4);  // CBoat::ProcessControl
//
//	CHook::NOP(g_libGTASA + 0x454A88, 2);  // CCamera::ClearPlayerWeaponMode from CPlayerPed::ClearWeaponTarget
//	CHook::NOP(g_libGTASA + 0x2FEE76, 2);	// CGarages::RespraysAreFree = true in CRunningScript::ProcessCommands800To899
//	CHook::NOP(g_libGTASA + 0x50FF64, 2);	// skip playerGifts from CVehicle::SetDriver
//	CHook::NOP(g_libGTASA + 0x39840A, 2);	// CStreaming::Shutdown from CGame::Shutdown
//	CHook::NOP(g_libGTASA + 0x4874E0, 5);  // CPedIntelligence::AddTaskPrimaryMaybeInGroup from CTaskComplexEnterCar::CreateNextSubTask
//	CHook::NOP(g_libGTASA + 0x2E1EDC, 2); 	// CUpsideDownCarCheck::UpdateTimers from CTheScripts::Process
//
//	CHook::WriteMemory(g_libGTASA + 0x2C3868, "\x00\x20\x70\x47", 4); 					// CGameLogic::IsCoopGameGoingOn
//	CHook::WriteMemory(g_libGTASA + 0x1A7EF2, "\x4F\xF4\x00\x10\x4F\xF4\x80\x00", 8); 	// RenderQueue::RenderQueue
//	CHook::WriteMemory(g_libGTASA + 0x1A7F32, "\x4F\xF4\x00\x10\x4F\xF4\x80\x00", 8); 	// RenderQueue::RenderQueue
//
//	CHook::WriteMemory(g_libGTASA + 0x45FC20, "\x4F\xF0\x00\x00\xF7\x46", 6);			// CPopulation::AddToPopulation
//	CHook::WriteMemory(g_libGTASA + 0x293218, "\x01\x20\x70\x47", 4); 					// CStreaming::RemoveLoadedVehicle
//	CHook::WriteMemory(g_libGTASA + 0x1BDD4A, "\x10\x46\xA2\xF1\x04\x0B", 6); 			// TextureDatabaseRuntime::LoadFullTexture
//	CHook::WriteMemory(g_libGTASA + 0x3E1A2C, "\x67\xE0", 2);							// Interior_c::Init

    CHook::RET(g_libGTASA + 0x004C9A14); // CPlayerPed::AnnoyPlayerPed не агриться в ответ
	//CHook::NOP(g_libGTASA + 0x00427D38, 2);
//	CHook::NOP(g_libGTASA + 0x45F74C, 16); // CPopulation::AddPedInCar - skip some wtf stuff
//
//	CHook::NOP(g_libGTASA + 0x00454950, 17); // CAnimManager::RemoveAnimBlockRef
//
	// TxdStore pool (old: 5000, new: 20000)
//	CHook::WriteMemory(g_libGTASA + 0x005D3AAA, (uintptr_t)"\x4f\xf4\xb8\x50\xc0\xf2\x11\x00", 8); //  MOV.W	R0, #0x1700 | MOVT	R0, #0x11
//	//CHook::WriteMemory(g_libGTASA + 0x55BA9E, "\xC0\xF2\x11\x00", 4);
//	CHook::WriteMemory(g_libGTASA + 0x005D3AB8, (uintptr_t)"\x44\xf6\x20\x60", 4); // MOVW	R0, #0x4E20
//	CHook::WriteMemory(g_libGTASA + 0x005D3ABC, (uintptr_t)"\x44\xf6\x20\x62", 4); // MOVW	R2, #0x4E20
//
//	// VehicleStruct increase (0x32C*0x50 = 0xFDC0)
//	CHook::WriteMemory(g_libGTASA + 0x405338, (uintptr_t)"\x4F\xF6\xC0\x50", 4);	// MOV  R0, #0xFDC0
//	CHook::WriteMemory(g_libGTASA + 0x405342, (uintptr_t)"\x50\x20", 2);			// MOVS R0, #0x50
//	CHook::WriteMemory(g_libGTASA + 0x405348, (uintptr_t)"\x50\x22", 2);			// MOVS R2, #0x50
//	CHook::WriteMemory(g_libGTASA + 0x405374, (uintptr_t)"\x50\x2B", 2);			// CMP  R3, #0x50
//
//
//	// stop assign 0xFFFFFF to beam color!
//	CHook::NOP(g_libGTASA + 0x00519116, 2);
//	CHook::NOP(g_libGTASA + 0x0051911C, 4);
//	CHook::NOP(g_libGTASA + 0x00519198, 4);
//	CHook::NOP(g_libGTASA + 0x00519198, 2);
//	CHook::NOP(g_libGTASA + 0x005191D0, 4);
//	CHook::NOP(g_libGTASA + 0x005191DC, 2);
//

//	// CPed::Say
//	CHook::NOP(g_libGTASA + 0x43E288, 2);
}
void ApplyInGamePatches()
{
	
	Log("Installing patches (ingame)..");

//	/* Разблокировка карты */
//	// CTheZones::ZonesVisited[100]
//	memset((void*)(g_libGTASA + 0x8EA7B0), 1, 100);
//	// CTheZones::ZonesRevealed
//	*(uint32_t*)(g_libGTASA + 0x8EA7A8) = 100;
//
//	// CarCtl::GenerateRandomCars nulled from CGame::Process
//	CHook::NOP(g_libGTASA + 0x398A3A, 2);
//
//	// CTheCarGenerators::Process nulled from CGame::Process
//	CHook::NOP(g_libGTASA + 0x398A34, 2);
//
//	// множитель для MaxHealth
//	CHook::UnFuck(g_libGTASA + 0x3BAC68);
//	*(float*)(g_libGTASA + 0x3BAC68) = 176.0f;
//	// множитель для Armour
//	CHook::UnFuck(g_libGTASA + 0x27D884);
//	*(float*)(g_libGTASA + 0x27D884) = 176.0;
//
//	// CEntryExit::GenerateAmbientPeds nulled from CEntryExit::TransitionFinished
//	CHook::NOP(g_libGTASA + 0x2C2C22, 4);
//
//	CHook::NOP(g_libGTASA + 0x003D6FDC, 2);
//	CHook::NOP(g_libGTASA + 0x0026B214, 2);
//
	// CPlayerPed::CPlayerPed task fix crash
	CHook::WriteMemory(g_libGTASA + 0x4C36E2, (uintptr_t)"\xE0", 1);
//
//	// ReapplyPlayerAnimation (хз зачем)
//	CHook::NOP(g_libGTASA + 0x45477E, 5);
//
//	// radar draw blips
//	CHook::NOP(g_libGTASA + 0x3DCA90, 2);
//	CHook::NOP(g_libGTASA + 0x3DD4A4, 2);
//	// CCamera::CamShake from CExplosion::AddExplosion
//	CHook::NOP(g_libGTASA + 0x55EFB8, 2);
//	CHook::NOP(g_libGTASA + 0x55F8F8, 2);
//
//	// camera_on_actor path
//	CHook::UnFuck(g_libGTASA + 0x2F7B68);
//	*(uint8_t*)(g_libGTASA + 0x2F7B6B) = 0xBE;
//
	// CPed::RemoveWeaponWhenEnteringVehicle (GetPlayerInfoForThisPlayerPed)
	CHook::RET(g_libGTASA + 0x004A536C );
//
//	// CBike::ProcessAI
//	CHook::UnFuck(g_libGTASA + 0x4EE200);
//	*(uint8_t*)(g_libGTASA + 0x4EE200) = 0x9B;
//
//
//	// no vehicle audio processing
//	CHook::NOP(g_libGTASA + 0x4E31A6, 2);
//	CHook::NOP(g_libGTASA + 0x4EE7D2, 2);
//	CHook::NOP(g_libGTASA + 0x4F741E, 2);
//	CHook::NOP(g_libGTASA + 0x50AB4A, 2);
////
//	CHook::NOP(g_libGTASA + 0x00398768, 2); // nop police helis
//	CHook::NOP(g_libGTASA + 0x003987DC, 2);
//
//	CHook::NOP(g_libGTASA + 0x003688EC, 2); // nop ServiceAmbientGunFire
//
//
//	// Disable in-game radio
//	CHook::NOP(g_libGTASA + 0x3688DA, 2);
//	CHook::NOP(g_libGTASA + 0x368DF0, 2);
//	CHook::NOP(g_libGTASA + 0x369072, 2);
//	CHook::NOP(g_libGTASA + 0x369168, 2);
//
	// Stop it trying to load tracks2.dat
	CHook::NOP(g_libGTASA + 0x0057CBD6, 2);

	// Stop it trying to load tracks4.dat
	CHook::NOP(g_libGTASA + 0x0057CC06, 2);
//
//    // Prevent cheats processing
//	CHook::NOP(g_libGTASA + 0x3987BA, 2);
//
//	// мини-карта в меню
//	CHook::NOP(g_libGTASA + 0x0026B504, 2); // убирает текст легенды карты
//	CHook::NOP(g_libGTASA + 0x0026B514, 2); // убирает значки легенды
//	CHook::NOP(g_libGTASA + 0x0026B49C, 2); // fix crash GetNextSpace
//
//	// дефолт кнопка сесть в авто
//	CHook::NOP(g_libGTASA + 0x00276512, 32); //CWidgetButtonEnterCar::Draw

	//todo CPlayerPed::ProcessAnimGroups in the end
}