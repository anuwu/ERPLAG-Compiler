#ifndef _SYMBOLTable
#define _SYMBOLTable

#include "lexerDef.h"
#include "ast.h"

#define VAR_BIN_COUNT 13
#define MODULE_BIN_COUNT 13
#define SCOPE_BIN_COUNT 13
#define IO_BIN_COUNT 1


enum _stType {
	DRIVER_ST , MODULE_ST , FOR_ST, WHILE_ST , SWITCH_ST,  MODULE_REDEC_ST
} ;

// structures
struct _baseST ;
struct _moduleST ;

/*
struct _arrayInST {
	char *startingPos ;
	char *endingPos ;
	tokenID dataType ;
} ;
*/

enum _variableType 
{
	VAR_INPUT ,
	VAR_OUTPUT ,
	VAR_LOCAL ,
	VAR_LOOP ,
	VAR_MODULE 
} ;

struct _varST {
	char * lexeme ; // variable name

	tokenID datatype ; // TK_INTEGER, TK_REAL, TK_BOOL, TK_ARRAY
	arrayTypeInfo *arrayIndices ; // if TK_ARRAY 

	int offset ;
	void *scope ;
	enum _variableType varType ;
} ;


struct _moduleST {
	char * lexeme ; // iterations must be given a unique name also
	enum _stType tableType ;
	struct _varSTentry *localVars[VAR_BIN_COUNT] ;
	struct _varSTentry *inputVars[IO_BIN_COUNT] ;
	struct _varSTentry *outputVars[IO_BIN_COUNT] ;
	struct _moduleSTentry *scopeVars[SCOPE_BIN_COUNT] ;
	void * parent ; // DRIVER_ST, MODULE_ST -> baseST , ITER_ST -> moduleST 

	int currOffset ;
} ;



struct _moduleSTentry {
	struct _moduleST * thisModuleST ;
	struct _moduleSTentry * next ;
} ;

struct _varSTentry {
	struct _varST * thisVarST ;
	struct _varSTentry * next ;
} ;

struct _baseST {
	struct _varSTentry * vars[VAR_BIN_COUNT] ;
	struct _moduleSTentry * modules[MODULE_BIN_COUNT] ;
	struct _moduleST * driverST ;

	int semanticError ;
} ;


typedef enum _stType stType ;
typedef enum _variableType variableType ;
typedef struct _baseST baseST ;
typedef struct _moduleST moduleST ;
//typedef struct _arrayInST arrayInST ;
typedef struct _varST varST ;
typedef struct _varSTentry varSTentry ;
typedef struct _moduleSTentry moduleSTentry ;



//general functions
int hashFunction ( char* lexeme , int size ) ;
char * generateString () ;


// function for base symbol table
baseST * createBaseST () ;
moduleST * createModuleST ( baseST * parent , char * lexeme, int currOffset) ;
moduleST * createDriverST ( baseST * parent ) ;
moduleST * createScopeST ( moduleST * parent , stType scopeType ) ;
varST * createVarST ( astNode * thisASTnode, void *scope, variableType varType ) ;

// Insertion
void insertModuleSTInbaseST ( baseST * base , moduleST * thisModule) ;
void insertVarSTInbaseST ( baseST * base , varST * thisVarST ) ;
void insertDriverSTInbaseST ( baseST * base , moduleST * thisDriverModule ) ;
void insertScopeST ( moduleST* parent , moduleST * thisScopeST ) ;
void insertLocalVarST ( moduleST* thisModule , varST* thisVarST ) ;
void insertInputVarST ( moduleST* thisModule , varST* thisVarST ) ;
void insertOutputVarST ( moduleST* thisModule , varST* thisVarST ) ;

//search
varST * searchVarInbaseST ( baseST * base ,char * lexeme ) ;
moduleST * searchModuleInbaseST ( baseST * base, char * lexeme ) ;
moduleST * searchDriverInbaseST ( baseST * base ) ;
varST * searchlocalVarInCurrentModule ( moduleST * thisModule , char * lexeme ) ;
varST * searchInputVarInCurrentModule ( moduleST * thisModule , char * lexeme ) ;
varST * searchOutputVarInCurrentModule ( moduleST * thisModule , char * lexeme ) ;
varST * searchVarInCurrentModule ( moduleST * thisModule , char * lexeme ) ;
varST * searchVar (baseST* realBase, moduleST * thisModule , char * lexeme ) ;

// Printing
void printBaseST ( baseST * base ) ;
void printModuleST ( moduleST * thisModuleST, int printParam) ;


// ST population
baseST * fillSymbolTable (astNode * thisASTNode, int depthSTPrint ) ;
void fillModuleST ( baseST* realBase ,moduleST* baseModule , astNode * moduleASTNode, int depthSTPrint ) ;

// Function call checker
int isValidCall ( baseST * base ,moduleST* thisModule , astNode * funcIDNode , int haveReturns) ;
varST * checkIP (baseST *realBase, moduleST * thisModule ,moduleST * targetModule , astNode * inputNode ) ;
varST * checkOP (baseST *realBase, moduleST * thisModule ,moduleST * targetModule , astNode * inputNode ) ;

// Helper
char* varTypeToString (variableType varType) ;
int getSize(baseST * realBase, varST * thisVar) ;
char *getParentModuleName (baseST* realBase, moduleST *scope) ;

#endif