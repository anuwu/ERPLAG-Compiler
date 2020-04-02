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

typedef struct _astNode 
{
	tokenID id ;
	token *tok ;

	datTag dtTag ;
	datType *dt ;

	struct _astNode *parent , *child , *next, *prev ;
} astNode ;

astNode* applyASTRule (treeNode *PTNode) ;
void inorderAST (astNode *node, int space) ;

#endif