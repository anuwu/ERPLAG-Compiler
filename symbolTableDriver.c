#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ast.h"
#include "symbolTable.h"
#include "error.h"


int main(int argc, char *argv[])
{
	if (!(argc == 3 || argc == 4))
	{
		printf("Incorrect number of arguments\n") ;
		exit (0) ;
	}

	FILE *fpIn ;
	if (!(fpIn = fopen (argv[1], "r")))
 	{
 		errFatal () ;
 		printf ("Source file %s does not exist\n", argv[1]) ;
 		exit (1) ;
 	}

	treeNode *root = parse (fpIn) ;

	if (root->syntax_error)
	{
		printf ("Source code contains syntax errors. Please correct them\n") ;
		exit (0) ;
	}


	astNode *astRoot = applyASTRule (root) ; 
	deletePT (root) ;

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