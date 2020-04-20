/*
Group #52 -
Anwesh Bhattacharya (2016B5A70590P)
Deepak Chahar (2017A7PS0147P)
Rohan Kela (2016B1A70822P)
Komal Vasudeva (2017A7PS0103P)
*/


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

	if (argc == 4)
	{
		if (strcmp(argv[2], "-printAST") == 0)
			inorderAST (astRoot, 0) ;
		else
		{
			printf ("Incorrect command!\n") ;
			exit (0) ;
		}
	}


	baseST *baseTable ;
	if (argc == 3)
	{
		baseTable = fillSymbolTable (astRoot, atoi(argv[2])) ;
		if (argv[2][0] == '1')
			printBaseST (baseTable) ;
	}
	else
	{
		baseTable = fillSymbolTable (astRoot, atoi(argv[3])) ;
		if (argv[3][0] == '1')
			printBaseST (baseTable) ;
	}

	printf ("*********************************************************\n") ;

	if (baseTable->semanticError)
		printf ("Please check the above messages for semantic errors!\n") ;
	else
		printf ("Code is semantically clean!\n") ;

	return 0 ;
}