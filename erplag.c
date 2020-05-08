#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "typeChecker.h"
#include "codegen.h"

extern baseST *realBase ;

int main(int argc, char *argv[])
{
	if (argc != 2 && argc != 3)
	{
		printf ("\t\tIncorrect number of arguments\n") ;
		exit (0) ;
	}

	treeNode *root = parseTree (argv[1]) ;

	if (root->syntax_error)
	{
		printf ("The source file contains lexical/syntactical errors. Please correct them\n") ;
		exit (0) ;
	}

	astNode *astRoot ;
	astRoot = applyASTRule (root) ;
	realBase = fillSymbolTable(astRoot,0);

	if (argc == 3)
	{
		if (strcmp(argv[2], "-printAST") == 0)
			inorderAST (astRoot, 0) ;
		else
		{
			printf ("Incorrect command!\n") ;
			exit (0) ;
		}
	}

	if (!realBase->semanticError)
	{
		FILE *fp = fopen ("code.asm", "w") ;

		preamble (fp) ;
		codeGeneration (astRoot, fp) ;
		printCommentLineNASM (fp) ;
		postamble (fp) ;
	}
	else
		printf ("Semantic errors found. Please check the above messages\n") ;
	return 0;
}