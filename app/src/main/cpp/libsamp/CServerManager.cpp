#include "CServerManager.h"

#include <stdint.h>

const CServerInstance::CServerInstanceEncrypted g_sEncryptedAddresses[MAX_SERVERS] = {
	CServerInstance::create("149.56.195.234", 1, 20, 7777, false),	// основа
    CServerInstance::create("149.56.195.234", 1, 20, 7777, false),		// release
	CServerInstance::create("149.56.195.234", 1, 20, 7777, false)
};

