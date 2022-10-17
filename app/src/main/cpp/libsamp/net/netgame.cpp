#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "..//util/CJavaWrapper.h"
#include "..//util/utf8_to_cp1251.h"
#include <thread>
#include <chrono>
#include <ctime>
#include "../vendor/hash/sha256.h"

extern CNetGame *pNetGame;

#include "gui/gui.h"
extern CGUI *pGUI;

#include "..//CSettings.h"
extern CSettings* pSettings;

extern CWidgetManager *g_pWidgetManager;

#include "../chatwindow.h"

#include "..//CClientInfo.h"
#include "..//CLocalisation.h"

#define NETGAME_VERSION 4057
//#define AUTH_BS "E02262CF28BC542486C558D4BE9EFB716592AFAF8B"
#define AUTH_BS "10B3D2B1317ADD02CC1F680BC500A8BC0FD7AD42CE7"

extern CGame *pGame;
extern CChatWindow *pChatWindow;

int iVehiclePoolProcessFlag = 0;
int iPickupPoolProcessFlag = 0;

void RegisterRPCs(RakClientInterface *pRakClient);
void UnRegisterRPCs(RakClientInterface *pRakClient);
void RegisterScriptRPCs(RakClientInterface *pRakClient);
void UnRegisterScriptRPCs(RakClientInterface *pRakClient);

unsigned char GetPacketID(Packet *p)
{
	if (p == 0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
		return (unsigned char)p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	else
		return (unsigned char)p->data[0];
}

CNetGame::CNetGame(const char *szHostOrIp, int iPort, const char *szPlayerName, const char *szPass)
{
	strcpy(m_szHostName, "San Andreas Multiplayer");
	strncpy(m_szHostOrIp, szHostOrIp, sizeof(m_szHostOrIp));
	m_iPort = iPort;

	m_pPlayerPool = new CPlayerPool();
	m_pPlayerPool->SetLocalPlayerName(szPlayerName);

	m_pVehiclePool = new CVehiclePool();
	m_pObjectPool = new CObjectPool();
	m_pPickupPool = new CPickupPool();
	m_pGangZonePool = new CGangZonePool();
	m_pLabelPool = new CText3DLabelsPool();
	m_pTextDrawPool = new CTextDrawPool();
	g_pWidgetManager = new CWidgetManager();
	m_pStreamPool = new CStreamPool();
	m_pActorPool = new CActorPool();

	m_pRakClient = RakNetworkFactory::GetRakClientInterface();
	RegisterRPCs(m_pRakClient);
	RegisterScriptRPCs(m_pRakClient);
	m_pRakClient->SetPassword(szPass);

	m_dwLastConnectAttempt = GetTickCount();
	m_iGameState = GAMESTATE_WAIT_CONNECT;

	killlistduels = 0;

	m_iSpawnsAvailable = 0;
	m_bHoldTime = true;
	m_byteWorldMinute = 0;
	m_byteWorldTime = 12;
	m_byteWeather = 10;
	m_fGravity = (float)0.008000000;
	m_bUseCJWalk = false;
	m_bDisableEnterExits = false;
	m_fNameTagDrawDistance = 60.0f;
	m_bZoneNames = false;
	m_GreenZoneState = false;
	m_bInstagib = false;
	m_iDeathDropMoney = 0;
	m_bNameTagLOS = false;

	for (int i = 0; i < 100; i++)
		m_dwMapIcons[i] = 0;

	g_pJavaWrapper->HideKilllist();
	g_pJavaWrapper->HideSamwillMoney();
	g_pJavaWrapper->ShowServer(pSettings->GetReadOnly().szServer);

	pGame->EnableClock(false);
	pGame->EnableZoneNames(false);
	if (pChatWindow)
		pChatWindow->AddDebugMessage("{bbbbbb}������ {ff0000}LIVE RUSSIA{bbbbbb} �������{ffffff}");
}

CNetGame::~CNetGame()
{
	m_pRakClient->Disconnect(0);
	UnRegisterRPCs(m_pRakClient);
	UnRegisterScriptRPCs(m_pRakClient);
	RakNetworkFactory::DestroyRakClientInterface(m_pRakClient);

	if (m_pPlayerPool)
	{
		delete m_pPlayerPool;
		m_pPlayerPool = nullptr;
	}

	if (m_pVehiclePool)
	{
		delete m_pVehiclePool;
		m_pVehiclePool = nullptr;
	}

	if (m_pPickupPool)
	{
		delete m_pPickupPool;
		m_pPickupPool = nullptr;
	}

	if (m_pGangZonePool)
	{
		delete m_pGangZonePool;
		m_pGangZonePool = nullptr;
	}

	if (m_pLabelPool)
	{
		delete m_pLabelPool;
		m_pLabelPool = nullptr;
	}

	if (m_pTextDrawPool)
	{
		delete m_pTextDrawPool;
		m_pTextDrawPool = nullptr;
	}

	if (g_pWidgetManager)
	{
		delete g_pWidgetManager;
		g_pWidgetManager = nullptr;
	}
	if (m_pStreamPool)
	{
		delete m_pStreamPool;
		m_pStreamPool = nullptr;
	}
}

bool g_IsVoiceServer();
#include "CUDPSocket.h"
#include "..//CServerManager.h"
void CNetGame::Process()
{

	// todo: 30 fps fixed rate
	static uint32_t time = GetTickCount();
	bool bProcess = false;
	if (GetTickCount() - time >= 1000 / 30)
	{
		UpdateNetwork();
		time = GetTickCount();
		bProcess = true;
	}
	// server checkpoints update
	if (m_pPlayerPool)
	{
		if (m_pPlayerPool->GetLocalPlayer())
		{
			if (m_pPlayerPool->GetLocalPlayer()->m_bIsActive && m_pPlayerPool->GetLocalPlayer()->GetPlayerPed())
			{
				pGame->UpdateCheckpoints();
			}
		}
	}
	if (m_bHoldTime)
		pGame->SetWorldTime(m_byteWorldTime, m_byteWorldMinute);

	if (GetGameState() == GAMESTATE_CONNECTED)
	{
		// pool process
		if (m_pPlayerPool && bProcess)
			m_pPlayerPool->Process();
		if (m_pObjectPool)
			m_pObjectPool->Process();
		if (m_pVehiclePool && iVehiclePoolProcessFlag > 5)
		{
			m_pVehiclePool->Process();
			iVehiclePoolProcessFlag = 0;
		}

		if (m_pPickupPool && iPickupPoolProcessFlag > 5)
		{
			m_pPickupPool->Process();
			iPickupPoolProcessFlag = 0;
		}
		else
		{
			++iPickupPoolProcessFlag;
			++iVehiclePoolProcessFlag;
		}
	}
	else
	{
		g_pJavaWrapper->HideLoadingScreen();

		CPlayerPed *pPlayer = pGame->FindPlayerPed();
		CCamera *pCamera = pGame->GetCamera();

		if (pPlayer && pCamera)
		{
			
			if (pPlayer->IsInVehicle())
				pPlayer->RemoveFromVehicleAndPutAt(1093.4f, -2036.5f, 82.7106f);
			else
				pPlayer->TeleportTo(1093.4f, -2036.5f, 82.7106f);


			// ������ ��� �����������
			pCamera->SetPosition(1093.0f, -2036.0f, 90.0f, 0.0f, 0.0f, 0.0f);
			pCamera->LookAtPoint(384.0f, -1557.0f, 20.0f, 2);
			pGame->SetWorldWeather(m_byteWeather);
			pGame->DisplayWidgets(false);
			

			/*  ������� ����� ��� ������������� �������
			PLAYER_SPAWN_INFO SpawnInfo;
			SpawnInfo.iSkin = g_pJavaWrapper->RegisterSkinId;
			SpawnInfo.vecPos.X = -82.9753;
			SpawnInfo.vecPos.Y = 966.7605;
			SpawnInfo.vecPos.Z = 1597.9788;
			SpawnInfo.fRotation = 90.0f;
			CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
			pPlayerPool->GetLocalPlayer()->SetSpawnInfo(&SpawnInfo);
			pPlayerPool->GetLocalPlayer()->Spawn();
			*/

			/*
			if (pPlayer->IsInVehicle())
				pPlayer->RemoveFromVehicleAndPutAt(-50.4f, 115.5f, 250.7106f);
			else
				pPlayer->TeleportTo(-50.4f, 115.5f, 250.7106f);

			VECTOR *camerastart = {-323.590118, 230.307800, 274.664215};
			VECTOR *cameraend = {-323.590118, 230.307800, 274.664215};
			VECTOR *cameralookstart = {-321.099090, 226.183731, 273.327484};
			VECTOR *cameralookend = {-321.097198, 226.184265, 273.329376};

			// �������� ������ ��� �����������
			pCamera->InterpolateCameraPos(camerastart, cameraend, 1, 1);
			pCamera->InterpolateCameraLookAt(cameralookstart, cameralookend, 1, 1);
			pGame->SetWorldWeather(m_byteWeather);
			pGame->DisplayWidgets(false);
			*/
		}
	}

	if (GetGameState() == GAMESTATE_WAIT_CONNECT &&
		(GetTickCount() - m_dwLastConnectAttempt) > 3000)
	{
		/*if (CClientInfo::bSAMPModified)
		{
			if (pChatWindow) pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::MODIFIED_FILES));
			SetGameState(GAMESTATE_CONNECTING);
			m_dwLastConnectAttempt = GetTickCount();
			return;
		}

		if (!CClientInfo::bJoinedFromLauncher)
		{
			if (pChatWindow) pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::NOT_FROM_LAUNCHER));
			SetGameState(GAMESTATE_CONNECTING);
			m_dwLastConnectAttempt = GetTickCount();
			return;
		}*/

		if (pChatWindow)
			pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::CONNECTING));
		static bool sent = false;
		CUDPSocket sock;
		for (int i = 0; i < 100; i++)
		{
			const char *ip = m_pRakClient->GetPlayerID().ToString();
			if (sock.Bind(5000 + i * 10 + (rand() % 100)))
			{
				CRawData data(250);
				while (data.GetWriteOffset() < 170)
				{
					data.Write("SAMP", 4);
					data.Write("00000000", 8);
					data.Write(ip, strlen(ip));
					data.Write("00000000", 8);
					data.Write(g_sEncryptedAddresses[0].decrypt(), strlen(g_sEncryptedAddresses[0].decrypt()));
					data.Write("00112233445566778899FFFFFFFFFFFFFFFF", 36);
				}
				std::string ip = g_sEncryptedAddresses[0].decrypt();
				std::stringstream s(ip);
				int a, b, c, d;
				char ch;
				s >> a >> ch >> b >> ch >> c >> ch >> d;
				CAddress dest(a, b, c, d);
				dest.usPort = g_sEncryptedAddresses[0].getPort();

				for (int j = 0; j < 3; j++)
				{
					sock.Send(dest, data);
					usleep(5);
				}

				break;
			}
		}

		m_pRakClient->Connect(m_szHostOrIp, m_iPort, 0, 0, 5);
		m_dwLastConnectAttempt = GetTickCount();
		SetGameState(GAMESTATE_CONNECTING);
	}
}

