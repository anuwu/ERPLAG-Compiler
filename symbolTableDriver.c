#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ast.h"
#include "symbolTable.h"


int main(int argc, char *argv[])
{
	if (!(argc == 3 || argc == 4))
	{
		printf("Incorrect number of arguments\n") ;
		exit (0) ;
	}

	treeNode *root = parseTree (argv[1]) ;
	astNode *astRoot = applyASTRule (root) ; 

	if (argc == 4 && (strcmp(argv[2], "printAST") == 0))
		inorderAST (astRoot, 0) ;
	else
	{
		printf ("Incorrect command!\n") ;
		exit (0) ;
	}

	printf ("*********************************************************\n") ;

	baseST *baseTable ;
	if (argc == 3)
		baseTable = fillSymbolTable (astRoot, atoi(argv[2])) ;
	else
		baseTable = fillSymbolTable (astRoot, atoi(argv[3])) ;

	printBaseST ( baseTable ) ;

	return 0 ;
}