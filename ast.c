#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ast.h"
#include "parser.h"

#define SPANOUT 10

char* tokenIDToString (tokenID id)
{
	switch (id)
	{
		case TK_REAL :
			return "TK_REAL" ;
		case TK_INTEGER :
			return "TK_INTEGER" ;
		case TK_BOOLEAN :
			return "TK_BOOLEAN" ;
		case TK_ID :
			return "TK_ID" ;
		case TK_NUM :
			return "TK_NUM" ;
		case TK_RNUM :
			return "TK_RNUM" ;
		case TK_OF :
			return "TK_OF" ;
		case TK_ARRAY :
			return "TK_ARRAY" ;
		case TK_START :
			return "TK_START" ;
		case TK_END :
			return "TK_END" ;
		case TK_DECLARE :
			return "TK_DECLARE" ;
		case TK_MODULE :
			return "TK_MODULE" ;
		case TK_DRIVER :
			return "TK_DRIVER" ;
		case TK_PROGRAM :
			return "TK_PROGRAM" ;
		case TK_GET_VALUE :
			return "TK_GET_VALUE" ;
		case TK_PRINT :
			return "TK_PRINT" ;
		case TK_USE :
			return "TK_USE" ;
		case TK_WITH :
			return "TK_WITH" ;
		case TK_PARAMETERS :
			return "TK_PARAMETERS" ;
		case TK_TRUE :
			return "TK_TRUE" ;
		case TK_FALSE :
			return "TK_FALSE" ;
		case TK_TAKES :
			return "TK_TAKES" ;
		case TK_INPUT :
			return "TK_INPUT" ;
		case TK_RETURNS :
			return "TK_RETURNS" ;
		case TK_AND :
			return "TK_AND" ;
		case TK_OR :
			return "TK_OR" ;
		case TK_FOR :
			return "TK_FOR" ;
		case TK_IN :
			return "TK_IN" ;
		case TK_SWITCH:
			return "TK_SWITCH" ;
		case TK_CASE :
			return "TK_CASE" ;
		case TK_BREAK :
			return "TK_BREAK" ;
		case TK_DEFAULT :
			return "TK_DEFAULT" ;
		case TK_WHILE :
			return "TK_WHILE" ;
		case TK_PLUS :
			return "TK_PLUS" ;
		case TK_MINUS :
			return "TK_MINUS" ;
		case TK_MUL :
			return "TK_MUL" ;
		case TK_DIV :
			return "TK_DIV" ;
		case TK_LT :
			return "TK_LT" ;
		case TK_LE :
			return "TK_LE" ;
		case TK_GE :
			return "TK_GE" ;
		case TK_GT :
			return "TK_GT" ;
		case TK_EQ :
			return "TK_EQ" ;
		case TK_NE :
			return "TK_NE" ;
		case TK_DEF :
			return "TK_DEF" ;
		case TK_ENDDEF :
			return "TK_ENDDEF" ;
		case TK_DRIVERDEF :
			return "TK_DRIVERDEF" ;
		case TK_DRIVERENDDEF :
			return "TK_DRIVERENDDEF" ;
		case TK_COLON :
			return "TK_COLON" ;
		case TK_RANGEOP :
			return "TK_RANGEOP" ;
		case TK_SEMICOL :
			return "TK_SEMICOL" ;
		case TK_COMMA :
			return "TK_COMMA" ;
		case TK_ASSIGNOP :
			return "TK_ASSIGNOP" ;
		case TK_SQBO:
			return "TK_SQBO" ;
		case TK_SQBC :
			return "TK_SQBC" ;
		case TK_BO :
			return "TK_BO" ;
		case TK_BC :
			return "TK_BC" ;


		/************************************************************************
		*************************************************************************/


		case program :
			return "program" ;
		case moduleDeclarations :
			return "moduleDeclarations" ;
		case otherModules :
			return "otherModules" ;
		case moduleDeclaration :
			return "moduleDeclaration" ;
		case IPL :
			return "IPL" ;
		case input_plist :
			return "input_plist" ;
		case ret :
			return "ret" ;
		case moduleDef :
			return "moduleDef" ;
		case module :
			return "module" ;
		case driverModule :
			return "driverModule" ;
		case output_plist :
			return "output_plist" ;
		case OPL :
			return "OPL" ;
		case statements :
			return "statements" ;
		case statement :
			return "statement" ;
		case ioStmt :
			return "ioStmt" ;
		case print_var :
			return "print_var" ;
		case simpleStmt :
			return "simpleStmt" ;
		case whichStmt :
			return "whichStmt" ;
		case assignmentStmt :
			return "assignmentStmt" ;
		case lvalueIDStmt :
			return "lvalueIDStmt" ;
		case lvalueARRStmt :
			return "lvalueARRStmt" ;
		case moduleReuseStmt :
			return "moduleReuseStmt" ;
		case optional :
			return "optional" ;
		case declareStmt :
			return "declareStmt" ;
		case iterativeStmt :
			return "iterativeStmt" ;
		case conditionalStmt :
			return "conditionalStmt" ;
		case caseStmt :
			return "caseStmt" ;
		case default_new :
			return "default_new"  ;
		case expression_new :
			return "expression_new" ;
		case U :
			return "U" ;
		case expression :
			return "expression" ;
		case bT :
			return "bT" ;
		case boolTerm :
			return "boolTerm" ;
		case aE :
			return "aE" ;
		case arithmeticExpr :
			return "arithmeticExpr" ;
		case aT :
			return "aT" ;
		case term :
			return "term" ;
		case aF :
			return "aF" ;
		case factor :
			return "factor" ;
		case factor_new :
			return "factor_new" ;
		case logicalOp :
			return "logicalOp" ;
		case relationalOp :
			return "relationalOp" ;
		case pmop :
			return "pmop" ;
		case mdop :
			return "mdop" ;
		case dataType :
			return "dataType" ;
		case type :
			return "type" ;
		case var :
			return "var" ;
		case whichID :
			return "whichId" ;
		case index_new :
			return "index_new" ;
		case value :
			return "value" ;
		case range :
			return "range" ;
		case range_new :
			return "range_new" ;
		case idList :
			return "idList" ;
		case idL :
			return "idL" ;
 	}
}

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

