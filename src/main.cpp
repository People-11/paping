#include "standard.h"

// using namespace std; // Removed iostream dependency


void printError(int error);
int printConnectInfo(host_c host);
int printSuccessfulConnection(host_c host, double time);
int printFailedConnection(int failtype);
int printStats();

void signalHandler(int id);


bool	useColor	= false;
int		exitCode	= 0;
stats_c	stats;
volatile bool g_running = true;



int main(int argc, pc_t argv[])
{
	host_c		host;
	int			result;
	arguments_c	arguments;


	stats.Attempts	= 0;
	stats.Connects	= 0;
	stats.Failures	= 0;
	stats.Minimum	= 0.0;
	stats.Maximum	= 0.0;
	stats.Total		= 0.0;

#ifdef WIN32
	timeBeginPeriod(1);
	print_c::Initialize();
	// Enable standard Ctrl+C handling to trigger SIGINT
	SetConsoleCtrlHandler(NULL, FALSE);
#endif

	arguments_c::PrintBanner();

	result = arguments_c::Process(argc, argv, arguments);

	if (result != SUCCESS)
	{
		arguments_c::PrintUsage();
		return ERROR_INVALIDARGUMENTS;
	}

	useColor = arguments.UseColor;

	result = socket_c::Resolve(arguments.Destination, host);

	if (result == SUCCESS)
	{
		bool setPort = true;
#ifdef WIN32
		// If ICMP is used, set type to ICMP
		if (arguments.UseICMP)
		{
			host.Type = IPPROTO_ICMP;
			host.Port = 0;
			setPort = false;
		}
#endif
		if (setPort)
		{
			socket_c::SetPortAndType(arguments.Port, arguments.Type, host);
		}

		if (result != SUCCESS)
		{
			printError(result);
			return result;
		}

		result = printConnectInfo(host);

		if (result != SUCCESS)
		{
			printError(result);
			return result;
		}
	}
	else
	{
		printError(result);
		return result;
	}

	signal(SIGINT, &signalHandler);


	unsigned int	i		= 0;

	double			time	= 0.0;

	while ((arguments.Continous || i < (unsigned int)arguments.Count) && g_running)
	{
#ifdef WIN32
		if (host.Type == IPPROTO_ICMP)
		{
			result = icmp_c::Ping(host, arguments.Timeout, time);
		}
		else
#endif
		{
			result = socket_c::Connect(host, arguments.Timeout, time);
		}

		stats.Attempts++;

		if (result == SUCCESS)
		{
			stats.Connects++;
			stats.Total += time;
			stats.UpdateMaxMin(time);

			printSuccessfulConnection(host, time);
		}
		else
		{
			// For ICMP, exitCode might depend on error type, but keep consistent for failure
			exitCode = 1;
			stats.Failures++;

			printFailedConnection(result);
		}

		// Interval logic: arguments.Interval is in milliseconds (int)
		// We need to wait: Interval - time (ms)
		double waitTimeMs = (double)arguments.Interval - time;

		if (waitTimeMs > 0)
		{
			#ifdef WIN32
				Sleep((DWORD)ceil(waitTimeMs));
			#else
				usleep((useconds_t)(waitTimeMs * 1000));
			#endif
		}

		i++;
	}


	printStats();	

#ifdef WIN32
	timeEndPeriod(1);
#endif

	return exitCode;
}


void signalHandler(int id)
{
	switch (id)
	{
		case SIGINT:
			g_running = false;
			return;
	}
}


void printError(int error)
{
	print_c::Lock();
	if (useColor)
		print_c::FormattedPrint(PRINT_COLOR_RED, i18n_c::GetString(error));
	else
		print_c::FormattedPrint(NULL, i18n_c::GetString(error));

	putchar('\n');
	print_c::Unlock();
}


int printConnectInfo(host_c host)
{
	print_c::Lock();
	host.PrintConnectInfo(useColor);
	print_c::Write("\n");
	print_c::Unlock();
	return SUCCESS;
}


int printSuccessfulConnection(host_c host, double time)
{
	print_c::Lock();
	host.PrintSuccessfulConnection(time, useColor);
	print_c::Write("\n");
	print_c::Unlock();
	return SUCCESS;
}


int printStats()
{
	char	str[1024];

	stats.GetStatisticsString(str);

	if (useColor)
		print_c::FormattedPrint(PRINT_COLOR_BLUE, str);
	else
		print_c::FormattedPrint(NULL, str);

	putchar('\n');

	return SUCCESS;
}


int printFailedConnection(int error)
{
	print_c::Lock();
	if (useColor)
		print_c::FormattedPrint(PRINT_COLOR_RED, i18n_c::GetString(error));
	else
		print_c::FormattedPrint(NULL, i18n_c::GetString(error));

	putchar('\n');
	print_c::Unlock();

	return SUCCESS;
}
