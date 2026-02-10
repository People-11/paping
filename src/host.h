#include "standard.h"

class host_c
{
	public:
		char	Hostname[256];
		in_addr	IPAddress;
		bool	HostIsIP;
		int		Port;
		int		Type;
		
		pcc_t	IPAddressString();
		// Optimized direct print methods
		void	PrintConnectInfo(bool useColor);
		void	PrintSuccessfulConnection(double time, bool useColor);

};
