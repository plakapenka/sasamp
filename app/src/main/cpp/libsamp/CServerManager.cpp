#include "CServerManager.h"

#include <stdint.h>

const char *g_szServerNames[MAX_SERVERS] = {
	"LIVE RUSSIA | Moscow",
	"LIVE RUSSIA | Peterburg",
	//"LIVE RUSSIA | Novosibirsk",
//	"LIVE RUSSIA | Samara",
	"LIVE RUSSIA | Sochi",
	"LIVE RUSSIA | Test Server"
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("46.174.49.47", 1, 20, 7788, false),// москва
	CServerInstance::create("46.174.49.47", 1, 20, 7793, false), // спб
	//CServerInstance::create("46.174.49.47", 1, 20, 7825, false), //  новосиб
	//CServerInstance::create("46.174.49.47", 1, 20, 7788, false),
	CServerInstance::create("46.174.49.47", 1, 20, 7862, false), // сочи
	CServerInstance::create("46.174.49.47", 1, 20, 7786, false) // тест
};
