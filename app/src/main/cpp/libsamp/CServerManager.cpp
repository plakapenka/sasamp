#include "CServerManager.h"

#include <stdint.h>

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("45.136.205.21", 1, 20, 5535, false),	// основа
    CServerInstance::create("62.122.213.116", 1, 20, 7777, false)		// release
};

