#include "../main.h"
#include "../game/game.h"
#include "..//util/CJavaWrapper.h"
#include "netgame.h"
#include "java_systems/CHUD.h"
#include "CSettings.h"
#include <thread>
#include <chrono>

extern CWidgetManager* g_pWidgetManager;
extern CNetGame *pNetGame;

#include "../chatwindow.h"

#include "..//CLocalisation.h"
#include "../vendor/hash/sha256.h"

#define NETGAME_VERSION 4057
#define AUTH_BS "E02262CF28BC542486C558D4BE9EFB716592AFAF8B"
//#define AUTH_BS "1528354F18550C00AB504591304D0379BB0ACA99043"

extern CGame *pGame;

int iVehiclePoolProcessFlag = 0;
int iPickupPoolProcessFlag = 0;

void RegisterRPCs(RakClientInterface* pRakClient);
void UnRegisterRPCs(RakClientInterface* pRakClient);
void RegisterScriptRPCs(RakClientInterface* pRakClient);
void UnRegisterScriptRPCs(RakClientInterface* pRakClient);

unsigned char GetPacketID(Packet *p)
{
	if(p == 0) return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
		return (unsigned char) p->data[sizeof(unsigned char) + sizeof(unsigned long)];
	else
		return (unsigned char) p->data[0];
}

class string_encryptor;

CNetGame::CNetGame(const char* szHostOrIp, int iPort, const char* szPlayerName, const char* szPass)
{
	strcpy(m_szHostName, "San Andreas Multiplayer");
	strncpy(m_szHostOrIp, szHostOrIp, sizeof(m_szHostOrIp));
	m_iPort = iPort;

	m_pPlayerPool = new CPlayerPool();
	m_pPlayerPool->SetLocalPlayerName(szPlayerName);
//
	m_pVehiclePool = new CVehiclePool();
	m_pObjectPool = new CObjectPool();
	m_pPickupPool = new CPickupPool();
	m_pGangZonePool = new CGangZonePool();
	m_pLabelPool = new CText3DLabelsPool();
//
//	m_pTextDrawPool = new CTextDrawPool();
	g_pWidgetManager = new CWidgetManager();
	m_pStreamPool = new CStreamPool();
	m_pActorPool = new CActorPool();

	m_pRakClient = RakNetworkFactory::GetRakClientInterface();
	RegisterRPCs(m_pRakClient);
	RegisterScriptRPCs(m_pRakClient);
	// key

	m_pRakClient->SetPassword(szPass);

	m_dwLastConnectAttempt = GetTickCount();
	m_iGameState = 	GAMESTATE_WAIT_CONNECT;

	m_GreenZoneState = false;
	m_iSpawnsAvailable = 0;
	m_byteWorldMinute = 0;
	m_byteWorldTime = 12;
	m_byteWeather =	10;
	m_fGravity = (float)0.008000000;
	m_bUseCJWalk = false;
	m_bDisableEnterExits = false;
	m_fNameTagDrawDistance = 60.0f;
	m_bZoneNames = false;
	m_bInstagib = false;
	m_iDeathDropMoney = 0;
	m_bNameTagLOS = false;

	for(int i=0; i<100; i++)
		m_dwMapIcons[i] = 0;
}

