#include "networkconnection.h"
#include "assert.h"
#include "unittests.h"
#include <stdint.h>
#include <enet\enet.h>

#ifdef _DEBUG
TESTABLE_DEFINE(NetworkConnectionIdentifier, {
	{
		NetworkConnectionIdentifier connection(10, 0);
		if (connection.GetUniqueIdentifier() != 10) {
			DbgError("Mismatch (uid) read %u but expected %u", connection.GetUniqueIdentifier(), 10);
			return false;
		}
		if (connection.GetBits() != 0) {
			DbgError("Mismatch, no bits should be set");
			return false;
		}
	}

	{
		NetworkConnectionIdentifier connection(10, 1);
		if (connection.GetUniqueIdentifier() != 10) {
			DbgError("Mismatch (uid) read %u but expected %u", connection.GetUniqueIdentifier(), 10);
			return false;
		}
		if (connection.GetBits() != 1) {
			DbgError("Mismatch, only one bit should be set (saw %u)", connection.GetBits());
			return false;
		}
	}
	return true;
});
#endif

NetworkConnection::~NetworkConnection()
{
	if (IsConnected())
	{
		DisconnectSocket();
	}
}
