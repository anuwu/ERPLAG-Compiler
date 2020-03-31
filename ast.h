#ifndef _AST
#define _AST

#include "lexerDef.h"
#include "lexer.h"
#include "parser.h"

/*

typedef enum _astTagsID
{
	ast_idList, ast_stmtList, ast_moduleInfo
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

*/

typedef struct _arrayTypeInfo
{
	tokenID type ;
	char *lex1 ;
	char *lex2 ;
} arrayTypeInfo ;

typedef enum _datTag
{
	PRIMITIVE, ARRAY
} datTag ;

typedef tokenID primitive ;

typedef union _datType
{
	primitive pType ;
	arrayTypeInfo *arrType ;
} datType ;

typedef struct _astNode 
{
	tokenID id ;
	token *tok ;

	datTag dtTag ;
	datType *dt ;

	struct _astNode *parent , *child , *next, *prev ;
} astNode ;


void astTest () ;
astNode* applyASTRule (treeNode *PTNode) ;
void inorderAST (astNode *node, int space) ;
void testroot (astNode *root) ;


#endif