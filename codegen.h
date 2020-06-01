#ifndef _Codegen
#define _Codegen

#include "ast.h"
#include "symbolTable.h"

extern baseST *realBase ;
extern FILE *fpOut ;
extern int realPresent ;

#define codePrint(x, ...) fprintf (fpOut, x, ##__VA_ARGS__ ) 

typedef enum _textFlags
{
	boundERROR, declERROR, declNegERROR, argLimERROR, asgnLimERROR, divZeroERROR, printGetArrPrompt, getValuePrimitive, getArr, printInteger, printBoolean, printIntegerArr, printBooleanArr, boundCheck, dynamicDeclCheck, asgnArr
} textFlag ;

typedef enum _dataFlags
{
	boundPrint, declPrint, declNeg, arrArgMismatch, asgnArgMismatch, divZero, printFormatArray, printInt, printNewLine, printFormat, printTrue, printFalse, DATA_true, DATA_false, inputIntPrompt, inputBoolPrompt, inputIntArrPrompt, inputBoolArrPrompt, leftRange, rightRange, inputInt
} dataFlag ;

typedef enum _switchDeclareStatus
{
	NOT_SWITCH, SWITCH_INIT, SWITCH_GEN
} switchDeclareStatus ;

typedef enum _pushArrLim
{
	LEFT, RIGHT
} pushArrLim ;

typedef enum _labelType
{
	LABEL_SWITCH, LABEL_FOR, LABEL_WHILE, LABEL_DIV_ZERO, LABEL_OTHER
} labelType ;

// Helper functions
int get_label (labelType lt) ;
void codeComment (int tabCount, char *comment) ;
char* getOffsetStr (int offset) ;
int isIndexStatic (astNode *node) ;
void loadRegLeftLim (varST *searchedVar, char *reg) ;
void loadRegRightLim (varST *searchedVar, char *reg) ;

// Array handling
int getStaticOffset (varST *vst, astNode *node, int size) ;
void arrBoundCheck (astNode *node, moduleST *lst, varST *vst) ;
void dynamicDeclarationCheck (moduleST *lst, varST *searchedVar) ;
void dynamicDeclaration (moduleST *lst, varST *searchedVar, int declCount) ;

// Expression helper function
int isExprLeaf (tokenID id) ;
int isUnary (tokenID id) ;
int isAssignUnary (astNode *assignNode) ;
int isSingleRHS (astNode *node) ;
void dynAsgnArr (varST *lhs, varST *rhs) ;

// Expression
void exprAssign (astNode *node, moduleST *lst, int singleAssign) ;
void exprAssign (astNode *node, moduleST *lst, int singleAssign) ;
void exprLeaf (astNode *node, moduleST *lst, int singleAssign) ;

// I/O
void print (astNode *node, moduleST *lst) ;
void RSPAlign () ;
void RSPRestore () ;
void getValue (moduleST *lst, varST *searchedVar) ;

void preamble () ;
void errorTF (char *errStr) ;
void printArrStart () ;
void printArrEnd () ;
void postamble () ;
int isFlagSet (int flagInt, int id) ;

// Switch case
int getCaseCount (astNode *statementsNode) ;
void switchDeclareVars (astNode *statementNode) ;
int switchCaseLabels (astNode *node, moduleST *lst, int caseCount , int *caseLabels) ;

// Function call
void pushInputDynamicArr (varST *vst, varSTentry *varEntry, char *reg, pushArrLim flag) ;
void pushInput (astNode *inputEnd, varSTentry *varEntry, moduleST *lst) ;
void popOutput (astNode *inputEnd, moduleST *lst) ;

void freeDynamic (varSTentry *varEntry) ;
void retModule (moduleST *lst) ;

void moduleGeneration (astNode *node, moduleST *lst) ;
void codeGeneration(astNode *node) ;

void printCommentLineNASM () ;


#endif