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

extern int realPresent ;
extern baseST *realBase ;

// <var> checker
int validVarIndex (moduleST *baseModule, astNode *varIndexASTNode) ;
int validStaticArrStaticIndex (moduleST *baseModule, varST *arrVar, astNode *indASTNode) ;
tokenID validateVar (moduleST *baseModule , astNode *varASTNode, varST **searchedVar) ;
int isLeftLimStatic (varST *arrayVar) ;
int isRightLimStatic (varST *arrayVar) ;
int isVarStaticArr (varST *arrayVar) ;

// Overall function
void assignmentTypeCheck (moduleST *baseModule, astNode *assignopASTNode) ;

// expression checker
int isArithmeticType (tokenID id) ;
int isLogicalOp (tokenID id) ;
int isRelationalOp (tokenID id) ;
int isArithmeticType (tokenID id) ;
tokenID getExpressionType (moduleST *baseModule, astNode *exprNode) ;

#endif