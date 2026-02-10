#ifndef ICMP_H
#define ICMP_H

#include "standard.h"

// Define IPPROTO_ICMP if not available (Standard value is 1)
#ifndef IPPROTO_ICMP
#define IPPROTO_ICMP 1
#endif

class icmp_c
{
public:
	static int Ping(host_c host, int timeout, double &time);
};

#endif
