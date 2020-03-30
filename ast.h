#ifndef _AST
#define _AST

#include "lexerDef.h"
#include "lexer.h"
#include "parser.h"

typedef struct _astnode {
	tokenID tkID ;
	nodeTag tag ;
	TNT tnt ;

	struct _astnode *parent , *child , *next ;
} astNode ;

astNode* newASTnonLeaf (tokenID tkID , astNode * child) ;
astNode* newASTLeaf (tokenID tkID , token tkn) ;
astNode* generateASTtree (treeNode* root) ;

void astTest () ;


#endif