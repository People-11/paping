#include "standard.h"


using namespace std;

#ifdef _WIN32
	#define	PRINT_COLOR_BLUE	FOREGROUND_BLUE		|	FOREGROUND_GREEN		|	FOREGROUND_INTENSITY
	#define	PRINT_COLOR_GREEN	FOREGROUND_GREEN	|	FOREGROUND_INTENSITY
	#define	PRINT_COLOR_RED		FOREGROUND_RED		|	FOREGROUND_INTENSITY
	#define	PRINT_COLOR_YELLOW	FOREGROUND_GREEN	|	FOREGROUND_RED			|	FOREGROUND_INTENSITY
#else
	#define	PRINT_COLOR_BLUE	34
	#define	PRINT_COLOR_GREEN	32
	#define	PRINT_COLOR_RED		31
	#define	PRINT_COLOR_YELLOW	33
#endif


struct slice_s
{
	int	Begin;
	int	End;
};


class print_c
{
	public:
		static	int		initialColors_;
		static	bool	UseColor;

		static	void	Initialize();
		static	void	Lock();
		static	void	Unlock();

		static	void	FormattedPrint(int color, pcc_t data);
		static	void	Write(pcc_t data); // Direct write optimization
		static void SetColor(int color);
		static void ResetColor();
};
