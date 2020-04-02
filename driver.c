#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "parser.h"
#include "lexer.h"

#define SIZE 200
#define TNTLENGTH 40

#define isTerminal(x) x>=TK_EPS && x<=TK_RNUM
#define isNonTerminal(x) x>=program && x<=idL
#define endl printf("\n")
#define allRules rule


int lineBeforeAsterisk ;
char charBeforeAsterisk ;

extern char* tokenIDToString (tokenIDToString) ;
extern int printNextToken (twinBuffer* twinBuf);
extern int printNextTokenCOMMENT (twinBuffer *twinBuf);


void lexCapability (twinBuffer* twinBuf)
{
	while (printNextToken(twinBuf))
	{ ;
	}
}

void removeComments (twinBuffer *twinBuf)
{
	while (printNextTokenCOMMENT(twinBuf))
	{
		;
	}
}

twinBuffer *twinBuf ;

int main(int argc, char *argv[])
{
	if(argc<3){
		printf("Insufficient arguments\n");
		exit(0);
	}
	else if(argc > 3){
		printf("Too many arguments\n");
		exit(0);
	}


	int flag = 1 ;
	struct timeval t1, t2;
	double elapsedTime;
	
	while(flag)
	{

		printf("Enter option:\n\t0 - Exit (say thanks)\n\t1 - Print Comment-Free code\n\t2 - Print Token List\n\t3 - Run Parser (lexer included)\n\t4 - Total time taken\n");

		int ch; scanf("%d",&ch);
		switch(ch)
		{
			case 0:
				flag = 0 ;
				exit(1);
				break ;
			case 1:
				twinBuf = lexer_init (argv[1]) ;
				removeComments (twinBuf) ;
				break;
			
			case 2:
				twinBuf = lexer_init (argv[1]) ;
				lexCapability (twinBuf) ;
				break ;

			case 3 :
				parseTree(argv[1], argv[2], 1) ;
				break ;

			case 4 :	
                gettimeofday(&t1, NULL);
				// do something or call a function
				// ...
				// stop timer
				gettimeofday(&t2, NULL);
				// compute and print the elapsed time in millisec
				elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;
				elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;
				printf("Total time is %lf",elapsedTime);printf("ms.\n");
				break ;
		}
	}
	return 0;
}