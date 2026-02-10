#include "standard.h"


// using namespace std; // Removed iostream dependency


pcc_t host_c::IPAddressString()
{
	return inet_ntoa(this->IPAddress);
}


void host_c::PrintSuccessfulConnection(double time, bool useColor)
{
	char buffer[64];
	
	if (this->Type == IPPROTO_ICMP)
	{
		print_c::Write("Connected to ");
		if (useColor) print_c::SetColor(PRINT_COLOR_GREEN);
		print_c::Write(this->IPAddressString());
		if (useColor) print_c::ResetColor();
		
		print_c::Write(": time=");
		if (useColor) print_c::SetColor(PRINT_COLOR_GREEN);
		snprintf(buffer, sizeof(buffer), "%.2fms", time);
		print_c::Write(buffer);
		if (useColor) print_c::ResetColor();
		
		print_c::Write(" protocol=");
		if (useColor) print_c::SetColor(PRINT_COLOR_GREEN);
		print_c::Write(socket_c::GetFriendlyTypeName(this->Type));
		if (useColor) print_c::ResetColor();
	}
	else
	{
		print_c::Write("Connected to ");
		if (useColor) print_c::SetColor(PRINT_COLOR_GREEN);
		print_c::Write(this->IPAddressString());
		if (useColor) print_c::ResetColor();
		
		print_c::Write(": time=");
		if (useColor) print_c::SetColor(PRINT_COLOR_GREEN);
		snprintf(buffer, sizeof(buffer), "%.2fms", time);
		print_c::Write(buffer);
		if (useColor) print_c::ResetColor();
		
		print_c::Write(" protocol=");
		if (useColor) print_c::SetColor(PRINT_COLOR_GREEN);
		print_c::Write(socket_c::GetFriendlyTypeName(this->Type));
		if (useColor) print_c::ResetColor();
		
		print_c::Write(" port=");
		if (useColor) print_c::SetColor(PRINT_COLOR_GREEN);
		snprintf(buffer, sizeof(buffer), "%d", this->Port);
		print_c::Write(buffer);
		if (useColor) print_c::ResetColor();
	}
}

void host_c::PrintConnectInfo(bool useColor)
{
	char buffer[16];

#ifdef WIN32
	if (this->Type == IPPROTO_ICMP)
	{
		print_c::Write("Connecting to ");
		if (useColor) print_c::SetColor(PRINT_COLOR_YELLOW);
		print_c::Write(this->Hostname);
		if (useColor) print_c::ResetColor();
		
		if (!this->HostIsIP)
		{
			print_c::Write(" [");
			if (useColor) print_c::SetColor(PRINT_COLOR_YELLOW);
			print_c::Write(this->IPAddressString());
			if (useColor) print_c::ResetColor();
			print_c::Write("]");
		}

		print_c::Write(" on ");
		if (useColor) print_c::SetColor(PRINT_COLOR_YELLOW);
		print_c::Write(socket_c::GetFriendlyTypeName(this->Type));
		if (useColor) print_c::ResetColor();
		print_c::Write(":");
		return;
	}
#endif

	print_c::Write("Connecting to ");
	if (useColor) print_c::SetColor(PRINT_COLOR_YELLOW);
	print_c::Write(this->Hostname);
	if (useColor) print_c::ResetColor();

	if (!this->HostIsIP)
	{
		print_c::Write(" [");
		if (useColor) print_c::SetColor(PRINT_COLOR_YELLOW);
		print_c::Write(this->IPAddressString());
		if (useColor) print_c::ResetColor();
		print_c::Write("]");
	}

	print_c::Write(" on ");
	if (useColor) print_c::SetColor(PRINT_COLOR_YELLOW);
	print_c::Write(socket_c::GetFriendlyTypeName(this->Type));
	if (useColor) print_c::ResetColor();
	
	print_c::Write(" ");
	if (useColor) print_c::SetColor(PRINT_COLOR_YELLOW);
	snprintf(buffer, sizeof(buffer), "%d", this->Port);
	print_c::Write(buffer);
	if (useColor) print_c::ResetColor();
	print_c::Write(":");
}


