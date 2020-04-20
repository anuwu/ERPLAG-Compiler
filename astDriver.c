#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ast.h"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Incorrect number of arguments\n") ;
		exit (0) ;
	}

	treeNode *root = parseTree (argv[1]) ;

	astNode *astRoot ;

	if (root->syntax_error)
		printf ("There are lexical/syntactical errors\n") ;
	else
	{
		int numberPTNode, numberASTNode, sizePT, sizeAST ;
		astRoot = applyASTRule (root) ;
		inorderAST (astRoot, 0) ;

		numberPTNode = getNumberPTNodes (root) ;
		sizePT = (int)sizeof(treeNode)*numberPTNode ;
		printf ("\nNo of parse tree nodes = %d and size = %d\n", numberPTNode, sizePT) ;

		numberASTNode = getNumberASTNodes (astRoot) ;
		sizeAST = (int)sizeof(astNode)*numberASTNode ;
		printf ("No of AST Nodes = %d and size = %d\n", numberASTNode , sizeAST) ;

		double compression = (sizePT - sizeAST)/(1.0*sizePT) * 100.0 ;
		printf ("Compresson percentage = %lf%%\n", compression) ;

		deletePT (root) ;
		deleteAST (astRoot) ;
	}
	

	return 0;
}