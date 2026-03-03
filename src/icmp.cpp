#include "icmp.h"
#include <iphlpapi.h>
#include <icmpapi.h>

#ifdef WIN32
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

extern volatile bool g_running;

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

	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEvent == NULL) {
		free(ReplyBuffer);
		IcmpCloseHandle(hIcmpFile);
		return ERROR_SOCKET_GENERALFAILURE;
	}

	timer_c timer;
	timer.Start();

	IcmpSendEcho2(hIcmpFile, hEvent, NULL, NULL, ipaddr, SendData, sizeof(SendData),
		NULL, ReplyBuffer, ReplySize, timeout);

	const int sliceMs = 100;
	int remainMs = timeout + sliceMs; // give a little extra for reply processing
	dwRetVal = 0;
	while (remainMs > 0 && g_running)
	{
		int waitMs = (remainMs < sliceMs) ? remainMs : sliceMs;
		DWORD waitResult = WaitForSingleObject(hEvent, (DWORD)waitMs);
		if (waitResult == WAIT_OBJECT_0)
		{
			dwRetVal = IcmpParseReplies(ReplyBuffer, ReplySize);
			break;
		}
		remainMs -= waitMs;
	}

	time = timer.Stop();

	CloseHandle(hEvent);

	if (dwRetVal != 0) {
		PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)ReplyBuffer;
		if (pEchoReply->Status != IP_SUCCESS) {
			free(ReplyBuffer);
			IcmpCloseHandle(hIcmpFile);
			return ERROR_SOCKET_TIMEOUT;
		}
	}
	else {
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