CNetGame::~CNetGame()
{
	m_pRakClient->Disconnect(0);
	UnRegisterRPCs(m_pRakClient);
	UnRegisterScriptRPCs(m_pRakClient);
	RakNetworkFactory::DestroyRakClientInterface(m_pRakClient);
	m_pRakClient = nullptr;

	if(m_pPlayerPool) 
	{
		delete m_pPlayerPool;
		m_pPlayerPool = nullptr;
	}

	if(m_pVehiclePool)
	{
		delete m_pVehiclePool;
		m_pVehiclePool = nullptr;
	}

	if(m_pPickupPool)
	{
		delete m_pPickupPool;
		m_pPickupPool = nullptr;
	}

	if(m_pGangZonePool)
	{
		delete m_pGangZonePool;
		m_pGangZonePool = nullptr;
	}

	if(m_pLabelPool)
	{
		delete m_pLabelPool;
		m_pLabelPool = nullptr;
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

    if(!pGame->bIsGameExiting) g_pJavaWrapper->ClearScreen();

}

#include "CUDPSocket.h"
#include "..//CServerManager.h"
#include "java_systems/CSpeedometr.h"

int last_process_cnetgame = 0;
void CNetGame::Process()
{

	// 30 fps
	if (GetTickCount() - last_process_cnetgame >= 1000 / 30){
		last_process_cnetgame = GetTickCount();
	}else {
		return;
	}
	//CSkyBox::Process();

	CSpeedometr::update();

	UpdateNetwork();

	// server checkpoints update
	if(pGame->m_bCheckpointsEnabled) {
		CPlayerPed *pPlayerDed = m_pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		if (pPlayerDed)
		{
			ScriptCommand(&is_actor_near_point_3d,pPlayerDed->m_dwGTAId,
						  pGame->m_vecCheckpointPos.x,
						  pGame->m_vecCheckpointPos.y,
						  pGame->m_vecCheckpointPos.z,
						  pGame->m_vecCheckpointExtent.x,
						  pGame->m_vecCheckpointExtent.y,
						  pGame->m_vecCheckpointExtent.z,
						  1);
		}
	}

	if(GetGameState() == GAMESTATE_CONNECTED)
	{
		// pool process
		if(m_pPlayerPool) m_pPlayerPool->Process();
		if (m_pObjectPool) m_pObjectPool->Process();
		if(m_pVehiclePool) m_pVehiclePool->Process();
		if(m_pPickupPool) m_pPickupPool->Process();

	}
	else
	{
		CPlayerPed *pPlayer = pGame->FindPlayerPed();
		CCamera *pCamera = pGame->GetCamera();

		// ?????? ??? ???????????

		if(pPlayer && pCamera)
		{
			if(pPlayer->IsInVehicle())
				pPlayer->RemoveFromVehicleAndPutAt(314.0f, 160.0f, 39.0f);
			else
				pPlayer->TeleportTo(314.0f, 160.0f, 39.0f);

			pCamera->SetPosition(-314.0f, 160.0f, 39.0f, 0.0f, 0.0f, 0.0f);
			pCamera->LookAtPoint(-310.0f, 157.0f, 39.0f, 2);
            CHUD::toggleAll(false);
		//	pGame->SetWorldWeather(m_byteWeather);
		//	pGame->DisplayWidgets(false);
		}
	}

	if(GetGameState() == GAMESTATE_WAIT_CONNECT && (GetTickCount() - m_dwLastConnectAttempt) > 3000)
	{
		CChatWindow::AddDebugMessageNonFormatted("{bbbbbb}Соединение к LIVE RUSSIA{ffffff}");

		m_pRakClient->Connect(m_szHostOrIp, m_iPort, 0, 0, 5);
		m_dwLastConnectAttempt = GetTickCount();
		SetGameState(GAMESTATE_CONNECTING);
	}

}

void CNetGame::UpdateNetwork()
{
	Packet* pkt;
	unsigned char packetIdentifier;

	while(pkt = m_pRakClient->Receive())
	{
		packetIdentifier = GetPacketID(pkt);
		switch(packetIdentifier)
		{
			case ID_AUTH_KEY:
				Log("Incoming packet: ID_AUTH_KEY");
				packetAuthKey(pkt);
				break;

			case ID_CONNECTION_ATTEMPT_FAILED:
				CChatWindow::AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::CONNECTION_ATTEMPT_FAILED));
				SetGameState(GAMESTATE_WAIT_CONNECT);
				break;

			case ID_NO_FREE_INCOMING_CONNECTIONS:
				CChatWindow::AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::FULL_SERVER));
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
				CChatWindow::AddDebugMessage("Failed to initialize encryption.");
				break;

			case ID_CONNECTION_BANNED:
				CChatWindow::AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::BANNED));
				break;

			case ID_INVALID_PASSWORD:
				CChatWindow::AddDebugMessage("Неверный пароль!");
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

			case ID_CUSTOM_RPC:
				Packet_CustomRPC(pkt);
				break;

		}

		m_pRakClient->DeallocatePacket(pkt);
	}
}

void CNetGame::Packet_TrailerSync(Packet* p)
{
	CRemotePlayer* pPlayer;
	RakNet::BitStream bsSpectatorSync((unsigned char*)p->data, p->length, false);

	if (GetGameState() != GAMESTATE_CONNECTED) return;

	BYTE bytePacketID = 0;
	BYTE bytePlayerID = 0;
	TRAILER_SYNC_DATA trSync;

	bsSpectatorSync.Read(bytePacketID);
	bsSpectatorSync.Read(bytePlayerID);
	bsSpectatorSync.Read((char*)& trSync, sizeof(TRAILER_SYNC_DATA));

	pPlayer = GetPlayerPool()->GetAt(bytePlayerID);

	if (pPlayer)
	{
		pPlayer->StoreTrailerFullSyncData(&trSync);
	}
}

