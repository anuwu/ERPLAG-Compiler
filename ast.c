#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"

/*
typedef struct _astnode {
	tokenID tkID ;
	TNT tag ;

	struct _astnode *parent , *child , *next ;
} astNode ;
*/

/*
astNode* newASTnonLeaf ( tokenID tkID , astNode * child ) {
	astNode * tmp = ( astNode * ) malloc ( sizeof ( astNode )) ;
	tmp->tag = NON_TERMINAL ;
	tmp->tkID = tkID ;

	tmp->child = child ;
	tmp->parent = tmp->next = NULL ;

	return tmp ;
}

astNode* newASTLeaf ( tokenID tkID , token tkn ) {
	astNode * tmp = (astNode * ) malloc ( sizeof ( astNode )) ;
	tmp->tkID = tkID ;
	tmp->lexeme = tkn.lexeme ;

	tmp->parent = tmp->child = tmp->next = NULL ;

	return tmp ;
}
*/

void astTest ()
{
	printf ("This is astTest\n") ;
}

