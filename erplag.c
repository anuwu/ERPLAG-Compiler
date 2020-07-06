#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "typeChecker.h"
#include "codegen.h"
#include "error.h"

FILE *fpOut ;

void checkCommand (int argc, char **argv, char **outputFile)
{
	if (argc == 1)
	{
		errFatal () ;
		printf ("No source file\n") ;
		exit (1) ;
	}

	if (argc != 2 && argc != 3)
	{
		errFatal () ;
		printf ("Incorrect number of arguments\n") ;
		exit (1) ;
	}

	int len = strlen (argv[1]) ;
	if (!(len > 4 && argv[1][len-1] == 'p' && argv[1][len-2] == 'r' && argv[1][len-3] == 'e' && argv[1][len-4] == '.'))
	{
		errFatal () ;
		#if defined __linux__ || defined __MACH__
			printf ("Unsupported source format. Use file with " ANSI_BOLD ".erp" ANSI_RESET " extension\n") ;
		#endif
		#ifdef _WIN64
			printf ("Unsupported source format. Use file with .erp extension\n") ;
		#endif
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
			errFatal () ;
			#if defined __linux__ || defined __MACH__
				printf ("Unsupported output format. Use " ANSI_BOLD ".asm" ANSI_RESET " extension\n") ;
			#endif
			#ifdef _WIN64
				printf ("Unsupported output format. Use .asm extension\n") ;
			#endif
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
 		errFatal () ;
		#if defined __linux__ || defined __MACH__
 			printf ("Source file " ANSI_BOLD "%s" ANSI_RESET " does not exist\n", argv[1]) ;
		#endif
		#ifdef _WIN64
			printf ("Source file %s does not exist\n", argv[1]) ;
		#endif
 		exit (1) ;
 	}


	treeNode *root = parse (fpIn) ;
	if (root->syntax_error)
		exit (1) ;

	astNode *astRoot ;
	astRoot = applyASTRule (root) ;
	deletePT (root) ;
	
	realBase = fillSymbolTable (astRoot,0);

	if (!realBase->semanticError)
	{
		if (realPresent)
		{
			errCodegen () ;
			#if defined __linux__ || defined __MACH__
				printf ("Variables or static values of type " ANSI_BOLD ANSI_GREEN "real" ANSI_RESET " found in source file, which is not handled by the present version of the compiler.\n") ;
			#endif
			#ifdef _WIN64
				printf ("Variables or static values of type \"real\" found in source file, which is not handled by the present version of the compiler.\n") ;
			#endif
			exit (1) ;
		}

		int len ;
		if (!(fpOut = fopen (outputFile, "w")))
		{
			errFatal () ;
			#if defined __linux__ || defined __MACH__
				printf ("Error creating asm file " ANSI_BOLD "%s" ANSI_RESET "\n", outputFile) ;
			#endif
			#ifdef _WIN64
				printf ("Error creating asm file %s\n", outputFile) ;
			#endif
			exit (1) ;
		}

		preamble () ;
		codeGeneration (astRoot) ;
		postamble () ;
	}
	else
		exit (1) ;

	return 0;
}