void CNetGame::Packet_CustomRPC(Packet* p)
{
	
	RakNet::BitStream bs((unsigned char*)p->data, p->length, false);
	uint8_t packetID;
	uint32_t rpcID;
	bs.Read(packetID);
	bs.Read(rpcID);
	Log("Custom packet = %d", rpcID);
	//CChatWindow::AddDebugMessage("p %d rpc %d", packetID, rpcID);
	switch (rpcID)
	{
		case 555:{
			uint8_t gg;
			bool tt;
			bs.Read(gg);
			bs.Read(tt);

			Log("%d - %d", gg, tt);
            break;
		}
		case RPC_UPDATE_SATIETY: {
			packetUpdateSatiety(p);
			break;
		}
		case RPC_SHOW_ACTION_LABEL: {
			packetNotification(p);
			break;
		}

		case RPC_SET_MONEY: {
			uint32_t money;
			bs.Read(money);

			CHUD::iLocalMoney = money;

			CHUD::UpdateMoney();
			break;
		}

		case RPC_SHOW_SALARY:
		{
			packetSalary(p);
			break;
		}
		case RPC_SEND_BUFFER:
		{
			uint16_t len;
			bs.Read(len);
			char text[len+1];
			bs.Read(text, len);
			text[len] = '\0';

			g_pJavaWrapper->SendBuffer(text);
			break;
		}

		case RPC_OPEN_SETTINGS:
		{
			g_pJavaWrapper->ShowClientSettings();
			break;
		}
		case RPC_TOGGLE_HUD_ELEMENT:
		{
			uint32_t hud, toggle;
			bs.Read(hud);
			bs.Read(toggle);

			// CChatWindow::AddDebugMessage("hud %d toggle %d", hud, toggle);
			pGame->ToggleHUDElement(hud, toggle);
			//pGame->HandleChangedHUDStatus();
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
			cp1251_to_utf8(text, str, len);
			CHUD::showUpdateTargetNotify(value, (char *)text);

			break;
		}
		case RPC_SHOW_TD_BUS:
		{
			uint8_t toggle;
			uint32_t time;
			bs.Read(toggle);
			bs.Read(time);

			if (toggle == 1)
			{
				CHUD::showBusInfo((int)time);
			}
			else
			{
				CHUD::hideBusInfo();
			}

			break;
		}
//		case RPC_CHECK_CLIENT:
//		{
//			char recievKey[17];
//			uint16_t recievKey_len;
//			bs.Read(recievKey_len);
//			bs.Read(recievKey, recievKey_len);
//
//			recievKey[recievKey_len] = '\0';
//
//			char key_with_salt[recievKey_len+ strlen(AUTH_SALT)+1];
//			strcpy(key_with_salt, recievKey);
//			strcat(key_with_salt, AUTH_SALT);
//
//			SendCheckClientPacket(sha256(key_with_salt).c_str());
//			break;
//
//		}
		case RPC_CUSTOM_SET_LEVEL:
		{
			uint8_t current;
			uint8_t max;
			uint8_t level;
			bs.Read(current);
			bs.Read(max);
			bs.Read(level);

			CHUD::updateLevelInfo(level, current, max);
			break;
		}
		case RPC_TOGGLE_GPS_INFO:
		{
			uint8_t value;
			bs.Read(value);
			if (value == 1)
			{
				CHUD::toggleGps(true);
			}
			else if (value == 0)
			{
				CHUD::toggleGps(false);
			}
			break;
		}
		case RPC_TOGGLE_GREENZONE:
		{
			uint8_t value;
			bs.Read(value);

			if (value == 1)
			{
				CHUD::toggleGreenZone(true);
				m_GreenZoneState = true;
			}
			else if (value == 0)
			{
				CHUD::toggleGreenZone(false);
				m_GreenZoneState = false;
			}
			break;
		}
		case RPC_VIBRATE:
		{
			uint32_t value;
			bs.Read(value);

			g_pJavaWrapper->Vibrate(value);
			break;
		}
		case RPC_CUSTOM_HANDLING_DEFAULTS:
		{
			uint16_t vehId;
			bs.Read(vehId);

			if (GetVehiclePool())
			{
				CVehicle* pVeh = GetVehiclePool()->GetAt(vehId);
				if (pVeh)
				{
					pVeh->ResetVehicleHandling();
				}
			}

			break;
		}

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

		case RPC_STREAM_CREATE:
		{
			char str[255];
			uint8_t len;
			uint16_t id, vw, interior;
			CVector pos;
			float fDistance;
			bs.Read(id);
			bs.Read(pos.x);
			bs.Read(pos.y);
			bs.Read(pos.z);
			bs.Read(fDistance);
			bs.Read(vw);
			bs.Read(interior);
			bs.Read(len);
			bs.Read(&str[0], len);
			str[len] = '\0';
			//CChatWindow::AddDebugMessage("%d %f %f %f %f %d %d %d %s", id, pos.x, pos.y, pos.z, fDistance, vw, interior, len, str);
			GetStreamPool()->AddStream(id, &pos, vw, interior, fDistance, (const char*)&str[0]);
			break;
		}
		case RPC_STREAM_INDIVIDUAL:
		{
			char str[255];
			uint8_t len;
			uint8_t repeat;

			bs.Read(len);
			bs.Read(&str[0], len);
			str[len] = '\0';
			bs.Read(repeat);
			//CChatWindow::AddDebugMessage("%s", str);
			//CChatWindow::AddDebugMessage("Playing audiostream %s", str);
			if(repeat) {
				GetStreamPool()->PlayIndividualStream(&str[0], BASS_SAMPLE_LOOP);
			}
			else {
				GetStreamPool()->PlayIndividualStream(&str[0], BASS_STREAM_AUTOFREE);
			}
			break;
		}
		case RPC_STREAM_POS:
		{
			uint16_t id;
			CVector pos;
			bs.Read(id);
			bs.Read(pos.x);
			bs.Read(pos.y);
			bs.Read(pos.z);
			if (GetStreamPool()->GetStream(id))
			{
				GetStreamPool()->GetStream(id)->SetPosition(pos);
			}
			break;
		}
		case RPC_STREAM_DESTROY:
		{
			uint32_t id;
			bs.Read(id);
			//CChatWindow::AddDebugMessage("%d", id);
			m_pStreamPool->DeleteStreamByID(id);
			break;
		}
		case RPC_STREAM_VOLUME:
		{
			uint16_t id;
			float fVolume;
			bs.Read(id);
			bs.Read(fVolume);
			//CChatWindow::AddDebugMessage("%d %f", id, fVolume);
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
			bs.Read((char*)& link, len);
			AND_OpenLink(&link[0]);

			break;
		}
		case RPC_TIMEOUT_CHAT:
		{
//			uint32_t timeoutStart = 0;
//			uint32_t timeoutEnd = 0;
//			bs.Read(timeoutStart);
//			bs.Read(timeoutEnd);
//
//			if (pChatWindow)
//			{
//				CChatWindow::SetChatDissappearTimeout(timeoutStart, timeoutEnd);
//			}

			break;
		}
		case RPC_CUSTOM_SET_FUEL:
		{
			uint32_t fuel;
			uint32_t mileage;
			bs.Read(fuel);
			bs.Read(mileage);

            CSpeedometr::fFuel = (int) fuel;
            CSpeedometr::iMilliage = (int) mileage;
			break;
		}
	}

}
void CNetGame::ResetVehiclePool()
{
	Log("ResetVehiclePool");
	if(m_pVehiclePool)
		delete m_pVehiclePool;

	m_pVehiclePool = new CVehiclePool();
}

