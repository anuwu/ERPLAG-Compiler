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

twinBuffer *twinBuf ;

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Incorrect number of arguments\n") ;
		exit (0) ;
	}

	treeNode *root = parse (argv[1]) ;

	if (root->syntax_error)
	{
		printf ("Source code contains syntax errors. Please correct them\n") ;
		exit (0) ;
	}

	astNode *astRoot ;
	astRoot = applyASTRule (root) ;
	inorderAST (astRoot, 0) ;

	return 0;
}