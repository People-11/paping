#include "standard.h"

#ifdef WIN32

#include <stdint.h>

#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
	#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
	#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

// Implementation of gettimeofday for Windows
// Note: struct timezone is forward declared in standard.h/gettimeofday.h
// We do not need its definition if we don't access it (we always pass NULL in paping).

extern "C"
{
	int gettimeofday(struct timeval *tv, struct timezone *tz)
	{
		FILETIME ft;
		uint64_t tmpres = 0;

		// Handle NULL tv
		if (NULL != tv)
		{
			GetSystemTimeAsFileTime(&ft);

			tmpres |= ft.dwHighDateTime;
			tmpres <<= 32;
			tmpres |= ft.dwLowDateTime;

			// converting file time to unix epoch
			tmpres -= DELTA_EPOCH_IN_MICROSECS;
			tmpres /= 10;  // convert into microseconds
			tv->tv_sec = (long)(tmpres / 1000000UL);
			tv->tv_usec = (long)(tmpres % 1000000UL);
		}

		// We intentionally ignore 'tz' here to avoid needing struct timezone definition.
		// Detailed timezone logic is not required for ping functionality.

		return 0;
	}
}

#endif