void CNetGame::UpdateNetwork()
{

	Packet *pkt = nullptr;
	unsigned char packetIdentifier;

	while (pkt = m_pRakClient->Receive())
	{
		packetIdentifier = GetPacketID(pkt);

		switch (packetIdentifier)
		{
		case ID_AUTH_KEY:
			Log("Incoming packet: ID_AUTH_KEY");
			Packet_AuthKey(pkt);
			break;

		case ID_CONNECTION_ATTEMPT_FAILED:
			pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::CONNECTION_ATTEMPT_FAILED));
			SetGameState(GAMESTATE_WAIT_CONNECT);
			break;

		case ID_NO_FREE_INCOMING_CONNECTIONS:
			pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::FULL_SERVER));
			SetGameState(GAMESTATE_WAIT_CONNECT);
			break;

		case ID_DISCONNECTION_NOTIFICATION:
			Packet_DisconnectionNotification(pkt);
			break;

		case ID_CONNECTION_LOST:
			Packet_ConnectionLost(pkt);
			break;

		case ID_CONNECTION_REQUEST_ACCEPTED:
			Packet_ConnectionSucceeded(pkt);
			break;

		case ID_FAILED_INITIALIZE_ENCRIPTION:
			// pChatWindow->AddDebugMessage("Failed to initialize encryption.");
			break;

		case ID_CONNECTION_BANNED:
			// pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::BANNED));
			break;

		case ID_INVALID_PASSWORD:
			pChatWindow->AddDebugMessage("������������ ������.");
			m_pRakClient->Disconnect(0);
			break;

		case ID_PLAYER_SYNC:
			Packet_PlayerSync(pkt);
			break;

		case ID_VEHICLE_SYNC:
			Packet_VehicleSync(pkt);
			break;

		case ID_PASSENGER_SYNC:
			Packet_PassengerSync(pkt);
			break;

		case ID_MARKERS_SYNC:
			Packet_MarkersSync(pkt);
			break;

		case ID_AIM_SYNC:
			Packet_AimSync(pkt);
			break;

		case ID_BULLET_SYNC:
			Packet_BulletSync(pkt);
			break;

		case ID_TRAILER_SYNC:
			Packet_TrailerSync(pkt);
			break;

		case PACKET_CUSTOM:
			Packet_CustomRPC(pkt);
			break;

		case PACKET_AUTH:
			Packet_AuthRPC(pkt);
			break;

		case PACKET_SPECIALCUSTOM:
			Packet_SpecialCustomRPC(pkt);
			break;

		case PACKET_AUTOSALON:
			Packet_AutosalonRPC(pkt);
			break;
		}

		m_pRakClient->DeallocatePacket(pkt);
	}
}

void CNetGame::Packet_TrailerSync(Packet *p)
{
	CRemotePlayer *pPlayer;
	RakNet::BitStream bsSpectatorSync((unsigned char *)p->data, p->length, false);

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	BYTE bytePacketID = 0;
	BYTE bytePlayerID = 0;
	TRAILER_SYNC_DATA trSync;

	bsSpectatorSync.Read(bytePacketID);
	bsSpectatorSync.Read(bytePlayerID);
	bsSpectatorSync.Read((char *)&trSync, sizeof(TRAILER_SYNC_DATA));

	pPlayer = GetPlayerPool()->GetAt(bytePlayerID);

	if (pPlayer)
	{
		pPlayer->StoreTrailerFullSyncData(&trSync);
	}
}

#define AUTH_SALT "78r5jgwvbcg6dv1"
// Packet 251
#define RPC_TOGGLE_HUD_ELEMENT 1
#define RPC_STREAM_CREATE 2
#define RPC_STREAM_POS 3
#define RPC_STREAM_DESTROY 4
#define RPC_STREAM_INDIVIDUAL 5
#define RPC_STREAM_VOLUME 6
#define RPC_STREAM_ISENABLED 7
#define RPC_OPEN_LINK 8
#define RPC_TIMEOUT_CHAT 9
#define RPC_CUSTOM_COMPONENT 10
#define RPC_CUSTOM_HANDLING 11
#define RPC_CUSTOM_VISUALS 13
#define RPC_CUSTOM_HANDLING_DEFAULTS 14
#define RPC_OPEN_SETTINGS 15
#define RPC_CUSTOM_SET_FUEL 16
#define RPC_CUSTOM_SET_LEVEL 17
#define RPC_TOGGLE_GPS_INFO 18
#define RPC_TOGGLE_GREENZONE 19
#define RPC_TOGGLE_SAMWILL_GAME 20 // �������� uint8 toggle, ��������� uint8 quantity
#define RPC_KILLLIST 21
#define RPC_CLEAR_KILLLIST 22
#define RPC_SHOW_DUELS_KILLS_LEFT 23
#define RPC_DUELS_SHOW_LOCAL_TOP 24
#define RPC_DUELS_SHOW_LOCAL_STAT 25
#define RPC_SHOW_ACTION_LABEL 26
#define RPC_SHOW_TARGET_LABEL 27
#define RPC_SHOW_FACTORY_GAME 28
#define RPC_SHOW_MINING_GAME 29
#define RPC_SHOW_TD_BUS 30
#define RPC_SHOW_DICE_TABLE 31
#define RPC_SHOW_LUCKYWHEEL 32
#define RPC_SHOW_CASINO_BUY_CHIP 34
#define RPC_AUC_MENU_TOGGLE 35
#define RPC_CHECK_CLIENT 46

// Packet 252
#define RPC_TOGGLE_LOGIN (1)
#define RPC_TOGGLE_REGISTER (2)

// Packet 253
#define RPC_CLEAR_ANIM (1)
#define RPC_TOGGLE_CHOOSE_SPAWN (2)

// Packet 254
#define RPC_TOGGLE_BUY_MENU (1)
#define RPC_VEHICLE_DATA (2)
#define RPC_BUTTON_CLICKED (3) // send rpc

// other
//#define RPC_CUSTOM_ACTOR_PUT_IN_VEH 0x20
//#define RPC_CUSTOM_ACTOR_REMOVE_VEH 0x21
//#define RPC_CUSTOM_ACTOR_ADD_ADDITIONAL 0x22
#include "..//game/CCustomPlateManager.h"

void CNetGame::Packet_AutosalonRPC(Packet *p)
{
	RakNet::BitStream bs((unsigned char *)p->data, p->length, false);
	uint8_t packetID;
	uint32_t rpcID;
	bs.Read(packetID);
	bs.Read(rpcID);
	// pChatWindow->AddDebugMessage("packet: %d rpc: %d", packetID, rpcID);

	switch (rpcID)
	{
		case RPC_TOGGLE_BUY_MENU:
		{
			uint32_t toggle;
			bs.Read(toggle);
			if (toggle == 1)
			{
				g_pJavaWrapper->HideHud();
				pGame->ToggleHUDElement(0, false);
				g_pJavaWrapper->ShowAutosalon();
			}
			else if (toggle == 0)
			{
				g_pJavaWrapper->HideAutosalon();
				g_pJavaWrapper->ShowHud();
				pGame->ToggleHUDElement(0, true);
			}
			break;
		}
		case RPC_VEHICLE_DATA:
		{
			uint32_t price;
			uint32_t quantity;
			uint32_t vehicleid;
			char str[256];
			uint8_t len;
			bs.Read(len);
			bs.Read(&str[0], len);
			bs.Read(price);
			bs.Read(quantity);
			bs.Read(vehicleid);
			g_pJavaWrapper->HideHud();
			pGame->ToggleHUDElement(0, false);
			g_pJavaWrapper->UpdateAutosalon((char *)str, price, quantity, -1, -1);
			break;
		}
	}
}