void CNetGame::ResetObjectPool()
{
	Log("ResetObjectPool");
	if(m_pObjectPool)
		delete m_pObjectPool;

	m_pObjectPool = new CObjectPool();
}

void CNetGame::ResetPickupPool()
{
	Log("ResetPickupPool");
	if(m_pPickupPool)
		delete m_pPickupPool;

	m_pPickupPool = new CPickupPool();
}

void CNetGame::ResetGangZonePool()
{
	Log("ResetGangZonePool");
	if(m_pGangZonePool)
		delete m_pGangZonePool;

	m_pGangZonePool = new CGangZonePool();
}

void CNetGame::ResetLabelPool()
{
	Log("ResetLabelPool");
	if(m_pLabelPool)
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

void CNetGame::ShutDownForGameRestart()
{
	for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		CRemotePlayer* pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer)
		{
			//pPlayer->SetTeam(NO_TEAM);
			//pPlayer->ResetAllSyncAttributes();
		}
	}

	CLocalPlayer *pLocalPlayer = m_pPlayerPool->GetLocalPlayer();
	if(pLocalPlayer)
	{
		pLocalPlayer->ResetAllSyncAttributes();
		pLocalPlayer->ToggleSpectating(false);
	}

	m_iGameState = GAMESTATE_RESTARTING;

	//m_pPlayerPool->DeactivateAll();
	m_pPlayerPool->Process();

	ResetVehiclePool();
	ResetObjectPool();
	ResetPickupPool();
	ResetGangZonePool();
	ResetLabelPool();
	ResetActorPool();
	g_pJavaWrapper->ClearScreen();

	m_bDisableEnterExits = false;
	m_fNameTagDrawDistance = 60.0f;
	m_byteWorldTime = 12;
	m_byteWorldMinute = 0;
	m_byteWeather = 1;
	m_bNameTagLOS = true;
	m_bUseCJWalk = false;
	m_fGravity = 0.008f;
	m_iDeathDropMoney = 0;

	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	if(pPlayerPed)
	{
	//	pGame->RemovePlayer(pPlayerPed);
		pPlayerPed->SetInterior(0, true);
//		//pPlayerPed->SetDead();
		pPlayerPed->SetArmour(0.0f);
	}

	CHUD::iLocalMoney = 0;
	m_bZoneNames = false;
	GameResetRadarColors();
	pGame->SetGravity(m_fGravity);
}
//
//void CNetGame::SendCheckClientPacket(const char password[])
//{
//	uint8_t packet = ID_CUSTOM_RPC;
//	uint8_t RPC = RPC_CHECK_CLIENT;
//	uint16_t bytePasswordLen = strlen(password);
//	RakNet::BitStream bsSend;
//	bsSend.Write(packet);
//	bsSend.Write(RPC);
//	bsSend.Write(bytePasswordLen);
//	bsSend.Write(password, bytePasswordLen);
//	GetRakClient()->Send(&bsSend, SYSTEM_PRIORITY, RELIABLE, 0);
//
//	//CChatWindow::AddDebugMessage("key: %s", password);
//}

