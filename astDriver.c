#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ast.h"

#define SIZE 200
#define TNTLENGTH 40

#define isTerminal(x) x>=TK_EPS && x<=TK_RNUM
#define isNonTerminal(x) x>=program && x<=idL
#define endl printf("\n")
#define allRules rule


int lineBeforeAsterisk ;
char charBeforeAsterisk ;

extern int printNextToken (twinBuffer* twinBuf);
extern int printNextTokenCOMMENT (twinBuffer *twinBuf);

twinBuffer *twinBuf ;

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		printf("Incorrect number of arguments\n") ;
		exit (0) ;
	}
	FILE* outfile ;
	outfile = fopen (argv[2] , "w") ;

	//astTest () ;
	treeNode *root = parseTree (argv[1]) ;
	inorderTraversal(root, outfile) ;
	fclose (outfile) ;

	printf ("ID of root = %d\n", root->tnt.nonTerm) ;
	applyASTRule (root) ;


	

	return 0;
}