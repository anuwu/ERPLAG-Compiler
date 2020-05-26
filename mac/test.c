#include <stdio.h>

int main ()
{
	#ifdef __linux__
		printf ("Linux\n") ;
	#endif
	#ifdef __MACH__
		printf ("Mac\n") ;
	#else
		printf ("None\n") ;
	#endif

	return 0 ;
}