void CNetGame::SendCustomPacket(uint8_t packet, uint8_t RPC, uint8_t Quantity)
{
	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(Quantity);
	GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}

void CNetGame::SendCustomPacketFuelData(uint8_t packet, uint8_t RPC, uint8_t fueltype, uint32_t fuel)
{
	RakNet::BitStream bsSend;
	bsSend.Write(packet);
	bsSend.Write(RPC);
	bsSend.Write(fueltype);
	bsSend.Write(fuel);
	GetRakClient()->Send(&bsSend, HIGH_PRIORITY, RELIABLE, 0);
}

void CNetGame::SendChatMessage(const char* szMsg)
{
	if (GetGameState() != GAMESTATE_CONNECTED) return;

	RakNet::BitStream bsSend;
	uint8_t byteTextLen = strlen(szMsg);

	bsSend.Write(byteTextLen);
	bsSend.Write(szMsg, byteTextLen);

	m_pRakClient->RPC(&RPC_Chat,&bsSend,HIGH_PRIORITY,RELIABLE,0,false, UNASSIGNED_NETWORK_ID, NULL);
}

void CNetGame::SendChatCommand(const char* szCommand)
{
	if (GetGameState() != GAMESTATE_CONNECTED) return;

	RakNet::BitStream bsParams;
	int iStrlen = strlen(szCommand);

	bsParams.Write(iStrlen);
	bsParams.Write(szCommand, iStrlen);
	m_pRakClient->RPC(&RPC_ServerCommand, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
}

void CNetGame::SetMapIcon(uint8_t byteIndex, float fX, float fY, float fZ, uint8_t byteIcon, int iColor, int style)
{
	if(byteIndex >= 100) return;
	if(m_dwMapIcons[byteIndex]) DisableMapIcon(byteIndex);

	m_dwMapIcons[byteIndex] = pGame->CreateRadarMarkerIcon(byteIcon, fX, fY, fZ, iColor, style);
}

void CNetGame::DisableMapIcon(uint8_t byteIndex)
{
	if(byteIndex >= 100) return;
	ScriptCommand(&disable_marker, m_dwMapIcons[byteIndex]);
	m_dwMapIcons[byteIndex] = 0;
}

void CNetGame::UpdatePlayerScoresAndPings()
{

	static uint32_t dwLastUpdateTick = 0;

	if ((GetTickCount() - dwLastUpdateTick) >= 3000) {
		dwLastUpdateTick = GetTickCount();
		RakNet::BitStream bsParams;
		m_pRakClient->RPC(&RPC_UpdateScoresPingsIPs, &bsParams, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, NULL);
	}
}

void gen_auth_key(char buf[260], char* auth_in);
void CNetGame::packetAuthKey(Packet* pkt)
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
	m_pRakClient->Send(&bsKey, SYSTEM_PRIORITY, RELIABLE_SEQUENCED, 0);

}

