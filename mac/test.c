#include <stdio.h>

int main ()
{
	#ifdef __linux__
		printf ("Linux\n") ;
	#endif
	#ifdef __MACH__
		printf ("Mac\n") ;
	#endif

	return 0 ;
}
