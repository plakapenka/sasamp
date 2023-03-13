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
	CHook::RET(g_libGTASA + 0x004C6D64);
//    // CPlayerPed::FindNextWeaponLockOnTarget
	CHook::RET(g_libGTASA + 0x004C7D4C);
	//CHook::RET(g_libGTASA + 0x438DB4);

}

void ApplyFPSPatch(uint8_t fps)
{
	//CHook::WriteMemory(g_libGTASA + 0x004D4546, (uintptr_t)& fps, 1);
	CHook::WriteMemory(g_libGTASA + 0x005E49E0, (uintptr_t)& fps, 1);
	CHook::WriteMemory(g_libGTASA + 0x005E492E, (uintptr_t)& fps, 1);
	CHook::WriteMemory(g_libGTASA + 0x009FC8FC + 0xC, (uintptr_t)& fps, 1);

	Log("New fps limit = %d", fps);
}

int test_pointsArray[1000];
int test_pointersLibArray[1000];

extern void (*ANDRunThread)(void* a1);
void ANDRunThread_hook(void* a1);

void ApplyPatches_level0()
{
	Log("ApplyPatches_level0");

	CWorld::Players = new CPlayerInfoGta[MAX_PLAYERS]{nullptr};
	CHook::Write(g_libGTASA + 0x006783C0, CWorld::Players);

    CHook::Write(g_libGTASA + 0x00679B5C, &CWorld::PlayerInFocus);

    CHook::Redirect(g_libGTASA, 0x0042CA1C, &CUtil::FindPlayerSlotWithPedPointer);

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
	CExtendedCarColors::ApplyPatches_level0();
//
	CHook::SetUpHook(g_libGTASA + 0x0026BEA0, (uintptr_t)ANDRunThread_hook, (uintptr_t*)& ANDRunThread);
//
//    DisableAutoAim();
//
//	//UnFuck(g_libGTASA + 0x0056CA68);
//	//*(uint8_t*)(g_libGTASA + 0x0056CA68) = 1;
//
//	CHook::NOP(g_libGTASA + 0x001D2410, 4);
//

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
	CHook::RET(g_libGTASA + 0x00457A28); // CClothes::Init
	CHook::RET(g_libGTASA + 0x00457580); // CClothes::RebuildPlayer

	//CHook::NOP(g_libGTASA + 0x004087E8, 2); // CGlass::Init
	//CHook::NOP(g_libGTASA + 0x004087EC, 2); // CGarages::Init_AfterRestart
//	CHook::RET(g_libGTASA + 0x0049EF08); // CGangs::Initialise
	//CHook::NOP(g_libGTASA + 0x00408812, 2); // CClock::Initialise(uint)
//	CHook::NOP(g_libGTASA + 0x00408816, 2); // CHeli::InitHelis(void)

	//CHook::NOP(g_libGTASA + 0x402472, 2);
	CHook::RET(g_libGTASA + 0x0046B5B8); // CFileLoader::LoadPickup
	CHook::RET(g_libGTASA + 0x00445EE8); // Interior_c::AddPickups

//	CHook::NOP(g_libGTASA + 0x0040881A, 2); // CCranes::InitCranes(void)
}

void ApplyShadowPatch()
{
	CHook::NOP(g_libGTASA + 0x003FCD84, 2); // CRealTimeShadowManager::ReturnRealTimeShadow from ~CPhysical crash!
//	CHook::RET(g_libGTASA + 0x541AA8); // CRealTimeShadowManager::ReturnRealTimeShadow from ~CPhysical
//	CHook::NOP(g_libGTASA + 0x2661DA, 2); // Display - Shadows ( настройки )
	CHook::RET(g_libGTASA + 0x005B800C); //shadow CRealTimeShadowManager::Update
//	CHook::RET(g_libGTASA + 0x543B04); // CShadows::RenderStaticShadows
//	CHook::NOP(g_libGTASA + 0x0039B2C4, 2); // CMirrors::BeforeMainRender(void)
//	CHook::RET(g_libGTASA + 0x5471F4); // CShadows::RenderStoredShadows
//	CHook::RET(g_libGTASA + 0x545C04); // CShadows::StoreStaticShadow
}