void CNetGame::Packet_DisconnectionNotification(Packet* pkt)
{
	CChatWindow::AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::SERVER_CLOSED_CONNECTION));
	m_pRakClient->Disconnect(2000);
	g_pJavaWrapper->ClearScreen();

	//pNetGame->ShutDownForGameRestart();
}

void CNetGame::Packet_ConnectionLost(Packet* pkt)
{
	if(m_pRakClient) m_pRakClient->Disconnect(0);

	CChatWindow::AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::CONNECTION_LOST));
	ShutDownForGameRestart();

	for(PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		CRemotePlayer *pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer) m_pPlayerPool->Delete(playerId, 0);
	}

	SetGameState(GAMESTATE_WAIT_CONNECT);
	
}
//#define SUM_MAS_ENCR	10
//int g_sumMas[SUM_MAS_ENCR] = { 290, 291, 417, 424, 477, 54+38+142+49, 51+91+91+84, 54+38+142+50, 54 + 38 + 142 + 51, 51 + 77 + 238 + 92 };

#include "..//CServerManager.h"

bool g_isValidSum(int a)
{
	for (int i = 0; i < MAX_SERVERS; i++)
	{
		if (g_sEncryptedAddresses[i].getSum() == a) return true;
	}
	return false;
}
#include <sstream>

