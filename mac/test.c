#include <stdio.h>

int main ()
{
	#ifdef __linux__
		printf ("Linux\n") ;
	#else
	#ifdef __MACH__
		printf ("Mac\n")
	#else
		printf ("None\n") ;
	#endif
	#endif

	return 0 ;
}