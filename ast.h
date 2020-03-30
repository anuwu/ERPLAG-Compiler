#ifndef _AST
#define _AST

#include "lexerDef.h"
#include "lexer.h"
#include "parser.h"

typedef enum _astTagsID
{
	ast_idList, ast_stmtList
} astTagsID ;

typedef enum _stmtTypeID
{
	ast_get_value, ast_print, ast_assignment, ast_moduleReuse, ast_declareStmt, ast_switch, ast_for, ast_while
} stmtTypeID ;

typedef struct _idListNode
{
	char *idName ;
	struct _idListNode *next ;
} idListNode ;

typedef struct _stmtListNode
{
	int lineno ;
	stmtTypeID stmtType ;
	struct _stmtListNode *next ;
} stmtListNode ;

typedef struct _moduleInfoNode
{
	char *moduleName ;
	idListNode *inputArgList , *retList ;
	struct _moduleInfoNode *next ;
} moduleInfoNode ;

typedef union _astDataUnion 		// Smash together everything that the AST can contain
{
	idListNode* idListHead ;
	stmtListNode* stmtListHead ; 
	moduleInfoNode* moduleInfoHead ;
} astDataUnion ;

typedef struct _astNode {
	nodeTag PTTag ;		// Whether terminal or non-terminal
	TNT tnt ;			// Union of terminal or non-terminal

	astTagsID astTag ;		// Describes one of the many things a node in the AST can contain
	astDataUnion data ;	// Synthesized and inherited attributes

	struct _astnode *parent , *child , *next ;
} astNode ;

astNode* newASTnonLeaf (tokenID tkID , astNode * child) ;
astNode* newASTLeaf (tokenID tkID , token tkn) ;
astNode* generateASTtree (treeNode* root) ;

void astTest () ;
astNode* applyASTRule (treeNode *PTNode) ;


#endif