#include "CServerManager.h"

#include <stdint.h>

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("45.136.205.21", 1, 20, 5535, false),	// основа
    CServerInstance::create("94.250.248.153", 1, 20, 7788, false)		// release
};