void CNetGame::Packet_AuthRPC(Packet *p)
{
	RakNet::BitStream bs((unsigned char *)p->data, p->length, false);
	uint8_t packetID;
	uint32_t rpcID;
	bs.Read(packetID);
	bs.Read(rpcID);
	// pChatWindow->AddDebugMessage("packet: %d rpc: %d", packetID, rpcID);

	switch (rpcID)
	{
		case RPC_TOGGLE_LOGIN:
		{
            uint32_t toggle;
            bs.Read(toggle);
            if (toggle == 1)
            {
                CPlayerPool *pPlayerPool = GetPlayerPool();
                if (pPlayerPool)
                {
                    g_pJavaWrapper->ShowAuthorization(pPlayerPool->GetLocalPlayerName(), pPlayerPool->GetLocalPlayerID());
                }
            }
            else if (toggle == 0)
            {
                g_pJavaWrapper->HideAuthorization();
            }
            break;
		}
		case RPC_TOGGLE_REGISTER:
		{
			uint8_t toggle;
			bs.Read(toggle);
			if (toggle == 1)
			{
				g_pJavaWrapper->ShowRegister();
			}
			else if (toggle == 0)
			{
				g_pJavaWrapper->HideRegister();
				g_pJavaWrapper->ShowHud();
			}
			break;
		}
	}
}

void CNetGame::Packet_SpecialCustomRPC(Packet *p)
{
	RakNet::BitStream bs((unsigned char *)p->data, p->length, false);
	uint8_t packetID;
	uint32_t rpcID;
	bs.Read(packetID);
	bs.Read(rpcID);
	// pChatWindow->AddDebugMessage("packet: %d rpc: %d", packetID, rpcID);

	switch (rpcID)
	{
		case RPC_CLEAR_ANIM:
		{
			uint8_t toggle;
			bs.Read(toggle);
			if (toggle == 1)
			{
				MATRIX4X4 mat;
				uint8_t byteSpecialAction;
				byteSpecialAction = 0;
				CPlayerPool *pPlayerPool = GetPlayerPool();

				if(pPlayerPool) {
					CPlayerPed *pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
					if(pPlayerPed) 
					{
						pPlayerPool->GetLocalPlayer()->ApplySpecialAction(byteSpecialAction);
						pPlayerPed->GetMatrix(&mat);
						pPlayerPed->TeleportTo(mat.pos.X, mat.pos.Y, mat.pos.Z);
					}
				}

				Log("Custom RPC_CLEAR_ANIM");
			}
			break;
		}
		case RPC_TOGGLE_CHOOSE_SPAWN:
		{
			uint8_t toggle;
			bs.Read(toggle);
			if (toggle == 1)
			{
				g_pJavaWrapper->HideHud();
				g_pJavaWrapper->HideServer();
				g_pJavaWrapper->ShowSpawn(1, 1, 1, 1, 1);
			}
			else if (toggle == 0)
			{
				g_pJavaWrapper->HideSpawn();
				g_pJavaWrapper->ShowHud();
				g_pJavaWrapper->ShowServer(pSettings->GetReadOnly().szServer);
			}
			break;
		}
	}
}

void ShowNotificationProcess()
{
	for (int i = 0; i < 1000; i++)
	{
		g_pJavaWrapper->UpdateNotification(i);
		usleep(5000);
	}
	g_pJavaWrapper->HideNotification();
}

