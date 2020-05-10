#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "typeChecker.h"
#include "codegen.h"

FILE *fpOut ;

void checkCommand (int argc, char **argv, char **outputFile)
{
	if (argc == 1)
	{
		printf ("ERPLAG : No source file\n") ;
		exit (1) ;
	}

	if (argc != 2 && argc != 3)
	{
		printf ("ERPLAG : Incorrect number of arguments\n") ;
		exit (1) ;
	}

	int len = strlen (argv[1]) ;
	if (!(len > 4 && argv[1][len-1] == 'p' && argv[1][len-2] == 'r' && argv[1][len-3] == 'e' && argv[1][len-4] == '.'))
	{
		printf ("ERPLAG : Unsupported source format. Use file with .erp extension\n") ;
		exit (1) ;
	}

	if (argc == 2)
	{
		*outputFile = (char *) malloc (sizeof(char) * (len + 1)) ;
		strcpy (*outputFile, argv[1]) ;

		(*outputFile)[len - 1] = 'm' ;
		(*outputFile)[len - 2] = 's' ;
		(*outputFile)[len - 3] = 'a' ;
	}
	else if (argc == 3)
	{
		len = strlen (argv[2]) ;
		if (!(len > 4 && argv[2][len-1] == 'm' && argv[2][len-2] == 's' && argv[2][len-3] == 'a' && argv[2][len-4] == '.'))
		{
			printf ("ERPLAG : Unsupported output format. Use .asm extension\n") ;
			exit (1) ;
		}

		*outputFile = argv[2] ;
	}
}

int main(int argc, char **argv)
{
	FILE *fpIn ;
	char *outputFile ;
	checkCommand (argc, argv, &outputFile) ;
 
 	if (!(fpIn = fopen (argv[1], "r")))
 	{
 		printf ("ERPLAG : Source file %s does not exist\n", argv[1]) ;
 		exit (1) ;
 	}


	treeNode *root = parseTree (fpIn) ;
	if (root->syntax_error)
	{
		printf ("ERPLAG : Source file contains lexical/syntactical errors. Please check the above messages for details\n") ;
		exit (1) ;
	}	

	astNode *astRoot ;
	astRoot = applyASTRule (root) ;
	realBase = fillSymbolTable(astRoot,0);

	if (!realBase->semanticError)
	{
		int len ;
		if (!(fpOut = fopen (outputFile, "w")))
		{
			printf ("Error creating output file %s\n", outputFile) ;
			exit (1) ;
		}

		preamble () ;
		codeGeneration (astRoot) ;
		printCommentLineNASM () ;
		postamble () ;
	}
	else
		printf ("ERPLAG : Semantic errors found. Please check the above messages for details\n") ;

	printf ("Compilation successful\n") ;
	return 0;
}
