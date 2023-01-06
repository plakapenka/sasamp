#include "CServerManager.h"

#include <stdint.h>

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("83.136.232.57", 1, 20, 7777, false),	// основа
    CServerInstance::create("46.174.49.47", 1, 20, 7788, false)		// release
};