void CNetGame::Packet_CustomRPC(Packet *p)
{
	RakNet::BitStream bs((unsigned char *)p->data, p->length, false);
	uint8_t packetID;
	uint32_t rpcID;
	bs.Read(packetID);
	bs.Read(rpcID);
	// pChatWindow->AddDebugMessage("packet: %d rpc: %d", packetID, rpcID);
	
	switch (rpcID)
	{
	case RPC_AUC_MENU_TOGGLE:
	{
		uint8_t toggle;
		bs.Read(toggle);

		if (toggle == 1)
		{

		}
		else
		{

		}

		break;
	}
	case RPC_SHOW_LUCKYWHEEL:
	{
		uint16_t quantity;
		bs.Read(quantity);

		g_pJavaWrapper->ShowCasinoRoullete(quantity);

		break;
	}
	case RPC_SHOW_CASINO_BUY_CHIP:
	{
		uint8_t type;
		uint32_t balance;
		bs.Read(type);
		bs.Read(balance);

		g_pJavaWrapper->ShowCasinoChip(type, balance);
		break;
	}
	case RPC_OPEN_SETTINGS:
	{
		g_pJavaWrapper->ShowClientSettings();
		break;
	}
	case RPC_CUSTOM_HANDLING_DEFAULTS:
	{
		uint16_t vehId;
		bs.Read(vehId);

		if (GetVehiclePool())
		{
			CVehicle *pVeh = GetVehiclePool()->GetAt(vehId);
			if (pVeh)
			{
				pVeh->ResetVehicleHandling();
			}
		}

		break;
	}
	case RPC_CUSTOM_SET_LEVEL:
	{
		uint8_t current;
		uint8_t max;
		uint8_t level;
		bs.Read(current);
		bs.Read(max);
		bs.Read(level);
		CPlayerPool *pPlayerPool = GetPlayerPool();
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
		break;
	}
	case RPC_CUSTOM_SET_FUEL:
	{
		uint8_t fuel;
		uint8_t mileage;
		bs.Read(fuel);
		bs.Read(mileage);

		pGUI->SetFuel(fuel);
		pGUI->SetMeliage(mileage);
		break;
	}
	case RPC_TOGGLE_GPS_INFO:
	{
		uint8_t value;
		bs.Read(value);
		if (value == 1)
		{
			g_pJavaWrapper->ShowGPS();
		}
		else if (value == 0)
		{
			g_pJavaWrapper->HideGPS();
		}
		break;
	}
	case RPC_TOGGLE_GREENZONE:
	{
		uint8_t value;
		bs.Read(value);

		if (value == 1)
		{
			g_pJavaWrapper->ShowGreenZone();
			m_GreenZoneState = true;
		}
		else if (value == 0)
		{
			g_pJavaWrapper->HideGreenZone();
			m_GreenZoneState = false;
		}
		break;
	}
	case RPC_KILLLIST:
	{
		uint16_t killer;
		uint16_t killed;
		uint16_t weaponid;
		uint8_t killerteam;

		char *killerone;
		char *killedone;
		uint16_t weaponidone;
		uint8_t killerteamone;

		char *killertwo;
		char *killedtwo;
		uint16_t weaponidtwo;
		uint8_t killerteamtwo;

		char *killerthree;
		char *killedthree;
		uint16_t weaponidthree;
		uint8_t killerteamthree;

		bs.Read(killer);
		bs.Read(killed);
		bs.Read(weaponid);
		bs.Read(killerteam);

		killlistduels += 1;
		if (killlistduels > 3)
		{
			killlistduels = 4;
		}

		CPlayerPool *pPlayerPool = GetPlayerPool();
		if (pPlayerPool)
		{
			if (killlistduels == 1)
			{
				if(pPlayerPool->GetSlotState(killer))
				{
					killerone = pPlayerPool->GetPlayerName(killer);
				}
				else
				{
					killerone = "None";
				}
				if(pPlayerPool->GetSlotState(killed))
				{
					killedone = pPlayerPool->GetPlayerName(killed);
				}
				else
				{
					killedone = "None";
				}
				weaponidone = weaponid;
				killerteamone = killerteam;
			}
			else if (killlistduels == 2)
			{
				if(pPlayerPool->GetSlotState(killer))
				{
					killertwo = pPlayerPool->GetPlayerName(killer);
				}
				else
				{
					killertwo = "None";
				}
				if(pPlayerPool->GetSlotState(killed))
				{
					killedtwo = pPlayerPool->GetPlayerName(killed);
				}
				else
				{
					killedtwo = "None";
				}
				weaponidtwo = weaponid;
				killerteamtwo = killerteam;
			}
			else if (killlistduels == 3)
			{
				if(pPlayerPool->GetSlotState(killer))
				{
					killerthree = pPlayerPool->GetPlayerName(killer);
				}
				else
				{
					killerthree = "None";
				}
				if(pPlayerPool->GetSlotState(killed))
				{
					killedthree = pPlayerPool->GetPlayerName(killed);
				}
				else
				{
					killedthree = "None";
				}
				weaponidthree = weaponid;
				killerteamthree = killerteam;
			}

			if (killlistduels == 1)
			{
				g_pJavaWrapper->UpdateKilllistOne(killerteamone, weaponidone, killerone, killedone);
			}
			else if (killlistduels == 2)
			{
				g_pJavaWrapper->UpdateKilllistOne(killerteamone, weaponidone, killerone, killedone);
				g_pJavaWrapper->UpdateKilllistTwo(killerteamtwo, weaponidtwo, killertwo, killedtwo);
			}
			else if (killlistduels == 3)
			{
				g_pJavaWrapper->UpdateKilllistOne(killerteamone, weaponidone, killerone, killedone);
				g_pJavaWrapper->UpdateKilllistTwo(killerteamtwo, weaponidtwo, killertwo, killedtwo);
				g_pJavaWrapper->UpdateKilllistThree(killerteamthree, weaponidthree, killerthree, killedthree);
			}
			else if (killlistduels == 4)
			{
				killerone = killertwo;
				killedone = killedtwo;
				weaponidone = weaponidtwo;
				killerteamone = killerteamtwo;

				killertwo = killerthree;
				killedtwo = killedthree;
				weaponidtwo = weaponidthree;
				killerteamtwo = killerteamthree;

				if(pPlayerPool->GetSlotState(killer))
				{
					killerthree = pPlayerPool->GetPlayerName(killer);
				}
				else
				{
					killerthree = "None";
				}
				if(pPlayerPool->GetSlotState(killed))
				{
					killedthree = pPlayerPool->GetPlayerName(killed);
				}
				else
				{
					killedthree = "None";
				}
				weaponidthree = weaponid;
				killerteamthree = killerteam;

				g_pJavaWrapper->UpdateKilllistOne(killerteamone, weaponidone, killerone, killedone);
				g_pJavaWrapper->UpdateKilllistTwo(killerteamtwo, weaponidtwo, killertwo, killedtwo);
				g_pJavaWrapper->UpdateKilllistThree(killerteamthree, weaponidthree, killerthree, killedthree);
			}
		}

		break;
	}
	case RPC_CLEAR_KILLLIST:
	{
		g_pJavaWrapper->ClearKilllistDuels();
		killlistduels = 0;
		break;
	}
	case RPC_SHOW_DUELS_KILLS_LEFT:
	{
		uint8_t show;
		uint8_t kills;
		uint8_t max;

		bs.Read(show);
		bs.Read(kills);
		bs.Read(max);

		if (show == 1)
		{
			g_pJavaWrapper->ShowKilllist();
			g_pJavaWrapper->HideKilllistMulti();
			g_pJavaWrapper->ShowKilllisDuels();
			g_pJavaWrapper->UpdateKilllist(-1, -1, kills, max);
		}
		else
		{
			g_pJavaWrapper->HideKilllist();
		}

		break;
	}
	case RPC_DUELS_SHOW_LOCAL_TOP:
	{
		uint8_t show;
		uint16_t top1;
		uint16_t top2;
		uint16_t top3;

		bs.Read(show);
		bs.Read(top1);
		bs.Read(top2);
		bs.Read(top3);

		char *top1nick;
		char *top2nick;
		char *top3nick;

		CPlayerPool *pPlayerPool = GetPlayerPool();
		if (pPlayerPool)
		{
			if(pPlayerPool->GetSlotState(top1))
			{
				top1nick = pPlayerPool->GetPlayerName(top1);
			}
			else
			{
				top1nick = "";
			}
			if(pPlayerPool->GetSlotState(top2))
			{
				top2nick = pPlayerPool->GetPlayerName(top2);
			}
			else
			{
				top2nick = "";
			}
			if(pPlayerPool->GetSlotState(top2))
			{
				top3nick = pPlayerPool->GetPlayerName(top3);
			}
			else
			{
				top3nick = "";
			}
		}

		if (show == 1)
		{
			g_pJavaWrapper->ShowKilllist();
			g_pJavaWrapper->HideKilllistDuels();
			g_pJavaWrapper->ShowKilllistMulti();
			g_pJavaWrapper->UpdateKilllistTop(top1nick, top2nick, top3nick);
		}
		else
		{
			g_pJavaWrapper->HideKilllist();
		}

		break;
	}
	case RPC_DUELS_SHOW_LOCAL_STAT:
	{
		uint8_t show;
		uint16_t kills;
		uint16_t deaths;

		bs.Read(show);
		bs.Read(kills);
		bs.Read(deaths);

		if (show == 1)
		{
			g_pJavaWrapper->ShowKilllist();
			g_pJavaWrapper->HideKilllistDuels();
			g_pJavaWrapper->ShowKilllistMulti();
			g_pJavaWrapper->UpdateKilllist(kills, deaths, -1, -1);
		}
		else
		{
			g_pJavaWrapper->HideKilllist();
		}

		break;
	}
	case RPC_SHOW_ACTION_LABEL:
	{
		uint16_t type;
		char str[256];
		uint8_t len;
		uint8_t time;
		uint16_t actionId;
		bs.Read(type);
		bs.Read(len);
		bs.Read(&str[0], len);
		bs.Read(time);
		bs.Read(actionId);

		char text[256];
		cp1251_to_utf8(text, str, len);

		if (type != 65535)
		{
			g_pJavaWrapper->ShowNotification(type, (char*)text, time, (char*)"btnClick", (char*)"btn", actionId);
		}
		else
		{
			g_pJavaWrapper->HideNotification();
		}
		break;
	}
	case RPC_SHOW_TARGET_LABEL:
	{
		uint8_t value;
		char str[256];
		uint8_t len;
		bs.Read(len);
		bs.Read(&str[0], len);
		bs.Read(value);

		char text[256];
		cp1251_to_utf8(text, str);
		g_pJavaWrapper->UpdateTargetNotify(value, (char *)text);

		break;
	}
	case RPC_SHOW_FACTORY_GAME:
	{
		uint8_t show;
		uint32_t type;
		bs.Read(show);
		bs.Read(type);

		if (show == 1)
		{
			g_pJavaWrapper->ShowZavod(type);
		}
		else
		{
			g_pJavaWrapper->HideZavod();
		}

		break;
	}
	case RPC_SHOW_MINING_GAME:
	{
		uint8_t show;
		uint32_t type;
		bs.Read(show);
		bs.Read(type);

		if (show == 1)
		{
			g_pJavaWrapper->ShowMine(type);
		}
		else
		{
			g_pJavaWrapper->HideZavod();
		}

		break;
	}
	case RPC_SHOW_TD_BUS:
	{
		uint8_t show;
		uint32_t timer;
		bs.Read(show);
		bs.Read(timer);

		if (show == 1)
		{
			g_pJavaWrapper->ShowUpdateBus(timer);
		}
		else
		{
			g_pJavaWrapper->HideBus();
		}

		break;
	}
	case RPC_SHOW_DICE_TABLE:
	{
		uint8_t toggle;
		uint8_t tableid;
		uint32_t bet;
		uint32_t bank;
		uint16_t playerid1;
		uint8_t playerstatus1;
		uint16_t playerid2;
		uint8_t playerstatus2;
		uint16_t playerid3;
		uint8_t playerstatus3;
		uint16_t playerid4;
		uint8_t playerstatus4;
		uint16_t playerid5;
		uint8_t playerstatus5;

		char *strplayerid1;
		char *strplayerid2;
		char *strplayerid3;
		char *strplayerid4;
		char *strplayerid5;

		bs.Read(toggle);
		bs.Read(tableid);
		bs.Read(bet);
		bs.Read(bank);
		bs.Read(playerid1);
		bs.Read(playerstatus1);
		bs.Read(playerid2);
		bs.Read(playerstatus2);
		bs.Read(playerid3);
		bs.Read(playerstatus3);
		bs.Read(playerid4);
		bs.Read(playerstatus4);
		bs.Read(playerid5);
		bs.Read(playerstatus5);

		CPlayerPool *pPlayerPool = GetPlayerPool();
		if (pPlayerPool)
		{
			if(pPlayerPool->GetSlotState(playerid1))
			{
				strplayerid1 = pPlayerPool->GetPlayerName(playerid1);
			}
			else if (playerid1 == pPlayerPool->GetLocalPlayerID())
			{
				strplayerid1 = pPlayerPool->GetLocalPlayerName();
			}
			else
			{
				strplayerid1 = "--";
			}
			if(pPlayerPool->GetSlotState(playerid2))
			{
				strplayerid2 = pPlayerPool->GetPlayerName(playerid2);
			}
			else if (playerid2 == pPlayerPool->GetLocalPlayerID())
			{
				strplayerid2 = pPlayerPool->GetLocalPlayerName();
			}
			else
			{
				strplayerid2 = "--";
			}
			if(pPlayerPool->GetSlotState(playerid3))
			{
				strplayerid3 = pPlayerPool->GetPlayerName(playerid3);
			}
			else if (playerid3 == pPlayerPool->GetLocalPlayerID())
			{
				strplayerid3 = pPlayerPool->GetLocalPlayerName();
			}
			else
			{
				strplayerid3 = "--";
			}
			if(pPlayerPool->GetSlotState(playerid4))
			{
				strplayerid4 = pPlayerPool->GetPlayerName(playerid4);
			}
			else if (playerid4 == pPlayerPool->GetLocalPlayerID())
			{
				strplayerid4 = pPlayerPool->GetLocalPlayerName();
			}
			else
			{
				strplayerid4 = "--";
			}
			if(pPlayerPool->GetSlotState(playerid5))
			{
				strplayerid5 = pPlayerPool->GetPlayerName(playerid5);
			}
			else if (playerid5 == pPlayerPool->GetLocalPlayerID())
			{
				strplayerid5 = pPlayerPool->GetLocalPlayerName();
			}
			else
			{
				strplayerid5 = "--";
			}
		}
		
		if (toggle == 1)
		{
			g_pJavaWrapper->ShowUpdateCasino(tableid, bet, bank, pGame->GetLocalMoney(), strplayerid1, playerstatus1, strplayerid2, playerstatus2, strplayerid3, playerstatus3, strplayerid4, playerstatus4, strplayerid5, playerstatus5);
		}
		else
		{
			g_pJavaWrapper->HideCasino();
		}

		break;
	}
	case RPC_TOGGLE_SAMWILL_GAME:
	{
		uint8_t value;
		bs.Read(value);

		if (value == 1)
		{
		g_pJavaWrapper->HideHud();
		g_pJavaWrapper->HideServer();
		g_pJavaWrapper->HideSamwillMoney();
		g_pJavaWrapper->ShowSamwill();
		g_pJavaWrapper->UpdateSamwill(-1, 0, 2, 2, 2, 2, 2);
		g_pJavaWrapper->Samwillcheck1 = 2;
		g_pJavaWrapper->Samwillcheck2 = 2;
		g_pJavaWrapper->Samwillcheck3 = 2;
		g_pJavaWrapper->Samwillcheck4 = 2;
		g_pJavaWrapper->Samwillcheck5 = 2;
		int samwillpacket = 0;
		usleep(1500000);
		for (int i = 0; i < 1001; i++)
		{
			g_pJavaWrapper->UpdateSamwill(-1, i, g_pJavaWrapper->Samwillcheck1, g_pJavaWrapper->Samwillcheck2, g_pJavaWrapper->Samwillcheck3, g_pJavaWrapper->Samwillcheck4, g_pJavaWrapper->Samwillcheck5);
			
			if (i > 160 && g_pJavaWrapper->Samwillcheck1 != 1 && g_pJavaWrapper->Samwillcheck1 == 2)
			{
				g_pJavaWrapper->Samwillcheck1 = 0;
			}
			if (i > 340 && g_pJavaWrapper->Samwillcheck2 != 1 && g_pJavaWrapper->Samwillcheck2 == 2)
			{
				g_pJavaWrapper->Samwillcheck2 = 0;
			}
			if (i > 510 && g_pJavaWrapper->Samwillcheck3 != 1 && g_pJavaWrapper->Samwillcheck3 == 2)
			{
				g_pJavaWrapper->Samwillcheck3 = 0;
			}
			if (i > 690 && g_pJavaWrapper->Samwillcheck4 != 1 && g_pJavaWrapper->Samwillcheck4 == 2)
			{
				g_pJavaWrapper->Samwillcheck4 = 0;
			}
			if (i > 870 && g_pJavaWrapper->Samwillcheck5 != 1 && g_pJavaWrapper->Samwillcheck5 == 2)
			{
				g_pJavaWrapper->Samwillcheck5 = 0;
			}
			
			if (g_pJavaWrapper->Samwillclicked)
			{
				if (i >= 110 && i <= 160 && g_pJavaWrapper->Samwillcheck1 != 1)
				{
					g_pJavaWrapper->Samwillcheck1 = 1;
					samwillpacket += 1;
				}
				if (i >= 280 && i <= 340 && g_pJavaWrapper->Samwillcheck2 != 1)
				{
					g_pJavaWrapper->Samwillcheck2 = 1;
					samwillpacket += 1;
				}
				if (i >= 460 && i <= 510 && g_pJavaWrapper->Samwillcheck3 != 1)
				{
					g_pJavaWrapper->Samwillcheck3 = 1;
					samwillpacket += 1;
				}
				if (i >= 640 && i <= 690 && g_pJavaWrapper->Samwillcheck4 != 1)
				{
					g_pJavaWrapper->Samwillcheck4 = 1;
					samwillpacket += 1;
				}
				if (i >= 810 && i <= 870 && g_pJavaWrapper->Samwillcheck5 != 1)
				{
					g_pJavaWrapper->Samwillcheck5 = 1;
					samwillpacket += 1;
				}
				g_pJavaWrapper->Samwillclicked = false;
			}
			usleep(20000);
		}
		usleep(1500000);
		g_pJavaWrapper->ShowHud();
		g_pJavaWrapper->ShowServer(pSettings->GetReadOnly().szServer);
		g_pJavaWrapper->ShowSamwillMoney();
		g_pJavaWrapper->HideSamwill();
		SendCustomPacket(251, RPC_TOGGLE_SAMWILL_GAME, samwillpacket);
	}
		else if (value == 0)
		{
			g_pJavaWrapper->HideSamwill();
		}
		break;
	}
	case RPC_CUSTOM_VISUALS:
	{
		uint16_t vehId;
		uint8_t bLightsColor[3];
		int8_t bWheelAlignX;
		int8_t bWheelAlignY;
		int16_t sWheelOffsetXX;
		int16_t sWheelOffsetXY;
		uint8_t bToner[3];
		uint8_t bVinyls[2];
		uint16_t fWheelWidth;

		uint8_t bPlateType, bLen;
		char szText[30];
		char szRegion[10];
		memset(szText, 0, 30);
		memset(szRegion, 0, 10);

		bs.Read(vehId);
		bs.Read(bLightsColor[0]);
		bs.Read(bLightsColor[1]);
		bs.Read(bLightsColor[2]);
		bs.Read(fWheelWidth);
		bs.Read(bWheelAlignX);
		bs.Read(bWheelAlignY);
		bs.Read(sWheelOffsetXX);
		bs.Read(sWheelOffsetXY);
		bs.Read(bToner[0]);
		bs.Read(bToner[1]);
		bs.Read(bToner[2]);
		bs.Read(bVinyls[0]);
		bs.Read(bVinyls[1]);
		bs.Read(bPlateType);

		bs.Read(bLen);
		if (bLen >= sizeof(szText) - 1)
		{
			return;
		}
		bs.Read(&szText[0], bLen);

		bs.Read(bLen);
		if (bLen >= sizeof(szRegion) - 1)
		{
			return;
		}
		bs.Read(&szRegion[0], bLen);
		

		uint8_t bShadowColor[3];
		uint8_t bShadowSizeX, bShadowSizeY;
		char szName[32];

		memset(szName, 0, sizeof(szName));

		bs.Read(bShadowColor[0]);
		bs.Read(bShadowColor[1]);
		bs.Read(bShadowColor[2]);
		bs.Read(bShadowSizeX);
		bs.Read(bShadowSizeY);
		bs.Read(bLen);

		if (bLen >= sizeof(szName) + 1)
		{
			return;
		}

		bs.Read(szName, bLen);

		if (GetVehiclePool())
		{
			CVehicle *pVeh = GetVehiclePool()->GetAt(vehId);
			if (pVeh)
			{

				pVeh->SetCustomShadow(bShadowColor[0], bShadowColor[1], bShadowColor[1], (float)bShadowSizeX / 10.0f, (float)bShadowSizeY / 10.0f, szName);

				if (bLightsColor[0] != 0xFF || bLightsColor[1] != 0xFF || bLightsColor[2] != 0xFF)
				{
					pVeh->SetHeadlightsColor(bLightsColor[0], bLightsColor[1], bLightsColor[2]);
				}

				if (fWheelWidth)
				{
					pVeh->SetWheelWidth((float)fWheelWidth / 100.0f);
				}

				if (bWheelAlignX)
				{
					pVeh->SetWheelAlignment(0, (float)bWheelAlignX);
				}

				if (bWheelAlignY)
				{
					pVeh->SetWheelAlignment(1, (float)bWheelAlignY);
				}

				if (sWheelOffsetXX)
				{
					float fValueX = (float)((float)sWheelOffsetXX / 100.0f);
					pVeh->SetWheelOffset(0, fValueX);
					// pVeh->ProcessWheelsOffset();
				}
				if (sWheelOffsetXY)
				{
					float fValueX = (float)((float)sWheelOffsetXY / 100.0f);
					pVeh->SetWheelOffset(1, fValueX);
					// pVeh->ProcessWheelsOffset();
				}

				pVeh->ApplyToner(1, bToner[0]);
				pVeh->ApplyToner(2, bToner[1]);
				pVeh->ApplyToner(3, bToner[2]);
				pVeh->ApplyVinyls(bVinyls[0], bVinyls[1]);
				// pChatWindow->AddDebugMessage("%d %d %d %d %d", bToner[0], bToner[1], bToner[2], bVinyls[0], bVinyls[1]);
				if (bPlateType)
				{
					CCustomPlateManager::PushPlate(vehId, (uint32_t)bPlateType, szText, szRegion);
				}
			}
		}

		break;
	}
	/*
	case RPC_CUSTOM_ACTOR_PUT_IN_VEH:
	{
		uint16_t actorId;
		VEHICLEID vehicleId;
		uint8_t seat;

		bs.Read(actorId);
		bs.Read(vehicleId);
		bs.Read(seat);
#ifdef _CDEBUG
		pChatWindow->AddDebugMessage("Put actor %d to %d in %d", actorId, vehicleId, seat);
#endif
		CActorPool *pActorPool = GetActorPool();
		if (pActorPool && GetVehiclePool())
		{
			if (pActorPool->GetAt(actorId) && GetVehiclePool()->GetAt(vehicleId))
			{
				int iCarID = GetVehiclePool()->FindGtaIDFromID((int)vehicleId);

				pActorPool->GetAt(actorId)->PutDirectlyInVehicle(iCarID, (int)seat);
			}
		}
		break;
	}
	case RPC_CUSTOM_ACTOR_REMOVE_VEH:
	{
		uint16_t actorId;
		bs.Read(actorId);

		CActorPool *pActorPool = GetActorPool();
		if (pActorPool)
		{
			if (pActorPool->GetAt(actorId))
			{
				pActorPool->GetAt(actorId)->RemoveFromVehicle();
			}
		}
		break;
	}
	case RPC_CUSTOM_ACTOR_ADD_ADDITIONAL:
	{
		uint16_t actorId;
		VEHICLEID vehicleId;
		uint8_t seat;

		bs.Read(actorId);
		bs.Read(vehicleId);
		bs.Read(seat);

		if (GetActorPool() && GetVehiclePool())
		{
			if (GetActorPool()->GetAt(actorId) && GetVehiclePool()->GetAt(vehicleId))
			{
				int iCarID = GetVehiclePool()->FindGtaIDFromID((int)vehicleId);

				GetActorPool()->GetAt(actorId)->PutDirectlyInVehicle(iCarID, (int)seat);
			}
		}
		break;
	}
	*/
	case RPC_CUSTOM_HANDLING:
	{
		uint16_t veh;
		uint8_t value;
		bs.Read(veh);
		bs.Read(value);
		std::vector<SHandlingData> comps;
		for (uint8_t i = 0; i < value; i++)
		{
			uint8_t id;
			float fvalue;
			bs.Read(id);
			bs.Read(fvalue);
			comps.push_back(SHandlingData(id, fvalue, 0));
			Log("Pushed %d %f", id, fvalue);
		}
		if (m_pVehiclePool)
		{
			if (m_pVehiclePool->GetAt(veh))
			{
				m_pVehiclePool->GetAt(veh)->SetHandlingData(comps);
			}
		}
		break;
	}
	case RPC_CUSTOM_COMPONENT:
	{
		uint16_t veh, extra_comp;
		uint8_t comp;
		bs.Read(veh);
		CVehicle *pVehicle = nullptr;

		if (m_pVehiclePool)
		{
			pVehicle = m_pVehiclePool->GetAt(veh);
		}
		if (!pVehicle)
		{
			return;
		}
		for (int i = 0; i < E_CUSTOM_COMPONENTS::ccMax; i++)
		{
			if (i == E_CUSTOM_COMPONENTS::ccExtra)
			{
				bs.Read(extra_comp);
				pVehicle->SetComponentVisible(i, (uint16_t)extra_comp);
			}
			else
			{
				bs.Read(comp);
				pVehicle->SetComponentVisible(i, (uint16_t)comp);
			}
		}
		break;
	}
	case RPC_TOGGLE_HUD_ELEMENT:
	{
		uint32_t hud, toggle;
		bs.Read(hud);
		bs.Read(toggle);
		if (hud == 7)
		{
			if (toggle == -1)
			{
				g_pJavaWrapper->HideSamwillMoney();
			}
			else
			{
				g_pJavaWrapper->ShowSamwillMoney();
				g_pJavaWrapper->UpdateSamwillMoney(toggle);
			}
		}
		// pChatWindow->AddDebugMessage("hud %d toggle %d", hud, toggle);
		pGame->ToggleHUDElement(hud, toggle);
		pGame->HandleChangedHUDStatus();
		break;
	}
	case RPC_STREAM_CREATE:
	{
		char str[255];
		uint8_t len;
		uint16_t id, vw, interior;
		VECTOR pos;
		float fDistance;
		bs.Read(id);
		bs.Read(pos.X);
		bs.Read(pos.Y);
		bs.Read(pos.Z);
		bs.Read(fDistance);
		bs.Read(vw);
		bs.Read(interior);
		bs.Read(len);
		bs.Read(&str[0], len);
		str[len] = 0;
		// pChatWindow->AddDebugMessage("%d %f %f %f %f %d %d %d %s", id, pos.X, pos.Y, pos.Z, fDistance, vw, interior, len, str);
		GetStreamPool()->AddStream(id, &pos, vw, interior, fDistance, (const char *)&str[0]);
		break;
	}
	case RPC_STREAM_INDIVIDUAL:
	{
		char str[255];
		uint8_t len;
		bs.Read(len);
		bs.Read(&str[0], len);
		str[len] = 0;
		// pChatWindow->AddDebugMessage("%s", str);
		// pChatWindow->AddDebugMessage("Playing audiostream %s", str);
		GetStreamPool()->PlayIndividualStream(&str[0]);
		break;
	}
	case RPC_STREAM_POS:
	{
		/*		uint16_t id;
				VECTOR pos;
				bs.Read(id);
				bs.Read(pos.X);
				bs.Read(pos.Y);
				bs.Read(pos.Z);
				if (GetStreamPool()->GetStream(id))
				{
					GetStreamPool()->GetStream(id)->SetPosition(pos);
				}*/
		break;
	}
	case RPC_STREAM_DESTROY:
	{
		uint32_t id;
		bs.Read(id);
		// pChatWindow->AddDebugMessage("%d", id);
		m_pStreamPool->DeleteStreamByID(id);
		break;
	}
	case RPC_STREAM_VOLUME:
	{
		uint16_t id;
		float fVolume;
		bs.Read(id);
		bs.Read(fVolume);
		// pChatWindow->AddDebugMessage("%d %f", id, fVolume);
		m_pStreamPool->SetStreamVolume(id, fVolume);
		break;
	}
	case RPC_STREAM_ISENABLED:
	{
		uint32_t isEnabled;
		bs.Read(isEnabled);
		if (isEnabled)
		{
			m_pStreamPool->Activate();
		}
		else if (isEnabled == 0)
		{
			m_pStreamPool->Deactivate();
		}
		break;
	}
	case RPC_CHECK_CLIENT:
	{
		char recievKey[17];
		uint16_t recievKey_len;
		bs.Read(recievKey_len);
		bs.Read(recievKey, recievKey_len);

		recievKey[recievKey_len] = '\0';

		char key_with_salt[recievKey_len+ strlen(AUTH_SALT)+1];
		strcpy(key_with_salt, recievKey);
		strcat(key_with_salt, AUTH_SALT);


		SendCheckClientPacket(sha256(key_with_salt).c_str());
		break;
	}
	case RPC_OPEN_LINK:
	{
		uint16_t len;
		bs.Read(len);

		if (len >= 0xFA)
		{
			break;
		}

		char link[0xFF];
		memset(&link[0], 0, 0xFF);
		bs.Read((char *)&link, len);
		AND_OpenLink(&link[0]);

		break;
	}
	case RPC_TIMEOUT_CHAT:
	{
		uint32_t timeoutStart = 0;
		uint32_t timeoutEnd = 0;
		bs.Read(timeoutStart);
		bs.Read(timeoutEnd);

		if (pChatWindow)
		{
			pChatWindow->SetChatDissappearTimeout(timeoutStart, timeoutEnd);
		}

		break;
	}
	}
}
void CNetGame::ResetVehiclePool()
{
	Log("ResetVehiclePool");
	if (m_pVehiclePool)
		delete m_pVehiclePool;

	m_pVehiclePool = new CVehiclePool();
}

