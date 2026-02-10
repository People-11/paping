#include "icmp.h"
#include <iphlpapi.h>
#include <icmpapi.h>

#ifdef WIN32
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

int icmp_c::Ping(host_c host, int timeout, double &time)
{
#ifdef WIN32
	HANDLE hIcmpFile;
	unsigned long ipaddr = INADDR_NONE;
	DWORD dwRetVal = 0;
	char SendData[32] = "Data Buffer";
	LPVOID ReplyBuffer = NULL;
	DWORD ReplySize = 0;

	ipaddr = host.IPAddress.S_un.S_addr;

	hIcmpFile = IcmpCreateFile();
	if (hIcmpFile == INVALID_HANDLE_VALUE) {
		return ERROR_SOCKET_GENERALFAILURE;
	}

	ReplySize = sizeof(ICMP_ECHO_REPLY) + sizeof(SendData);
	ReplyBuffer = (VOID*)malloc(ReplySize);
	if (ReplyBuffer == NULL) {
		IcmpCloseHandle(hIcmpFile);
		return ERROR_POUTOFMEMORY;
	}

	timer_c timer;
	timer.Start();

	dwRetVal = IcmpSendEcho(hIcmpFile, ipaddr, SendData, sizeof(SendData),
		NULL, ReplyBuffer, ReplySize, timeout);

	time = timer.Stop();

	if (dwRetVal != 0) {
		PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
		// Use the round trip time from ICMP reply if available and non-zero, 
		// otherwise use our timer. ICMP RTT is in ms (integer).
		if (pEchoReply->Status == IP_SUCCESS) {
			// Prefer high-precision timer, but RTT is a good fallback
			// time = (double)pEchoReply->RoundTripTime; 
            // Stick to high-precision timer for better <1ms resolution if possible, 
            // but IcmpSendEcho is synchronous, so timer wraps the call.
		}
		else {
			free(ReplyBuffer);
			IcmpCloseHandle(hIcmpFile);
			return ERROR_SOCKET_TIMEOUT; // Or more specific error mapping
		}
	}
	else {
		// DWORD err = GetLastError();
		// Map errors if needed: IP_REQ_TIMED_OUT, etc.
		free(ReplyBuffer);
		IcmpCloseHandle(hIcmpFile);
		return ERROR_SOCKET_TIMEOUT;
	}

	free(ReplyBuffer);
	IcmpCloseHandle(hIcmpFile);
	return SUCCESS;

#else
	// Linux/Unix implementation would go here (requires raw sockets)
	return ERROR_SOCKET_GENERALFAILURE; 
#endif
}
