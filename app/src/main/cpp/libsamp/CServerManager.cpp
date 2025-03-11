#include "CServerManager.h"

#include <stdint.h>

const char* g_szServerNames[MAX_SERVERS] = {
	"Brilliant RP | Phantom",
	"Brilliant RP | Cullinan"

};

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("149.56.195.234", 1, 16, 7777, false), // 1
	CServerInstance::create("149.56.195.234", 1, 16, 7777, false) // 2
};