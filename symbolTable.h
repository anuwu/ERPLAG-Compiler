#ifndef _SYMBOLTable
#define _SYMBOLTable

#include "lexerDef.h"
#include "ast.h"

#define VAR_BIN_COUNT 13
#define MODULE_BIN_COUNT 13
#define SCOPE_BIN_COUNT 13
#define IO_BIN_COUNT 1


enum _stType {
	DRIVER_ST , MODULE_ST , SCOPE_ST , MODULE_REDEC_ST
} ;

// structures
struct _baseST ;
struct _moduleST ;


struct _arrayInST {
	char *startingPos ;
	char *endingPos ;
	tokenID dataType ;
} ;
struct _varST {
	char * lexeme ; // variable name
	tokenID datatype ; // TK_INTEGER, TK_REAL, TK_BOOL, TK_ARRAY
	int offset ;
	// if TK_ARRAY else unused
	struct _arrayInST * arrayIndices ;
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
} ;


typedef enum _stType stType ;
typedef struct _baseST baseST ;
typedef struct _moduleST moduleST ;
typedef struct _arrayInST arrayInST ;
typedef struct _varST varST ;
typedef struct _varSTentry varSTentry ;
typedef struct _moduleSTentry moduleSTentry ;



//general functions
int hashFunction ( char* lexeme , int size ) ;
char * generateString () ;


// function for base symbol table
baseST * createBaseST () ;


moduleST * createModuleST ( baseST * parent , char * lexeme ) ;
moduleST * createDriverST ( baseST * parent ) ;
moduleST * createScopeST ( moduleST * parent ) ;


varST * createVarST ( astNode * thisASTNode ) ;


baseST * insertModuleSTInbaseST ( baseST * base , moduleST * thisModule) ;
baseST * insertVarSTInbaseST ( baseST * base , varST * thisVarST ) ;
baseST * insertDriverSTInbaseST ( baseST * base , moduleST * thisDriverModule ) ;


moduleST * insertScopeST ( moduleST* parent , moduleST * thisScopeST ) ;
moduleST * insertLocalVarST ( moduleST* thisModule , varST* thisVarST ) ;
moduleST * insertInputVarST ( moduleST* thisModule , varST* thisVarST ) ;
moduleST * insertOutputVarST ( moduleST* thisModule , varST* thisVarST ) ;

//search
varST * searchVarInbaseST ( baseST * base ,char * lexeme ) ;
moduleST * searchModuleInbaseST ( baseST * base, char * lexeme ) ;
moduleST * searchDriverInbaseST ( baseST * base ) ;


varST * searchlocalVarInCurrentModule ( moduleST * thisModule , char * lexeme ) ;
varST * searchInputVarInCurrentModule ( moduleST * thisModule , char * lexeme ) ;
varST * searchOutputVarInCurrentModule ( moduleST * thisModule , char * lexeme ) ;
varST * searchVarInCurrentModule ( moduleST * thisModule , char * lexeme ) ;
varST * searchVar ( moduleST * thisModule , char * lexeme ) ;


void printBaseST ( baseST * base ) ;
void printModuleST ( moduleST * thisModuleST ) ;

baseST * fillSymbolTable (baseST * base , astNode * thisASTNode ) ;
moduleST * fillModuleST ( baseST* realBase ,moduleST* baseModule , astNode * moduleASTNode ) ;

int isValidCall ( baseST * base ,moduleST* thisModule , astNode * funcIDNode , int haveReturns) ;

varST * checkIP (moduleST * thisModule ,moduleST * targetModule , astNode * inputNode ) ;
varST * checkOP (moduleST * thisModule ,moduleST * targetModule , astNode * outputNode ) ;

int getSize( varST * thisVar ) ;

#endif