#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "symbolTable.h"
#include "ast.h"



extern char * current_lexeme ;

/*
struct _varST {
	char * lexeme ;
	tokenID datatype ;
	int offset ;
} ;

struct _moduleST {
	char * lexeme ; // iterations must be given a unique name also
	int isModule ;
	struct _varSTentry * vars[VAR_BIN_COUNT] ;
	struct _moduleSTentry * modules[ITER_BIN_COUNT] ;
	void * parent ;

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
*/





int main() {

	

	return 0 ;
}