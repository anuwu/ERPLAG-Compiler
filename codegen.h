#ifndef _Codegen
#define _Codegen

#include "ast.h"
#include "symbolTable.h"

int get_label() ;

int getStaticOffset (varST *vst, astNode *node, int size) ;
void staticArrBoundCheck (astNode *node, moduleST *lst, varST *vst, FILE *fp) ;
void dynamicArrBoundCheck (astNode *node, moduleST *lst, varST *vst, FILE *fp) ;

void printGetValueStaticArrayPrompt (tokenID baseType , int leftLim, int rightLim, FILE* fp) ;
void printGetValueDynArrayPrompt (varST *vst, FILE *fp) ;

void dynamicArrDX (varST *vst, FILE *fp) ;
void dynamicDeclare (moduleST *lst, varST *searchedVar, FILE *fp) ;

void IDGeneration (astNode *node, moduleST *lst, varST *vst, FILE* fp) ;
void printGeneration (astNode *node, moduleST *lst, varST *searchedVar, FILE *fp) ;

void getValueGeneration (moduleST *lst, varST *searchedVar, int rspDepth, FILE *fp) ;
void preamble (FILE *fp) ;


#endif