#include "standard.h"


// using namespace std; // Removed iostream dependency


pcc_t host_c::IPAddressString()
{
	return host_c::GetIPAddressAsString(this->IPAddress);
}


int	host_c::GetSuccessfulConnectionString(pcc_t str, double time)
{
	int		length	= 0;
	pcc_t	format	= i18n_c::GetString(STRING_CONNECT_SUCCESS);
	
	if (this->Type == IPPROTO_ICMP)
	{
		// Hack to reuse string but ignore port part if we wanted, but the string has 4 placeholders.
		// We should probably add a new string resource or just format it manually here for simplicity/minimal changes.
		// The original string is: "Connected to $%s$: time=$%.2fms$ protocol=$%s$ port=$%d$"
		// We can just pass 0 as port, but user wants clean output. 
		// Let's manually format for ICMP to avoid changing i18n.h/cpp too much or misusing the string.
		
		const char* icmpFormat = "Connected to %s: time=%.2fms protocol=%s";
		// Note: The original uses custom print format with $ colors. 
		// "Connected to $%s$: time=$%.2fms$ protocol=$%s$"
		
		const char* icmpFormatColored = "Connected to $%s$: time=$%.2fms$ protocol=$%s$";

		length = snprintf(NULL, 0, icmpFormatColored, this->IPAddressString(), time, socket_c::GetFriendlyTypeName(this->Type));
		if (str != NULL) sprintf((pc_t)str, icmpFormatColored, this->IPAddressString(), time, socket_c::GetFriendlyTypeName(this->Type));
	}
	else
	{
		length = snprintf(NULL, 0, format, this->IPAddressString(), time, socket_c::GetFriendlyTypeName(this->Type), this->Port);
		if (str != NULL) sprintf((pc_t)str, format, this->IPAddressString(), time, socket_c::GetFriendlyTypeName(this->Type), this->Port);
	}
	
	return length;
}


int host_c::GetConnectInfoString(pcc_t str)
{
	int		length	= 0;
	pcc_t	format	= NULL;
	
	// Manual format for ICMP to avoid port
#ifdef WIN32
	if (this->Type == IPPROTO_ICMP)
	{
		if (this->HostIsIP)
		{
			// "Connecting to $%s$ on $%s %d$:" -> "Connecting to $%s$ on $%s$:"
			const char* fmt = "Connecting to $%s$ on $%s$:";
			length = snprintf(NULL, 0, fmt, this->Hostname, socket_c::GetFriendlyTypeName(this->Type));
			if (str != NULL) sprintf((pc_t)str, fmt, this->Hostname, socket_c::GetFriendlyTypeName(this->Type));
		}
		else
		{
			// "Connecting to $%s$ [$%s$] on $%s %d$:" -> "Connecting to $%s$ [$%s$] on $%s$:"
			const char* fmt = "Connecting to $%s$ [$%s$] on $%s$:";
			length = snprintf(NULL, 0, fmt, this->Hostname, this->IPAddressString(), socket_c::GetFriendlyTypeName(this->Type));
			if (str != NULL) sprintf((pc_t)str, fmt, this->Hostname, this->IPAddressString(), socket_c::GetFriendlyTypeName(this->Type));
		}
		return length;
	}
#endif

	if (this->HostIsIP)
	{
		format = i18n_c::GetString(STRING_CONNECT_INFO_IP);

		length = snprintf(NULL, 0, format, this->Hostname, socket_c::GetFriendlyTypeName(this->Type), this->Port);
		if (str != NULL) sprintf((pc_t)str, format, this->Hostname, socket_c::GetFriendlyTypeName(this->Type), this->Port);
	}
	else
	{
		format = i18n_c::GetString(STRING_CONNECT_INFO_FULL);
		
		length = snprintf(NULL, 0, format, this->Hostname, this->IPAddressString(), socket_c::GetFriendlyTypeName(this->Type), this->Port);
		if (str != NULL) sprintf((pc_t)str, format, this->Hostname, this->IPAddressString(), socket_c::GetFriendlyTypeName(this->Type), this->Port);
	}
	
	
	return length;
}


pcc_t host_c::GetIPAddressAsString(in_addr ipAddress)
{
	return inet_ntoa(ipAddress);
}
