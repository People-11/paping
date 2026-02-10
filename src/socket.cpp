#include "standard.h"

// using namespace std; // Removed iostream dependency

int socket_c::Resolve(pcc_t hostname, host_c &host)
{
	#ifdef WIN32	// Init Winsock in Windows
		WSADATA	wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return ERROR_SOCKET_GENERALFAILURE;
	#endif

	struct hostent* he;

	// Try to treat as IP first for optimization
	unsigned long ip = inet_addr(hostname);

	if (ip != INADDR_NONE)
	{
		host.IPAddress.s_addr = ip;
		host.Type = IPPROTO_TCP; // Default, will be updated later
		strncpy(host.Hostname, hostname, sizeof(host.Hostname));
	}
	else
	{
		// Not an IP, try name resolution
		if ((he = gethostbyname(hostname)) == NULL)
		{
			#ifdef WIN32
				WSACleanup();
			#endif
			return ERROR_SOCKET_CANNOTRESOLVE;
		}

		host.IPAddress = *((struct in_addr*)he->h_addr);
		
		// Use the official name (h_name) as the hostname (CNAME)
		// If h_name is empty (unlikely if success), fallback to input
		if (he->h_name && strlen(he->h_name) > 0)
			strncpy(host.Hostname, he->h_name, sizeof(host.Hostname) - 1);
		else
			strncpy(host.Hostname, hostname, sizeof(host.Hostname) - 1);
	}
	
	host.Hostname[sizeof(host.Hostname) - 1] = '\0';

	#ifdef WIN32	// Cleanup Winsock in Windows
		WSACleanup();
	#endif

	const char* resolvedIP = inet_ntoa(host.IPAddress);
	// Check if the input hostname is effectively the IP
	host.HostIsIP = (strcmp(hostname, resolvedIP) == 0);

	return SUCCESS;
}


int socket_c::SetPortAndType(int port, int type, host_c &host)
{
	host.Port	= port;
	host.Type	= type;

	return SUCCESS;
}


pcc_t socket_c::GetFriendlyTypeName(int type)
{
	switch (type)
	{
		case IPPROTO_TCP:
			return "TCP";
		case IPPROTO_UDP:
			return "UDP";
#ifdef WIN32
		case IPPROTO_ICMP:
			return "ICMP";
#endif
		default:
			return "UNKNOWN";
	}
}


int socket_c::GetSocketType(int type)
{
	switch (type)
	{
		case IPPROTO_UDP:
			return SOCK_DGRAM;
		default:
			return SOCK_STREAM;
	}
}


int socket_c::Connect(host_c host, int timeout, double &time)
{
	int		result	= 0;
	int		clientSocket;


	#ifdef WIN32	// Init Winsock in Windows
		WSADATA	wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) return ERROR_SOCKET_GENERALFAILURE;
	#endif

	clientSocket = socket(AF_INET, socket_c::GetSocketType(host.Type), host.Type);

	if (clientSocket == -1) return ERROR_SOCKET_GENERALFAILURE;

	// Enable SO_LINGER with 0 timeout to force RST on close
	// This avoids TIME_WAIT state, allowing immediate port reuse for high-frequency pings
	struct linger l;
	l.l_onoff = 1;
	l.l_linger = 0;
	setsockopt(clientSocket, SOL_SOCKET, SO_LINGER, (const char*)&l, sizeof(l));

	sockaddr_in	clientAddress;

	clientAddress.sin_addr		= host.IPAddress;
	clientAddress.sin_family	= AF_INET;
	clientAddress.sin_port		= htons(host.Port);

	timeval	tv;
	
	// No blocking for Windows/Linux
	#ifdef WIN32
		ULONG	mode = 1;
		ioctlsocket(clientSocket, FIONBIO, &mode);
	#else
		long arg = fcntl(clientSocket, F_GETFL, host.Type);
		arg = fcntl(clientSocket, F_SETFL, arg | O_NONBLOCK);
	#endif


	tv.tv_sec 	= 0;
	tv.tv_usec	= timeout * 1000;

	timer_c	timer;

	timer.Start();		

	connect(clientSocket, (sockaddr*)&clientAddress, sizeof(clientAddress));

	fd_set	read, write;

	FD_ZERO(&read);
	FD_ZERO(&write);

	FD_SET(clientSocket, &read);
	FD_SET(clientSocket, &write);
	
	result = select(clientSocket + 1, &read, &write, NULL, &tv);
	
	if (result != 1)
	{
		close(clientSocket);

		#ifdef WIN32	// Cleanup Winsock in Windows
			WSACleanup();
		#endif

		return ERROR_SOCKET_TIMEOUT;
	}

	time		= timer.Stop();

	if (!FD_ISSET(clientSocket, &read) && !FD_ISSET(clientSocket, &write))
	{
		close(clientSocket);

		#ifdef WIN32	// Cleanup Winsock in Windows
			WSACleanup();
		#endif

		return ERROR_SOCKET_TIMEOUT;
	}

	close(clientSocket);

	#ifdef WIN32	// Cleanup Winsock in Windows
		WSACleanup();
	#endif

	return SUCCESS;
}