void CNetGame::ResetObjectPool()
{
	Log("ResetObjectPool");
	if (m_pObjectPool)
		delete m_pObjectPool;

	m_pObjectPool = new CObjectPool();
}

void CNetGame::ResetPickupPool()
{
	Log("ResetPickupPool");
	if (m_pPickupPool)
		delete m_pPickupPool;

	m_pPickupPool = new CPickupPool();
}

void CNetGame::ResetGangZonePool()
{
	Log("ResetGangZonePool");
	if (m_pGangZonePool)
		delete m_pGangZonePool;

	m_pGangZonePool = new CGangZonePool();
}

void CNetGame::ResetLabelPool()
{
	Log("ResetLabelPool");
	if (m_pLabelPool)
		delete m_pLabelPool;

	m_pLabelPool = new CText3DLabelsPool();
}

void CNetGame::ResetActorPool()
{
	Log("ResetActorPool");
	if (m_pActorPool)
	{
		delete m_pActorPool;
	}
	m_pActorPool = new CActorPool();
}

void CNetGame::ResetTextDrawPool()
{
	Log("ResetTextDrawPool");
	if (m_pTextDrawPool)
	{
		delete m_pTextDrawPool;
	}
	m_pTextDrawPool = new CTextDrawPool();
}

extern int RemoveModelIDs[1200];
extern VECTOR RemovePos[1200];
extern float RemoveRad[1200];
extern int iTotalRemovedObjects;

