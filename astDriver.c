#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ast.h"
#include "error.h"

int main(int argc, char *argv[])
{
	if (argc != 2)
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

	astNode *astRoot ;
	astRoot = applyASTRule (root) ;
	deletePT (root) ;

	inorderAST (astRoot, 0) ;

	return 0;
}