void ApplyDefaultPlatePatch()
{
	//CCustomCarPlateMgr all meth
	// убрать дефолтные номера
//	CHook::RET(g_libGTASA + 0x0052FF5C); //CCustomCarPlateMgr::GeneratePlateText
//
//	CHook::RET(g_libGTASA + 0x0052FE90);
//	CHook::RET(g_libGTASA + 0x00530098);
//	CHook::RET(g_libGTASA + 0x00530104);
//	CHook::RET(g_libGTASA + 0x0053012C);
//	CHook::RET(g_libGTASA + 0x00530158);
//	CHook::RET(g_libGTASA + 0x00530190);
//
//	CHook::RET(g_libGTASA + 0x0053021C);
//	CHook::RET(g_libGTASA + 0x0053038C);
//	CHook::RET(g_libGTASA + 0x00530420);
//
//	CHook::RET(g_libGTASA + 0x0050E37C);
}

void ApplyPatches()
{
	Log("Installing patches..");

	// Menu
	//CHook::NOP(g_libGTASA + 0x0029BC18, 76); // stats

	// JPatch
	CHook::WriteMemory(g_libGTASA + 0x003F4138, (uintptr_t)"\x03", 1); // RE3: Fix R* optimization that prevents peds to spawn
	CHook::WriteMemory(g_libGTASA + 0x001D240E, (uintptr_t)"\x08", 1); // Lower threads sleeping timer
	CHook::WriteMemory(g_libGTASA + 0x00266CC2, (uintptr_t)"\x08", 1); // Lower threads sleeping timer

	// задние фары
	CHook::WriteMemory(g_libGTASA + 0x00591074, (uintptr_t)"\x04", 1);
	CHook::WriteMemory(g_libGTASA + 0x00591086, (uintptr_t)"\xFA", 1);

	// CdStreamInit(6);
	CHook::WriteMemory(g_libGTASA + 0x003F3698, (uintptr_t)"\x06\x20", 2);

	CHook::RET(g_libGTASA + 0x004CF2DC); // CPopulation::AddPed
	CHook::RET(g_libGTASA + 0x003E33C8); // CClock::Update
    CHook::RET(g_libGTASA + 0x0056E3C0); // CTheCarGenerators::Process()
    CHook::RET(g_libGTASA + 0x002E833C); // CCarCtrl::GenerateRandomCars()
    CHook::RET(g_libGTASA + 0x0040B078); // CPlayerInfo::FindObjectToSteal
    CHook::RET(g_libGTASA + 0x00306F40); // CEntryExit::GenerateAmbientPeds
	CHook::RET(g_libGTASA + 0x002BCCF8);// здоровье, броня, фист звезды и тп
	CHook::RET(g_libGTASA + 0x00436FAC); // CHud::SetHelpMessage
	//CHook::RET(g_libGTASA + 0x004211F0); // CPlaceName::Process
	CHook::RET(g_libGTASA + 0x004B2D14); // CPedGroupDefaultTaskAllocatorRandom::AllocateDefaultTasks crash
	CHook::RET(g_libGTASA + 0x004397B4); // CHud::DrawBustedWastedMessage // ПОТРАЧЕНО
//	CHook::RET(g_libGTASA + 0x00352154); // in game radio
	CHook::RET(g_libGTASA + 0x003BD974); // звук загрузочного экрана
	CHook::RET(g_libGTASA + 0x00324368); // CVehicleRecording::Load

	CHook::RET(g_libGTASA + 0x004B7630); // CPedGroups::Process() crash
	CHook::RET(g_libGTASA + 0x004B425E); // CPedGroupIntelligence::Process crash
	CHook::RET(g_libGTASA + 0x004B2050); // CPedGroupMembership::SetLeader crash
	CHook::RET(g_libGTASA + 0x004B3156); // CPedGroupIntelligence::Flush crash!111111111

	CHook::NOP(g_libGTASA + 0x004409AE, 2); // моргание значнок на карте
	CHook::NOP(g_libGTASA + 0x002AB400, 85); // названия зон при просмотре карты
//	CHook::RET(g_libGTASA + 0x004B425E); // CPedGroupIntelligence::Process crash

	//CHook::WriteMemory(g_libGTASA + 0x00427D5C, "\x02\x21", 2); // CWorld::Process

	ApplyShadowPatch();
//    ApplyDefaultPlatePatch();
//
//	//CHook::Write<uint32_t>(g_libGTASA + 0x8E87E4, 1);
//
//	// nop check CWorld::FindPlayerSlotWithPedPointer (fuck*** R*)
//	// just left mov r4, r0 and ldr r1 [pc, shit]
//	CHook::NOP(g_libGTASA + 0x0045A4C8, 11);
//	CHook::NOP(g_libGTASA + 0x0045A4E0, 3);
//
//	//UnFuck(g_libGTASA + 0x008B8018);
//	//*(uint8_t*)(g_libGTASA + 0x008B8018) = 1;

//	// DefaultPCSaveFileName
//	char* DefaultPCSaveFileName = (char*)(g_libGTASA + 0x60EAE8);
//	memcpy((char*)DefaultPCSaveFileName, "GTASAMP", 8);
//
//	// live russia
//	CHook::RET(g_libGTASA + 0x0051DEC4);			// живность в воде WaterCreatureManager_c::Update

	CHook::NOP(g_libGTASA + 0x00584884, 2);	// не давать ган при выходе из тачки 	( клюшка, дробовик  )
	CHook::NOP(g_libGTASA + 0x00584850, 2);	// не давать ган при выходе из тачки 	( клюшка, дробовик  )
	CHook::NOP(g_libGTASA + 0x00584892, 2);	// не давать ган при выходе из тачки	( клюшка, дробовик  )

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
//	CHook::RET(g_libGTASA + 0x4BDB18);	// CTaskSimplePlayerOnFoot::PlayIdleAnimations
//	CHook::RET(g_libGTASA + 0x494FE4);	// CCarEnterExit::SetPedInCarDirect
//	CHook::RET(g_libGTASA + 0x3DA500); // CRadar::DrawLgegend
//	CHook::RET(g_libGTASA + 0x00441A58); // CRadar::AddBlipToLegendList
//	CHook::RET(g_libGTASA + 0x0043FF24);
//	CHook::RET(g_libGTASA + 0x5366D0); // CGlass::Render
//	//CHook::RET(g_libGTASA + 0x5286EC); // CBirds::Render
//	CHook::RET(g_libGTASA + 0x4DD00C); // CAutomobile::CustomCarPlate_BeforeRenderingStart
//	CHook::RET(g_libGTASA + 0x4DD038); // CAutomobile::CustomCarPlate_AfterRenderingStop
//	CHook::RET(g_libGTASA + 0x36E77C); // CCamera::CamShake
//	CHook::RET(g_libGTASA + 0x392A98); // CEntity::PreRenderForGlassWindow
//	CHook::RET(g_libGTASA + 0x315310); // CStuntJumpManager::Update
//	CHook::RET(g_libGTASA + 0x005C5098); // CMirrors::RenderReflBuffer
//	CHook::RET(g_libGTASA + 0x3B67F8); // CRopes::Update
//	CHook::RET(g_libGTASA + 0x4FDC78); // CHeli::UpdateHelis
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

//	CHook::RET(g_libGTASA + 0x00579284); // CPlane::DoPlaneGenerationAndRemoval
//
//	CHook::RET(g_libGTASA + 0x2C1CB0); // CEntryExit::GenerateAmbientPeds
//	CHook::RET(g_libGTASA + 0x002FB2C8); // CCarCtrl::GenerateOneEmergencyServicesCar
//	CHook::NOP(g_libGTASA + 0x4612A0, 9); 	// CPedIntelligence::SetPedDecisionMakerType from CPopulation::AddPedAtAttractor
	//CHook::NOP(g_libGTASA + 0x0046BE88, 2);	// CStreaming::ms_memoryAvailable = (int)v24
//	CHook::NOP(g_libGTASA + 0x39872A, 2);	// CCover::Update from CGame::Process
//	CHook::NOP(g_libGTASA + 0x3AC8B2, 2); 	// CMessages::AddBigMessage from CPlayerInfo::KillPlayer
//	CHook::NOP(g_libGTASA + 0x4F75B4, 4);  // CBoat::ProcessControl
//
//	CHook::NOP(g_libGTASA + 0x454A88, 2);  // CCamera::ClearPlayerWeaponMode from CPlayerPed::ClearWeaponTarget
//	CHook::NOP(g_libGTASA + 0x2FEE76, 2);	// CGarages::RespraysAreFree = true in CRunningScript::ProcessCommands800To899
//	CHook::NOP(g_libGTASA + 0x50FF64, 2);	// skip playerGifts from CVehicle::SetDriver
//	CHook::NOP(g_libGTASA + 0x39840A, 2);	// CStreaming::Shutdown from CGame::Shutdown
//	CHook::NOP(g_libGTASA + 0x4874E0, 5);  // CPedIntelligence::AddTaskPrimaryMaybeInGroup from CTaskComplexEnterCar::CreateNextSubTask
	//CHook::RET(g_libGTASA + 0x00328430); 	// CUpsideDownCarCheck::UpdateTimers from CTheScripts::Process
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

	// TxdStore pool (old: 5000, new: 20000)
//	CHook::WriteMemory(g_libGTASA + 0x005D3AAA, (uintptr_t)"\x4f\xf4\xb8\x50\xc0\xf2\x11\x00", 8); //  MOV.W	R0, #0x1700 | MOVT	R0, #0x11
//	//CHook::WriteMemory(g_libGTASA + 0x55BA9E, "\xC0\xF2\x11\x00", 4);
//	CHook::WriteMemory(g_libGTASA + 0x005D3AB8, (uintptr_t)"\x44\xf6\x20\x60", 4); // MOVW	R0, #0x4E20
//	CHook::WriteMemory(g_libGTASA + 0x005D3ABC, (uintptr_t)"\x44\xf6\x20\x62", 4); // MOVW	R2, #0x4E20
//
//	// VehicleStruct increase (0x32C*0x50 = 0xFDC0)
//	CHook::WriteMemory(g_libGTASA + 0x00468BEE, (uintptr_t)"\x4F\xF6\xC0\x50", 4);	// MOV  R0, #0xFDC0
//	CHook::WriteMemory(g_libGTASA + 0x00468BFA, (uintptr_t)"\x50\x20", 2);			// MOVS R0, #0x50
//	//CHook::WriteMemory(g_libGTASA + 0x405348, (uintptr_t)"\x50\x22", 2);			// MOVS R2, #0x50
//	CHook::WriteMemory(g_libGTASA + 0x00468C3C, (uintptr_t)"\x50\x28", 2);			// CMP  R0, #0x50
//
//
//	// stop assign 0xFFFFFF to beam color! FIXME
//	CHook::NOP(g_libGTASA + 0x00519116, 2);
//	CHook::NOP(g_libGTASA + 0x0051911C, 4);
//	CHook::NOP(g_libGTASA + 0x00519198, 4);
//	CHook::NOP(g_libGTASA + 0x00519198, 2);
//	CHook::NOP(g_libGTASA + 0x005191D0, 4);
//	CHook::NOP(g_libGTASA + 0x005191DC, 2);
//

	// CPed::Say
//	CHook::NOP(g_libGTASA + 0x43E288, 2);
}
void ApplyInGamePatches()
{
	
	Log("Installing patches (ingame)..");

	/* Разблокировка карты */
	// CTheZones::ZonesVisited[100]
	memset((void*)(g_libGTASA + 0x0098D252), 1, 100);
	// CTheZones::ZonesRevealed
	*(uint32_t*)(g_libGTASA + 0x0098D2B8) = 100;

	CHook::NOP(g_libGTASA + 0x002ABA08, 2); // текст на мини-карте ( легенда )
	CHook::NOP(g_libGTASA + 0x002ABA14, 2); // иконки ( легенда )

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
	//CHook::WriteMemory(g_libGTASA + 0x4C36E2, (uintptr_t)"\xE0", 1);


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

	// no vehicle audio processing
	CHook::NOP(g_libGTASA + 0x00553E96, 2);
	CHook::NOP(g_libGTASA + 0x00561AC2, 2);
	CHook::NOP(g_libGTASA + 0x0056BED4, 2);
	CHook::NOP(g_libGTASA + 0x0057D0C4, 2);

//	CHook::NOP(g_libGTASA + 0x00398768, 2); // nop police helis
//	CHook::NOP(g_libGTASA + 0x003987DC, 2);
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

//	CHook::NOP(g_libGTASA + 0x00427D38, 2);
//	CHook::NOP(g_libGTASA + 0x00427D5E, 2);
	//todo CPlayerPed::ProcessAnimGroups in the end
}