void CNetGame::ShutDownForGameRestart()
{
	iTotalRemovedObjects = 0;
	for (int i = 0; i < MAX_REMOVE_MODELS; i++)
	{
		RemoveModelIDs[i] = -1;
	}
	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		CRemotePlayer *pPlayer = m_pPlayerPool->GetAt(playerId);
		if (pPlayer)
		{
			// pPlayer->SetTeam(NO_TEAM);
			// pPlayer->ResetAllSyncAttributes();
		}
	}

	CLocalPlayer *pLocalPlayer = m_pPlayerPool->GetLocalPlayer();
	if (pLocalPlayer)
	{
		pLocalPlayer->ResetAllSyncAttributes();
		pLocalPlayer->ToggleSpectating(false);
	}

	m_iGameState = GAMESTATE_RESTARTING;

	// m_pPlayerPool->DeactivateAll();
	m_pPlayerPool->Process();

	ResetVehiclePool();
	ResetObjectPool();
	ResetPickupPool();
	ResetGangZonePool();
	ResetLabelPool();
	ResetTextDrawPool();
	ResetActorPool();

	m_bDisableEnterExits = false;
	m_fNameTagDrawDistance = 60.0f;
	m_byteWorldTime = 12;
	m_byteWorldMinute = 0;
	m_byteWeather = 1;
	m_bHoldTime = true;
	m_bNameTagLOS = true;
	m_bUseCJWalk = false;
	m_fGravity = 0.008f;
	m_iDeathDropMoney = 0;

	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	if (pPlayerPed)
	{
		pPlayerPed->SetInterior(0);
		// pPlayerPed->SetDead();
		pPlayerPed->SetArmour(0.0f);
	}

	pGame->ToggleCJWalk(false);
	pGame->ResetLocalMoney();
	pGame->EnableZoneNames(false);
	m_bZoneNames = false;
	GameResetRadarColors();
	pGame->SetGravity(m_fGravity);
	pGame->EnableClock(false);
}

