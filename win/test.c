#include <stdio.h>
#include <stdlib.h>

int main ()
{
	int a, *b ;
	a = 4 ;

	scanf ("%d", &a) ;
	printf ("Output : %d\n", a) ;

	b = (int *) malloc (32) ;

	return 0 ;
}