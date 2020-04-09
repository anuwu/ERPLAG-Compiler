#ifndef _AST
#define _AST

#include "lexerDef.h"
#include "lexer.h"
#include "parser.h"

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


/*
typedef struct _token
{
	tokenID id ;
	char *lexeme ;
	int lineNumber ;
} token ;
*/
typedef struct _astNode 
{
	tokenID id ;
	token *tok ;

	/*
	if id == NT
		tok = NULL
	else id == T
		tok holds tokn info
			id, lexeme, linenumber
	
	datTag random
	datType NULL
	*/

	datTag dtTag ;
	datType *dt ;

	struct _astNode *parent , *child , *next, *prev ;
} astNode ;

astNode* applyASTRule (treeNode *PTNode) ;
void inorderAST (astNode *node, int space) ;
void preorderAST (astNode *node, int space) ;
#endif