void CNetGame::SendCustomPacket(uint8_t packet, uint8_t RPC, uint8_t Quantity)
{
	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(Quantity);
	GetRakClient()->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void CNetGame::SendCustomCasinoChipPacket(uint8_t packet, uint8_t RPC, uint8_t type, uint8_t button, uint32_t money)
{
	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(type);
	bsSend.Write(button);
	bsSend.Write(money);
	pChatWindow->AddDebugMessage("packet: %d rpc: %d type: %d button: %d money: %d", packet, RPC, type, button, money);
	GetRakClient()->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void CNetGame::SendLoginPacket(char *password)
{
	uint8_t packet = PACKET_AUTH;
	uint8_t RPC = RPC_TOGGLE_LOGIN;
	uint8_t bytePasswordLen = strlen(password);
	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(bytePasswordLen);
	bsSend.Write(password, bytePasswordLen);
	GetRakClient()->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void CNetGame::SendRegisterPacket(char *password, char *mail, uint8_t sex, uint8_t skin)
{
	uint8_t packet = PACKET_AUTH;
	uint8_t RPC = RPC_TOGGLE_REGISTER;
	uint8_t bytePasswordLen = strlen(password);
	uint8_t byteMailLen = strlen(mail);
	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(bytePasswordLen);
	bsSend.Write(byteMailLen);
	bsSend.Write(password, bytePasswordLen);
	bsSend.Write(mail, byteMailLen);
	bsSend.Write(sex);
	bsSend.Write(skin);
	GetRakClient()->Send(&bsSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0);
}

void CNetGame::SendChatMessage(const char *szMsg)
{
	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	RakNet::BitStream bsSend;
	uint8_t byteTextLen = strlen(szMsg);

	bsSend.Write(byteTextLen);
	bsSend.Write(szMsg, byteTextLen);

	m_pRakClient->RPC(&RPC_Chat, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void CNetGame::SendChatCommand(const char *szCommand)
{
	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	RakNet::BitStream bsParams;
	int iStrlen = strlen(szCommand);

	bsParams.Write(iStrlen);
	bsParams.Write(szCommand, iStrlen);
	m_pRakClient->RPC(&RPC_ServerCommand, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void CNetGame::SendDialogResponse(uint16_t wDialogID, uint8_t byteButtonID, uint16_t wListBoxItem, char *szInput)
{
	uint8_t respLen = strlen(szInput);

	RakNet::BitStream bsSend;
	bsSend.Write(wDialogID);
	bsSend.Write(byteButtonID);
	bsSend.Write(wListBoxItem);
	bsSend.Write(respLen);
	bsSend.Write(szInput, respLen);
	m_pRakClient->RPC(&RPC_DialogResponse, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void CNetGame::SetMapIcon(uint8_t byteIndex, float fX, float fY, float fZ, uint8_t byteIcon, int iColor, int style)
{
	if (byteIndex >= 100)
		return;
	if (m_dwMapIcons[byteIndex])
		DisableMapIcon(byteIndex);

	m_dwMapIcons[byteIndex] = pGame->CreateRadarMarkerIcon(byteIcon, fX, fY, fZ, iColor, style);
}

void CNetGame::DisableMapIcon(uint8_t byteIndex)
{
	if (byteIndex >= 100)
		return;
	ScriptCommand(&disable_marker, m_dwMapIcons[byteIndex]);
	m_dwMapIcons[byteIndex] = 0;
}

void CNetGame::UpdatePlayerScoresAndPings()
{

	static uint32_t dwLastUpdateTick = 0;

	if ((GetTickCount() - dwLastUpdateTick) >= 3000)
	{
		dwLastUpdateTick = GetTickCount();
		RakNet::BitStream bsParams;
		m_pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
	}
}

void gen_auth_key(char buf[260], char *auth_in);
void CNetGame::Packet_AuthKey(Packet *pkt)
{
	RakNet::BitStream bsAuth((unsigned char *)pkt->data, pkt->length, false);

	uint8_t byteAuthLen;
	char szAuth[260];

	bsAuth.IgnoreBits(8);
	bsAuth.Read(byteAuthLen);
	bsAuth.Read(szAuth, byteAuthLen);
	szAuth[byteAuthLen] = '\0';

	char szAuthKey[260];
	gen_auth_key(szAuthKey, szAuth);

	RakNet::BitStream bsKey;
	uint8_t byteAuthKeyLen = (uint8_t)strlen(szAuthKey);

	bsKey.Write((uint8_t)ID_AUTH_KEY);
	bsKey.Write((uint8_t)byteAuthKeyLen);
	bsKey.Write(szAuthKey, byteAuthKeyLen);
	m_pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE, 0);

	Log("[AUTH] %s -> %s", szAuth, szAuthKey);
}

void CNetGame::Packet_DisconnectionNotification(Packet *pkt)
{
	if (pChatWindow)
		pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::SERVER_CLOSED_CONNECTION));
	m_pRakClient->Disconnect(2000);
}

void CNetGame::Packet_ConnectionLost(Packet *pkt)
{
	if (m_pRakClient)
		m_pRakClient->Disconnect(0);

	if (pChatWindow)
		pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::CONNECTION_LOST));
	ShutDownForGameRestart();

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		CRemotePlayer *pPlayer = m_pPlayerPool->GetAt(playerId);
		if (pPlayer)
			m_pPlayerPool->Delete(playerId, 0);
	}

	SetGameState(GAMESTATE_WAIT_CONNECT);
}
//#define SUM_MAS_ENCR	10
// int g_sumMas[SUM_MAS_ENCR] = { 290, 291, 417, 424, 477, 54+38+142+49, 51+91+91+84, 54+38+142+50, 54 + 38 + 142 + 51, 51 + 77 + 238 + 92 };

#include "..//CServerManager.h"

bool g_isValidSum(int a)
{
	for (int i = 0; i < MAX_SERVERS; i++)
	{
		if (g_sEncryptedAddresses[i].getSum() == a)
			return true;
	}
	return false;
}
#include <sstream>
void WriteVerified1();
void CNetGame::Packet_ConnectionSucceeded(Packet *pkt)
{
	if (pChatWindow)
		pChatWindow->AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::CONNECTED));
	SetGameState(GAMESTATE_AWAIT_JOIN);

	RakNet::BitStream bsSuccAuth((unsigned char *)pkt->data, pkt->length, false);
	PLAYERID MyPlayerID;
	unsigned int uiChallenge;

	bsSuccAuth.IgnoreBits(8);  // ID_CONNECTION_REQUEST_ACCEPTED
	bsSuccAuth.IgnoreBits(32); // binaryAddress
	bsSuccAuth.IgnoreBits(16); // port
	bsSuccAuth.Read(MyPlayerID);
	bsSuccAuth.Read(uiChallenge);
	char ip[0x7F];
	strncpy(ip, m_szHostOrIp, sizeof(ip));

	std::vector<std::string> strings;
	std::istringstream f((const char *)&ip[0]);
	std::string s;
	int sum = 0;
	while (getline(f, s, '.'))
	{
		sum += std::atoi(s.c_str());
	}

	if (g_isValidSum(sum))
	{
		WriteVerified1();
	}
	m_pPlayerPool->SetLocalPlayerID(MyPlayerID);

	int iVersion = NETGAME_VERSION;
	char byteMod = 0x01;
	unsigned int uiClientChallengeResponse = uiChallenge ^ iVersion;

	char byteAuthBSLen = (char)strlen(AUTH_BS);
	char byteNameLen = (char)strlen(m_pPlayerPool->GetLocalPlayerName());
	char byteClientverLen = (char)strlen(SAMP_VERSION);

	RakNet::BitStream bsSend;
	bsSend.Write(iVersion);
	bsSend.Write(byteMod);
	bsSend.Write(byteNameLen);
	bsSend.Write(m_pPlayerPool->GetLocalPlayerName(), byteNameLen);
	bsSend.Write(uiClientChallengeResponse);
	bsSend.Write(byteAuthBSLen);
	bsSend.Write(AUTH_BS, byteAuthBSLen);
	bsSend.Write(byteClientverLen);
	bsSend.Write(SAMP_VERSION, byteClientverLen);

	CClientInfo::WriteClientInfoToBitStream(bsSend);

	m_pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}
void CNetGame::Packet_PlayerSync(Packet *pkt)
{
	CRemotePlayer *pPlayer;
	RakNet::BitStream bsPlayerSync((unsigned char *)pkt->data, pkt->length, false);
	ONFOOT_SYNC_DATA ofSync;
	uint8_t bytePacketID = 0;
	PLAYERID playerId;

	bool bHasLR, bHasUD;
	bool bHasVehicleSurfingInfo;

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));

	bsPlayerSync.Read(bytePacketID);
	bsPlayerSync.Read(playerId);

	// LEFT/RIGHT KEYS
	bsPlayerSync.Read(bHasLR);
	if (bHasLR)
		bsPlayerSync.Read(ofSync.lrAnalog);

	// UP/DOWN KEYS
	bsPlayerSync.Read(bHasUD);
	if (bHasUD)
		bsPlayerSync.Read(ofSync.udAnalog);

	// GENERAL KEYS
	bsPlayerSync.Read(ofSync.wKeys);

	// VECTOR POS
	bsPlayerSync.Read((char *)&ofSync.vecPos, sizeof(VECTOR));

	// QUATERNION
	float tw, tx, ty, tz;
	bsPlayerSync.ReadNormQuat(tw, tx, ty, tz);
	ofSync.quat.w = tw;
	ofSync.quat.x = tx;
	ofSync.quat.y = ty;
	ofSync.quat.z = tz;

	// HEALTH/ARMOUR (COMPRESSED INTO 1 BYTE)
	uint8_t byteHealthArmour;
	uint8_t byteArmTemp = 0, byteHlTemp = 0;

	bsPlayerSync.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if (byteArmTemp == 0xF)
		ofSync.byteArmour = 100;
	else if (byteArmTemp == 0)
		ofSync.byteArmour = 0;
	else
		ofSync.byteArmour = byteArmTemp * 7;

	if (byteHlTemp == 0xF)
		ofSync.byteHealth = 100;
	else if (byteHlTemp == 0)
		ofSync.byteHealth = 0;
	else
		ofSync.byteHealth = byteHlTemp * 7;

	// CURRENT WEAPON
	bsPlayerSync.Read(ofSync.byteCurrentWeapon);
	// SPECIAL ACTION
	bsPlayerSync.Read(ofSync.byteSpecialAction);

	// READ MOVESPEED VECTORS
	bsPlayerSync.ReadVector(tx, ty, tz);
	ofSync.vecMoveSpeed.X = tx;
	ofSync.vecMoveSpeed.Y = ty;
	ofSync.vecMoveSpeed.Z = tz;

	bsPlayerSync.Read(bHasVehicleSurfingInfo);
	if (bHasVehicleSurfingInfo)
	{
		bsPlayerSync.Read(ofSync.wSurfInfo);
		bsPlayerSync.Read(ofSync.vecSurfOffsets.X);
		bsPlayerSync.Read(ofSync.vecSurfOffsets.Y);
		bsPlayerSync.Read(ofSync.vecSurfOffsets.Z);
	}
	else
		ofSync.wSurfInfo = INVALID_VEHICLE_ID;

	bool bHasAnimInfo;
	ofSync.dwAnimation = 0;
	bsPlayerSync.Read(bHasAnimInfo);
	if (bHasAnimInfo)
	{
		bsPlayerSync.Read(ofSync.dwAnimation);
	}
	else
	{
		ofSync.dwAnimation = 0b10000000000000000000000000000000;
	}

	uint8_t key = 0;

	if (m_pPlayerPool)
	{
		pPlayer = m_pPlayerPool->GetAt(playerId);
		if (pPlayer)
			pPlayer->StoreOnFootFullSyncData(&ofSync, 0, key);
	}
}

