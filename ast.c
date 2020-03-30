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

 	if (PTNode->tag == NON_TERMINAL)
 	{
 		//printf ("\tcreateASTNODE : PTNode->tnt.nonTerm = %d\n", PTNode->tnt.nonTerm) ;
 		node->id = PTNode->tnt.nonTerm ;
 		//printf ("\tcreateASTNODE : node->id = %d\n", node->id) ;
 		node->tok = NULL ;
 	}
 	else
 	{
 		node->id = PTNode->tnt.term->id ;
 		node->tok = PTNode->tnt.term ;
 	}

 	node->parent = NULL ;
 	node->child = NULL ;
 	node->next = NULL ;
 	node->prev = NULL ;

 	PTNode->syn = node ;

 	return node ;
}

void inorderAST (astNode *node, int space)
{	
	if (node == NULL)
		return ;

	astNode *sib = node->child ;

	if (sib != NULL)
	{
		while (sib->next != NULL)
		{
			//inorderAST (sib, space + 1) ;
			sib = sib->next ;
		}

		
		while (sib != NULL)
		{
			inorderAST (sib, space+1) ;
			sib = sib->prev ;
		}
	}

	for (int i = 0 ; i < 2*space ; i++)
		printf (" ") ;
	printf ("|(%s,%s)\n", tokenIDToString(node->id), (node->parent == NULL)?"NULL":tokenIDToString(node->parent->id)) ;
}

/*
void pushASTChild (astNode *node, astNode *sib)
{
	sib->parent = node ;

	if (node->child == NULL)
		node->child = sib ;
	else
	{
		node = node->child ;
		while (node->next != NULL)
			node = node->next ;

		node->next = sib ;
	}
}
*/

void connectChildren (astNode *parent, astNode **siblings, int num)
{
	for (int i = 0 ; i < num - 1; i++)
	{
		siblings[i]->next = siblings[i+1] ;
		siblings[i]->parent = parent ;
	}

	siblings[num-1]->parent = parent ;
	for (int i = num-1 ; i > 0 ; i--)
		siblings[i]->prev = siblings[i-1] ;
}

void testroot (astNode *root)
{
	astNode *mds , *stmt ;
	mds = root->child ;
	printf ("%s\n", tokenIDToString(root->id)) ;

	while (mds != NULL)
	{
		printf ("\t%s, parent = %s\n" , tokenIDToString(mds->id), tokenIDToString((mds->parent)->id)) ;
		if (mds->child == NULL)
			printf ("\t\tIts child is null\n") ;
		if (mds->id == driverModule)
		{
			stmt = mds->child ;
			printf ("\t\t%s, parent = %s\n", tokenIDToString(stmt->id) , tokenIDToString(stmt->parent->id)) ;
			if (stmt->child == NULL)
				printf ("\t\t\t Its child is null\n") ;
		}
		mds = mds->next ;
	}
}

astNode* applyASTRule (treeNode *PTNode)
{
	if (PTNode == NULL)
		return NULL ;
	
	astNode *node = NULL , *children[SPANOUT] ;
	treeNode *leftChild , *sibling ;

	//printf ("applyASTRule : Before any switch %d and %s\n", PTNode->gcode, tokenIDToString(PTNode->tnt.nonTerm)) ;
	//printf ("%d\n", PTNode->gcode) ;
	switch (PTNode->gcode)
	{
		case 0 :								// <program> --> <moduleDeclarations> <otherModules> <driverModule> <otherModules>
			node = createASTNode (PTNode) ;
			//node->id = PTNode->tnt.nonTerm ;

			// <moduleDeclarations>
			leftChild = PTNode->child ;		
			children[0] = createASTNode (leftChild) ; 
			applyASTRule (leftChild) ;	// 1, 2
			children[0]->child = leftChild->syn ;

			// <otherModules>
			sibling = leftChild->next ;
			children[1] = createASTNode (sibling) ; 
			applyASTRule (sibling) ;	// 5
			children[1]->child = sibling->syn ;

			// <driverModule>
			sibling = sibling->next ;		
			children[2] = createASTNode (sibling) ; 
			applyASTRule (sibling) ;	// 6
			children[2]->child = sibling->syn ;
			sibling->syn->parent = children[2] ;

			// <otherModules>
			sibling = sibling->next ;		
			children[3] = createASTNode (sibling) ; 
			applyASTRule (sibling) ;	// 5
			children[3]->child = sibling->syn ;

			// Connect the children at the program level.
			node->child = children[0] ;
			connectChildren (node, children, 4) ;

			//printf ("End of root %d\n", node->id) ;

			break ;

		case 1 :								// <moduleDeclarations> --> <moduleDeclaration> <moduleDeclarations>
			leftChild = PTNode->child ;
			sibling = leftChild->next ;

			applyASTRule (leftChild) ;		// 3
			sibling->inh = leftChild->syn ;
			if (PTNode->inh != NULL)
			{
				PTNode->inh->next = sibling->inh ;
				sibling->inh->prev = PTNode->inh ;
			}

			applyASTRule (sibling) ;		// Recurse on 1 until case 2
			PTNode->syn = sibling->syn ;
			break ;


		case 2 :								// <moduleDeclarations> --> EPS
			PTNode->syn = PTNode->inh ;
			if (PTNode->syn != NULL)
			{
				while (PTNode->syn->prev != NULL)
					PTNode->syn = PTNode->syn->prev ;
			}
			break ;

		case 3 :								// <moduleDeclaration> ---> DECLARE MODULE ID SEMICOL
			leftChild = PTNode->child ;
			sibling = leftChild->next->next ;

			PTNode->syn = createASTNode (sibling) ;
			break ;

		case 5 :								// <otherModules> --> EPS
			PTNode->syn = PTNode->inh ;
			break ;

		case 6 :								// <driverModule> --> DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>
			leftChild = PTNode->child ;
			sibling = leftChild->next ;

			while (sibling->next != NULL)
				sibling = sibling->next ;

			// <moduleDef>
			applyASTRule (sibling) ;	// 8
			PTNode->syn = sibling->syn ;

			break ;

		case 8 :								// <moduleDef> --> START <statements> END
			leftChild = PTNode->child ;
			sibling = leftChild->next ;

			// <statements>
			PTNode->syn = createASTNode (sibling) ;
			applyASTRule (sibling) ;	// 18
			PTNode->syn->child = sibling->syn ;

			break ;

		case 18 :								// <statements> --> EPS
			PTNode->syn = PTNode->inh ;
			break ;

	}

	return node ;
}

void astTest ()
{
	printf ("This is astTest\n") ;
}

