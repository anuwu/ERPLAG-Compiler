#ifndef _Codegen
#define _Codegen

#include "ast.h"
#include "symbolTable.h"

extern baseST *realBase ;
extern FILE *fp ;

#define codePrint(x, ...) fprintf (fp, x, ##__VA_ARGS__ )

typedef enum _textFlags
{
	boundERROR, declERROR, declNegERROR, argLimERROR, printGetArrPrompt, getValuePrimitive, getArr, printInteger, printBoolean, printIntegerArr, printBooleanArr, boundCheck, dynamicDeclCheck
} textFlag ;

typedef enum _dataFlags
{
	boundPrint, declPrint, declNeg, printFormatArray, printInt, printNewLine, printFormat,
	printTrue, printFalse, DATA_true, DATA_false, inputIntPrompt, inputBoolPrompt, inputIntArrPrompt, inputBoolArrPrompt, leftRange, rightRange, inputInt, arrArgMismatch
} dataFlag ;

typedef enum _switchDeclareStatus
{
	NOT_SWITCH, SWITCH_INIT, SWITCH_GEN
} switchDeclareStatus ;

typedef enum _pushArrLim
{
	LEFT, RIGHT
} pushArrLim ;

int get_label() ;
void codeComment (int tabCount, char *comment) ;
char* getOffsetStr (int offset) ;
int isIndexStatic (astNode *node) ;
void loadRegLeftLim (varST *searchedVar, char *reg) ;
void loadRegRightLim (varST *searchedVar, char *reg) ;

int getStaticOffset (varST *vst, astNode *node, int size) ;
void boundCheckGeneration (astNode *node, moduleST *lst, varST *vst) ;

void dynamicDeclareCheck (moduleST *lst, varST *searchedVar) ;
void dynamicDeclareGeneration (moduleST *lst, varST *searchedVar, int declCount) ;

void assignGeneration (astNode *node, moduleST *lst) ;
void exprGeneration (astNode *node, moduleST *lst) ;
void exprLeafGeneration (astNode *node, moduleST *lst) ;
void printGeneration (astNode *node, moduleST *lst) ;

void getValueRSPAlign () ;
void getValueGeneration (moduleST *lst, varST *searchedVar) ;

void preamble () ;
void postamble () ;
int isFlagSet (int flagInt, int id) ;

int getCaseCount (astNode *statementsNode) ;
void switchDeclareVars (astNode *statementNode) ;
int switchCaseLabels (astNode *node, moduleST *lst, int caseCount , int *caseLabels) ;

void pushInputDynamicArr (varST *vst, varSTentry *varEntry, char *reg, pushArrLim flag) ;
void pushInputGeneration (astNode *inputEnd, varSTentry *varEntry, moduleST *lst) ;
void popOutputGeneration (astNode *inputEnd, moduleST *lst) ;

int moduleGeneration (astNode *node, moduleST *lst) ;
void codeGeneration(astNode *node) ;

void printCommentLineNASM () ;


#endif