astNode* createASTNode (treeNode *PTNode)
{
	astNode *node = (astNode *) malloc (sizeof(astNode)) ;
 
	node->PTTag = PTNode->tag ;
	node->tnt = PTNode->tnt ;

	// if (tag == NON_TERMINAL)
	// 	node->tag = NON_TERMINAL ;
	// else if (tag == TERMINAL)
	// 	node->tag = TERMINAL ;
 
	node->tnt = tnt ;
	node->child = NULL ;
	node->next = NULL ;
	node->parent = NULL ;


	node->gcode = -5 ; // default value ( should be replaced )
	node->syn = NULL ;
	node->inh = NULL ;
}

astNode* applyASTRule (treeNode *PTNode)
{
	if (PTNode == NULL)
		return ;

	
	/*
	treeNode *childSibling ;
	applyASTRule (PTNode->child) ;

	if (PTNode->child != NULL)
	{
		childSibling = PTNode->child->next ;
		while (childSibling != NULL)
		{
			applyASTRule (childSibling) ;
			childSibling = childSibling->next ;
		}
	}

	if (PTNode->tag == TERMINAL)
		printf ("%s\n", PTNodeToString(PTNode)) ;
	else if (PTNode->tag == NON_TERMINAL)
		printf ("%s\n", PTNodeToString(PTNode)) ;
	*/

	

	astNode* children[SPANOUT] ;
	for (int i = 0 ; i < SPANOUT ; i++)
		astNode[i] = NULL ;
	treeNode *leftChild , *sibling ;

	switch (PTNode->gcode)
	{
		case 0 :
			leftChild = PTNode->child ;
			children[0] = applyASTRule (leftChild) ;	//moduleDeclarations

			sibling = leftChild->next ;
			children[1] = applyASTRule (sibling) ;		//otherModules

			sibling = sibling->next ;
			children[2] = applyASTRule (sibling) ;		//driverModule

			sibling = sibling->next ;
			children[3] = applyASTRule (sibling) ; 		//otherModules
			break ;

		case 2 :

			break ;

	}
}

void astTest ()
{
	printf ("This is astTest\n") ;
}