void CNetGame::Packet_VehicleSync(Packet *pkt)
{
	CRemotePlayer *pPlayer;
	RakNet::BitStream bsSync((unsigned char *)pkt->data, pkt->length, false);
	uint8_t bytePacketID = 0;
	PLAYERID playerId;
	INCAR_SYNC_DATA icSync;

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	memset(&icSync, 0, sizeof(INCAR_SYNC_DATA));

	bsSync.Read(bytePacketID);
	bsSync.Read(playerId);
	bsSync.Read(icSync.VehicleID);

	// keys
	bsSync.Read(icSync.lrAnalog);
	bsSync.Read(icSync.udAnalog);
	bsSync.Read(icSync.wKeys);

	// quaternion
	bsSync.ReadNormQuat(
		icSync.quat.w,
		icSync.quat.x,
		icSync.quat.y,
		icSync.quat.z);

	// position
	bsSync.Read((char *)&icSync.vecPos, sizeof(VECTOR));

	// speed
	bsSync.ReadVector(
		icSync.vecMoveSpeed.X,
		icSync.vecMoveSpeed.Y,
		icSync.vecMoveSpeed.Z);

	// vehicle health
	uint16_t wTempVehicleHealth;
	bsSync.Read(wTempVehicleHealth);
	icSync.fCarHealth = (float)wTempVehicleHealth;

	// health/armour
	uint8_t byteHealthArmour;
	uint8_t byteArmTemp = 0, byteHlTemp = 0;

	bsSync.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if (byteArmTemp == 0xF)
		icSync.bytePlayerArmour = 100;
	else if (byteArmTemp == 0)
		icSync.bytePlayerArmour = 0;
	else
		icSync.bytePlayerArmour = byteArmTemp * 7;

	if (byteHlTemp == 0xF)
		icSync.bytePlayerHealth = 100;
	else if (byteHlTemp == 0)
		icSync.bytePlayerHealth = 0;
	else
		icSync.bytePlayerHealth = byteHlTemp * 7;

	// CURRENT WEAPON
	uint8_t byteTempWeapon;
	bsSync.Read(byteTempWeapon);
	icSync.byteCurrentWeapon ^= (byteTempWeapon ^ icSync.byteCurrentWeapon) & 0x3F;

	bool bCheck;

	// siren
	bsSync.Read(bCheck);
	if (bCheck)
		icSync.byteSirenOn = 1;
	// landinggear
	bsSync.Read(bCheck);
	if (bCheck)
		icSync.byteLandingGearState = 1;
	// train speed
	bsSync.Read(bCheck);
	if (bCheck)
		bsSync.Read(icSync.fTrainSpeed);
	// triler id
	bsSync.Read(bCheck);
	if (bCheck)
		bsSync.Read(icSync.TrailerID);

	if (m_pPlayerPool)
	{
		pPlayer = m_pPlayerPool->GetAt(playerId);
		if (pPlayer)
		{
			pPlayer->StoreInCarFullSyncData(&icSync, 0);
		}
	}
}

void CNetGame::Packet_PassengerSync(Packet *pkt)
{
	CRemotePlayer *pPlayer;
	uint8_t bytePacketID;
	PLAYERID playerId;
	PASSENGER_SYNC_DATA psSync;

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	RakNet::BitStream bsPassengerSync((unsigned char *)pkt->data, pkt->length, false);
	bsPassengerSync.Read(bytePacketID);
	bsPassengerSync.Read(playerId);
	bsPassengerSync.Read((char *)&psSync, sizeof(PASSENGER_SYNC_DATA));

	if (m_pPlayerPool)
	{
		pPlayer = m_pPlayerPool->GetAt(playerId);
		if (pPlayer)
			pPlayer->StorePassengerFullSyncData(&psSync);
	}
}

void CNetGame::Packet_MarkersSync(Packet *pkt)
{
	CRemotePlayer *pPlayer;
	int iNumberOfPlayers = 0;
	PLAYERID playerId;
	short sPos[3];
	bool bIsPlayerActive;
	uint8_t unk0 = 0;

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	RakNet::BitStream bsMarkersSync((unsigned char *)pkt->data, pkt->length, false);
	bsMarkersSync.Read(unk0);
	bsMarkersSync.Read(iNumberOfPlayers);

	if (iNumberOfPlayers)
	{
		for (int i = 0; i < iNumberOfPlayers; i++)
		{
			bsMarkersSync.Read(playerId);
			bsMarkersSync.ReadCompressed(bIsPlayerActive);

			if (bIsPlayerActive)
			{
				bsMarkersSync.Read(sPos[0]);
				bsMarkersSync.Read(sPos[1]);
				bsMarkersSync.Read(sPos[2]);
			}

			if (playerId < MAX_PLAYERS && m_pPlayerPool->GetSlotState(playerId))
			{
				pPlayer = m_pPlayerPool->GetAt(playerId);
				if (pPlayer)
				{
					if (bIsPlayerActive)
						pPlayer->ShowGlobalMarker(sPos[0], sPos[1], sPos[2]);
					else
						pPlayer->HideGlobalMarker();
				}
			}
		}
	}
}

void CNetGame::Packet_BulletSync(Packet *p)
{
	uint8_t bytePacketID;
	uint16_t PlayerID;
	BULLET_SYNC_DATA btSync;
	RakNet::BitStream bsBulletSync((unsigned char *)p->data, p->length, false);

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	bsBulletSync.Read(bytePacketID);
	bsBulletSync.Read(PlayerID);
	bsBulletSync.Read((char *)&btSync, sizeof(BULLET_SYNC_DATA));

	CRemotePlayer *pRemotePlayer = m_pPlayerPool->GetAt(PlayerID);
	if (pRemotePlayer)
	{
		pRemotePlayer->StoreBulletSyncData(&btSync);
	}
}

void CNetGame::Packet_AimSync(Packet *p)
{
	CRemotePlayer *pPlayer;
	RakNet::BitStream bsAimSync((unsigned char *)p->data, p->length, false);
	AIM_SYNC_DATA aimSync;
	uint8_t bytePacketID = 0;
	uint16_t bytePlayerID = 0;

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	bsAimSync.Read(bytePacketID);
	bsAimSync.Read(bytePlayerID);
	bsAimSync.Read((char *)&aimSync, sizeof(AIM_SYNC_DATA));

	pPlayer = GetPlayerPool()->GetAt(bytePlayerID);

	if (pPlayer)
	{
		pPlayer->UpdateAimFromSyncData(&aimSync);
	}
}

void CNetGame::SendCheckClientPacket(const char password[])
{
	uint8_t packet = PACKET_CUSTOM;
	uint8_t RPC = RPC_CHECK_CLIENT;
	uint16_t bytePasswordLen = strlen(password);
	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(bytePasswordLen);
	bsSend.Write(password, bytePasswordLen);
	GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, UNRELIABLE_SEQUENCED, 0);

	pChatWindow->AddDebugMessage("key: %s", password);
}