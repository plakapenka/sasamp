#include "CServerManager.h"

#include <stdint.h>

const char *g_szServerNames[MAX_SERVERS] = {
	"LIVE RUSSIA | Moscow",
	"LIVE RUSSIA | Peterburg",
	"LIVE RUSSIA | Test Server"
};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("46.174.49.47", 1, 20, 7788, false),// москва
	CServerInstance::create("46.174.49.47", 1, 20, 7825, false), // спб
	CServerInstance::create("46.174.49.47", 1, 20, 7786, false) // тест
};
