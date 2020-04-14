#ifndef _typechecker
#define _typechecker

#include "lexerDef.h"
#include "ast.h"


struct _baseST ;
struct _moduleST ;
struct _varST ;
typedef struct _baseST baseST ;
typedef struct _moduleST moduleST ;
typedef struct _varST varST ;

// <var> checker
int validVarIndex (baseST *realBase, moduleST *baseModule, astNode *varIndexASTNode) ;
int validStaticArrStaticIndex (baseST *realBase, moduleST *baseModule, varST *arrVar, astNode *indASTNode) ;
tokenID validateVar (baseST *realBase , moduleST *baseModule , astNode *varASTNode, varST **searchedVar) ;
int isVarStaticArr (varST *arrayVar) ;

void expressionTypeCheck (baseST *realBase, moduleST *baseModule, astNode *assignopASTNode) ;

#endif