void CNetGame::Packet_ConnectionSucceeded(Packet* pkt)
{
	CChatWindow::AddDebugMessageNonFormatted(CLocalisation::GetMessage(E_MSG::CONNECTED));
	SetGameState(GAMESTATE_AWAIT_JOIN);

	RakNet::BitStream bsSuccAuth((unsigned char *)pkt->data, pkt->length, false);
	PLAYERID MyPlayerID;
	unsigned int uiChallenge;

	uint16_t playerid;
	bsSuccAuth.IgnoreBits(8); // ID_CONNECTION_REQUEST_ACCEPTED
	bsSuccAuth.IgnoreBits(32); // binaryAddress
	bsSuccAuth.IgnoreBits(16); // port
	bsSuccAuth.Read(playerid);
	bsSuccAuth.Read(uiChallenge);
	char ip[0x7F];
	strncpy(ip, m_szHostOrIp, sizeof(ip));

	std::vector<std::string> strings;
	std::istringstream f((const char*)&ip[0]);
	std::string s;
	int sum = 0;
	while (getline(f, s, '.'))
	{
		sum += std::atoi(s.c_str());
	}

	m_pPlayerPool->SetLocalPlayerID(playerid);

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

	m_pRakClient->RPC(&RPC_ClientJoin, &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, NULL);
	Log("Packet_ConnectionSucceeded");

	// auth
	RakNet::BitStream bsSendTwo;
	bsSendTwo.Write((uint8_t) ID_CUSTOM_RPC);
	bsSendTwo.Write((uint8_t) PRC_AUTHORIZATION);

	pNetGame->GetRakClient()->Send(&bsSendTwo, HIGH_PRIORITY, RELIABLE_ORDERED, 0);
}
void CNetGame::Packet_PlayerSync(Packet* pkt)
{
	CRemotePlayer * pPlayer;
	RakNet::BitStream bsPlayerSync((unsigned char *)pkt->data, pkt->length, false);
	ONFOOT_SYNC_DATA ofSync;
	uint8_t bytePacketID=0;
	PLAYERID playerId;

	bool bHasLR,bHasUD;
	bool bHasVehicleSurfingInfo;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	memset(&ofSync, 0, sizeof(ONFOOT_SYNC_DATA));

	bsPlayerSync.Read(bytePacketID);
	bsPlayerSync.Read(playerId);

	// LEFT/RIGHT KEYS
	bsPlayerSync.Read(bHasLR);
	if(bHasLR) bsPlayerSync.Read(ofSync.lrAnalog);

	// UP/DOWN KEYS
	bsPlayerSync.Read(bHasUD);
	if(bHasUD) bsPlayerSync.Read(ofSync.udAnalog);

	// GENERAL KEYS
	bsPlayerSync.Read(ofSync.wKeys);

	// CVector POS
	bsPlayerSync.Read((char*)&ofSync.vecPos,sizeof(CVector));

	// QUATERNION
	float tw, tx, ty, tz;
	bsPlayerSync.ReadNormQuat(tw, tx, ty, tz);
	ofSync.quat.w = tw;
	ofSync.quat.x = tx;
	ofSync.quat.y = ty;
	ofSync.quat.z = tz;

	// HEALTH/ARMOUR (COMPRESSED INTO 1 BYTE)
	uint8_t byteHealthArmour;
	uint8_t byteArmTemp=0,byteHlTemp=0;

	bsPlayerSync.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if(byteArmTemp == 0xF) ofSync.byteArmour = 100;
	else if(byteArmTemp == 0) ofSync.byteArmour = 0;
	else ofSync.byteArmour = byteArmTemp * 7;

	if(byteHlTemp == 0xF) ofSync.byteHealth = 100;
	else if(byteHlTemp == 0) ofSync.byteHealth = 0;
	else ofSync.byteHealth = byteHlTemp * 7;

	// CURRENT WEAPON
    bsPlayerSync.Read(ofSync.byteCurrentWeapon);
    // SPECIAL ACTION
    bsPlayerSync.Read(ofSync.byteSpecialAction);

    // READ MOVESPEED VECTORS
    bsPlayerSync.ReadVector(tx, ty, tz);
    ofSync.vecMoveSpeed.x = tx;
    ofSync.vecMoveSpeed.y = ty;
    ofSync.vecMoveSpeed.z = tz;

    bsPlayerSync.Read(bHasVehicleSurfingInfo);
    if (bHasVehicleSurfingInfo) 
    {
        bsPlayerSync.Read(ofSync.wSurfInfo);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.x);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.y);
        bsPlayerSync.Read(ofSync.vecSurfOffsets.z);
    } 
    else
    	ofSync.wSurfInfo = INVALID_VEHICLE_ID;

	//uint8_t key = 0;

    if(m_pPlayerPool)
    {
    	pPlayer = m_pPlayerPool->GetAt(playerId);
    	if(pPlayer)
    		pPlayer->StoreOnFootFullSyncData(&ofSync, 0);
    }
}

void CNetGame::Packet_VehicleSync(Packet* pkt)
{
	CRemotePlayer *pPlayer;
	RakNet::BitStream bsSync((unsigned char *)pkt->data, pkt->length, false);
	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	uint8_t bytePacketID = 0;
	PLAYERID playerId;
	INCAR_SYNC_DATA icSync;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

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
	bsSync.Read((char*)&icSync.vecPos, sizeof(CVector));

	// speed
	bsSync.ReadVector(
		icSync.vecMoveSpeed.x,
		icSync.vecMoveSpeed.y,
		icSync.vecMoveSpeed.z);

	// vehicle health
	uint16_t wTempVehicleHealth;
	bsSync.Read(wTempVehicleHealth);
	icSync.fCarHealth = (float)wTempVehicleHealth;

	// health/armour
	uint8_t byteHealthArmour;
	uint8_t byteArmTemp=0, byteHlTemp=0;

	bsSync.Read(byteHealthArmour);
	byteArmTemp = (byteHealthArmour & 0x0F);
	byteHlTemp = (byteHealthArmour >> 4);

	if(byteArmTemp == 0xF) icSync.bytePlayerArmour = 100;
	else if(byteArmTemp == 0) icSync.bytePlayerArmour = 0;
	else icSync.bytePlayerArmour = byteArmTemp * 7;

	if(byteHlTemp == 0xF) icSync.bytePlayerHealth = 100;
	else if(byteHlTemp == 0) icSync.bytePlayerHealth = 0;
	else icSync.bytePlayerHealth = byteHlTemp * 7;

	// CURRENT WEAPON
	uint8_t byteTempWeapon;
	bsSync.Read(byteTempWeapon);
	icSync.byteCurrentWeapon ^= (byteTempWeapon ^ icSync.byteCurrentWeapon) & 0x3F;

	bool bCheck;

	// siren
	bsSync.Read(bCheck);
	if(bCheck) icSync.byteSirenOn = 1;
	// landinggear
	bsSync.Read(bCheck);
	if(bCheck) icSync.byteLandingGearState = 1;

	// train speed
	bsSync.Read(bCheck);
	if(bCheck) bsSync.Read(icSync.HydraThrustAngle);

	// triler id
	bsSync.Read(bCheck);
	if(bCheck) bsSync.Read(icSync.TrailerID);

	if(m_pPlayerPool)
	{
		pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer)
		{
			pPlayer->StoreInCarFullSyncData(&icSync, 0);
		}
	}
}

void CNetGame::Packet_PassengerSync(Packet* pkt)
{
	CRemotePlayer *pPlayer;
	uint8_t bytePacketID;
	PLAYERID playerId;
	PASSENGER_SYNC_DATA psSync;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	RakNet::BitStream bsPassengerSync((unsigned char *)pkt->data, pkt->length, false);
	bsPassengerSync.Read(bytePacketID);
	bsPassengerSync.Read(playerId);
	bsPassengerSync.Read((char*)&psSync, sizeof(PASSENGER_SYNC_DATA));

	if(m_pPlayerPool)
	{
		pPlayer = m_pPlayerPool->GetAt(playerId);
		if(pPlayer)
			pPlayer->StorePassengerFullSyncData(&psSync);
	}
}

void CNetGame::Packet_MarkersSync(Packet *pkt)
{
	CRemotePlayer *pPlayer;
	int			iNumberOfPlayers = 0;
	PLAYERID	playerId;
	short		sPos[3];
	bool		bIsPlayerActive;
	uint8_t 	unk0 = 0;

	if(GetGameState() != GAMESTATE_CONNECTED) return;

	RakNet::BitStream bsMarkersSync((unsigned char *)pkt->data, pkt->length, false);
	bsMarkersSync.Read(unk0);
	bsMarkersSync.Read(iNumberOfPlayers);

	if(iNumberOfPlayers)
	{
		for(int i=0; i<iNumberOfPlayers; i++)
		{
			bsMarkersSync.Read(playerId);
			bsMarkersSync.ReadCompressed(bIsPlayerActive);

			if(bIsPlayerActive)
			{
				bsMarkersSync.Read(sPos[0]);
				bsMarkersSync.Read(sPos[1]);
				bsMarkersSync.Read(sPos[2]);
			}

			if(playerId < MAX_PLAYERS && m_pPlayerPool->m_pPlayers[playerId])
			{
				pPlayer = m_pPlayerPool->GetAt(playerId);
				if(pPlayer)
				{
					if(bIsPlayerActive)
						pPlayer->ShowGlobalMarker(sPos[0], sPos[1], sPos[2]);
					else
						pPlayer->HideGlobalMarker();
				}
			}
		}
	}
}

void CNetGame::Packet_BulletSync(Packet* p)
{
	uint8_t bytePacketID;
	uint16_t PlayerID;
	BULLET_SYNC btSync;
	RakNet::BitStream bsBulletSync((unsigned char *)p->data, p->length, false);

	if (GetGameState() != GAMESTATE_CONNECTED)
		return;

	bsBulletSync.Read(bytePacketID);
	bsBulletSync.Read(PlayerID);
	bsBulletSync.Read((char *)&btSync, sizeof(BULLET_SYNC));

	CRemotePlayer *pRemotePlayer = m_pPlayerPool->GetAt(PlayerID);
	if (pRemotePlayer)
	{
		pRemotePlayer->StoreBulletSyncData(&btSync);
	}
}

void CNetGame::Packet_AimSync(Packet * p)
{
	CRemotePlayer * pPlayer;
	RakNet::BitStream bsAimSync((unsigned char*)p->data, p->length, false);
	AIM_SYNC_DATA aimSync;
	uint8_t bytePacketID = 0;
	uint16_t bytePlayerID = 0;

	if (GetGameState() != GAMESTATE_CONNECTED) return;

	bsAimSync.Read(bytePacketID);
	bsAimSync.Read(bytePlayerID);
	bsAimSync.Read((char*)&aimSync, sizeof(AIM_SYNC_DATA));

	pPlayer = GetPlayerPool()->GetAt(bytePlayerID);

	if (pPlayer)  {
		pPlayer->UpdateAimFromSyncData(&aimSync);
	}
}
