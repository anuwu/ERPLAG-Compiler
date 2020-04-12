#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "symbolTable.h"
#include "ast.h"


// make a global initial lexeme
char current_lexeme[20] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ;

extern char* tokenIDToString (tokenID id) ;

int hashFunction ( char* lexeme , int size ) {
	int sm = 0 ;
	int i = 0 ;

	while( lexeme[i] != '\0'){
		sm += lexeme[i] ;
		i++ ;
	}
	return sm%size ;
}


char * generateString () {
	int i=0 ; 

	while ( 1 ) {	
		if(current_lexeme[i] == '\0') {
			// increase string length
			// printf ( "CHAR : ENDLINE\n") ;
			while ( i >= 0 ){
				current_lexeme[i] = 'a' ;
				i-- ;
			}
			return current_lexeme ;
		}
		else if ( current_lexeme[i] == 'z' ){
			current_lexeme[i] = 'a' ;
			i++ ;
		}
		else {
			current_lexeme[i]++ ;
			return current_lexeme ;
		}
	}
	return current_lexeme ;
}


baseST * createBaseST () {
	baseST * tmp = ( baseST *) malloc ( sizeof(baseST)) ;
	tmp->driverST =  NULL ;
	tmp->semanticError = 0 ;
	
	for ( int i = 0 ; i < MODULE_BIN_COUNT ; i++ ) {
		tmp->modules[i] = NULL ;
	}
	for ( int i = 0 ; i < VAR_BIN_COUNT ; i++ ) {
		tmp->vars[i] = NULL ;
	}
	return tmp ;
}

/////////////////////////////////////////////////////////////////////////
moduleST * createModuleST ( baseST * parent , char * lexeme, int currOffset) {
	moduleST * tmp = ( moduleST *) malloc ( sizeof(moduleST)) ;

	tmp->lexeme = lexeme ;
	tmp->tableType = MODULE_ST ;

	for ( int i = 0 ; i < VAR_BIN_COUNT ; i++ ) {
		tmp->localVars[i] = NULL ;
		
	}

	for (int i = 0 ; i < IO_BIN_COUNT ; i++) {
		tmp->inputVars[i] = NULL ;
		tmp->outputVars[i] = NULL ;
	}

	for ( int i = 0 ; i < MODULE_BIN_COUNT ; i++ ) {
		tmp->scopeVars[i] = NULL ;
	}

	tmp->parent = (void *) parent ;
	tmp->currOffset = currOffset ;
	tmp->hasReturns = 0 ;

	return tmp ;
}
moduleST * createDriverST ( baseST * parent ) {
	moduleST * tmp = createModuleST ( parent , "Driver", 0 ) ;
	tmp->tableType = DRIVER_ST ;

	return tmp ;
}
moduleST * createScopeST ( moduleST * parent , stType scopeType) {
	char * lexeme = (char*) malloc(sizeof(char)*20) ;
	strcpy ( lexeme , generateString () ) ;

	moduleST * tmp = createModuleST ( (baseST*)parent , lexeme, parent->currOffset ) ;
	tmp->tableType = scopeType ;

	return tmp ;
}

varST * createVarST ( astNode * thisASTnode, void *scope, variableType varType ) {
	varST * tmp = (varST *) malloc ( sizeof(varST)) ;

	tmp->lexeme = thisASTnode->tok->lexeme ;
	tmp->datatype = thisASTnode->tok->id ;
	tmp->offset = -9999 ; //default value
	tmp->tinker = 0 ;
	tmp->arrayIndices = NULL ;
	tmp->scope = scope ;
	tmp->varType = varType ;

	return tmp ;
}
///////////////////////////////////////////////////////////////////////////
void insertModuleSTInbaseST ( baseST * base , moduleST * thisModule) {

	int index = hashFunction ( thisModule->lexeme , MODULE_BIN_COUNT ) ;

	moduleSTentry * tmp = ( moduleSTentry *) malloc ( sizeof( moduleSTentry)) ;
	tmp->thisModuleST = thisModule ;
	tmp->next = base->modules[index] ;
	base->modules[index] = tmp ;
}

void insertVarSTInbaseST ( baseST * base , varST * thisVarST ) {
	int index = hashFunction ( thisVarST->lexeme , VAR_BIN_COUNT ) ;

	varSTentry * tmp = ( varSTentry *) malloc (sizeof(varSTentry)) ;
	tmp->thisVarST = thisVarST ;
	tmp->next = base->vars[index] ;
	base->vars[index] = tmp ;
}

void insertDriverSTInbaseST ( baseST * base , moduleST * thisDriverModule ) {
	base->driverST = thisDriverModule ;
}

///////////////////////////////////////////////////////////////////////////
void insertScopeST ( moduleST* parent , moduleST * thisScopeST ) {
	int index = hashFunction ( thisScopeST->lexeme , MODULE_BIN_COUNT ) ;

	moduleSTentry * tmp = ( moduleSTentry * ) malloc ( sizeof ( moduleSTentry )) ;

	tmp->thisModuleST = thisScopeST ;
	tmp->next = (parent->scopeVars)[index] ;
	(parent->scopeVars)[index] = tmp ;
}


void insertLocalVarST ( moduleST* thisModule , varST* thisVarST ) {
	int index = hashFunction ( thisVarST->lexeme , VAR_BIN_COUNT ) ;

	varSTentry * tmp = ( varSTentry * ) malloc ( sizeof(varSTentry)) ;
	tmp->thisVarST = thisVarST ;
	tmp->next = thisModule->localVars[index] ; 
	thisModule->localVars[index] = tmp ;
}

void insertInputVarST ( moduleST* thisModule , varST* thisVarST ) {
	int index = hashFunction ( thisVarST->lexeme , IO_BIN_COUNT ) ;

	varSTentry * tmp = ( varSTentry * ) malloc ( sizeof(varSTentry)) ;
	tmp->thisVarST = thisVarST ;
	tmp->next = thisModule->inputVars[index] ; 
	thisModule->inputVars[index] = tmp ;
}

void insertOutputVarST ( moduleST* thisModule , varST* thisVarST ) {
	int index = hashFunction ( thisVarST->lexeme , IO_BIN_COUNT ) ;

	varSTentry * tmp = ( varSTentry * ) malloc ( sizeof(varSTentry)) ;
	tmp->thisVarST = thisVarST ;
	tmp->next = thisModule->outputVars[index] ; 
	thisModule->outputVars[index] = tmp ;

	if (!thisModule->hasReturns)
		thisModule->hasReturns = 1 ;
}


///////////////////////////////////////////////////////////////////////


varST * searchVarInbaseST ( baseST * base ,char * lexeme ) {
	int index = hashFunction ( lexeme , VAR_BIN_COUNT ) ;

	varSTentry * tmp = base->vars[index] ;
	while ( tmp!= NULL ){
		if( strcmp(tmp->thisVarST->lexeme , lexeme) == 0 )
			return tmp->thisVarST ;
		
		tmp = tmp->next ;
	}
	return NULL ;
}


moduleST * searchModuleInbaseST ( baseST * base, char * lexeme ) {
	int index = hashFunction ( lexeme , MODULE_BIN_COUNT ) ;

	moduleSTentry * tmp = base->modules[index] ;
	while ( tmp!= NULL ) {
		if ( (tmp->thisModuleST->tableType == MODULE_ST ||  tmp->thisModuleST->tableType == MODULE_REDEC_ST)&& strcmp ( tmp->thisModuleST->lexeme , lexeme ) == 0 )
		{
			//printf ("REPEAT !\n") ;
			return tmp->thisModuleST ;
		}

		tmp = tmp->next ;
	}
	return NULL ;
}

moduleST * searchDriverInbaseST ( baseST * base ) {
	return base->driverST ;
}


/////////////////////////////////////////////////////////////////////////////

varST * searchlocalVarInCurrentModule ( moduleST * thisModule , char * lexeme ) {
	int index = hashFunction ( lexeme , VAR_BIN_COUNT ) ;

	varSTentry * tmp = thisModule->localVars[index] ;
	while ( tmp!= NULL ){
		if( strcmp(tmp->thisVarST->lexeme , lexeme) == 0 )
			return tmp->thisVarST ;
		
		tmp = tmp->next ;
	}
	return NULL ;
}
varST * searchInputVarInCurrentModule ( moduleST * thisModule , char * lexeme ) {
	int index = hashFunction ( lexeme , IO_BIN_COUNT ) ;

	varSTentry * tmp = thisModule->inputVars[index] ;
	while ( tmp!= NULL ){
		if( strcmp(tmp->thisVarST->lexeme , lexeme) == 0 )
			return tmp->thisVarST ;
		
		tmp = tmp->next ;
	}
	return NULL ;
}
varST * searchOutputVarInCurrentModule ( moduleST * thisModule , char * lexeme ) {
	int index = hashFunction ( lexeme , IO_BIN_COUNT ) ;

	varSTentry * tmp = thisModule->outputVars[index] ;
	while ( tmp!= NULL ){
		if( strcmp(tmp->thisVarST->lexeme , lexeme) == 0 )
			return tmp->thisVarST ;
		
		tmp = tmp->next ;
	}
	return NULL ;
}


varST * searchVarInCurrentModule ( moduleST * thisModule , char * lexeme ) {
	varST * tmp ;
	if( tmp = searchlocalVarInCurrentModule ( thisModule , lexeme ) )
		return tmp ;
	else if ( tmp = searchInputVarInCurrentModule(thisModule,lexeme) )
		return tmp ;
	else 
		return searchOutputVarInCurrentModule (thisModule,lexeme) ;
}


varST * searchVar (baseST* realBase, moduleST * thisModule , char * lexeme ) 
{
	
	if (thisModule->parent == realBase)
		return searchVarInCurrentModule ( thisModule , lexeme ) ;
	else
	{
		varST *tmp = searchlocalVarInCurrentModule (thisModule, lexeme) ;
		if (tmp == NULL)
			return searchVar (realBase, thisModule->parent, lexeme) ;
		else
			return tmp ;
	}
}

char* varTypeToString (variableType varType)
{
	switch (varType)
	{
		case VAR_INPUT :
			return "Input" ;
		case VAR_OUTPUT :
			return "Output" ;
		case VAR_LOOP :
			return "Counter" ;
		case VAR_MODULE :
			return "Module" ;
		case VAR_LOCAL :
			return "Local" ;
	}
}


////////////////////////////////////////////////////////////////////////////
void printBaseST ( baseST * base ) {
	
	printf("************ printBaseST **************\n") ;

	if(base == NULL)
		return ;

	printf("DriverST : %s\n" , base->driverST ? base->driverST->lexeme : "NULL" ) ;

	printf("\nModules of baseST:\n") ;
	for ( int i=0 ; i<MODULE_BIN_COUNT ; i++ ) {
		moduleSTentry * tt = base->modules[i] ;
		while( tt ){
			printf("\t%s" , tt->thisModuleST->lexeme ) ;
			tt = tt->next ;
		}
	}

	printf("\nVariables of baseST:\n") ;
	for ( int i=0 ; i<VAR_BIN_COUNT ; i++ ) {
		varSTentry * tt = base->vars[i] ;
		while( tt ){
			printf("\t%s" , tt->thisVarST->lexeme ) ;
			tt = tt->next ;
		}
	}


	printf("\n**************************************\n") ;
}



void printModuleST ( moduleST * thisModuleST, int printParam ) {

	if (!printParam)
		return ;

	if( thisModuleST->tableType == DRIVER_ST || thisModuleST->tableType == MODULE_ST ){
		printf("**************printModuleST( %s )***************\n",thisModuleST->lexeme ) ;
	}
	else {
		printf("**************printModuleST(SCOPE : %s )***************\n",thisModuleST->lexeme ) ;
		//printf("It's a ScopeST \n") ;
		printf("Parent : %s\n" , thisModuleST->parent ? ((moduleST*)thisModuleST->parent)->lexeme : "NULL") ;
	}
	
	moduleSTentry * tt ;
	printf("Modules :\n" ) ;
	for ( int i=0 ; i<MODULE_BIN_COUNT ; i++ ){
		
		tt = thisModuleST->scopeVars[i] ;
		while ( tt != NULL ) {
			printf("\t%s --> Label\n",tt->thisModuleST->lexeme) ;
			tt = tt->next ;
		}
	}

	printf("\nINPUT Variables :\n" ) ;
	for ( int i = 0 ; i<IO_BIN_COUNT ; i++ ){
		
		varSTentry * vv = thisModuleST->inputVars[i] ;

		while ( vv ) {
			printf ("\toffset: %d - %s ---- %s ",vv->thisVarST->offset,vv->thisVarST->lexeme , tokenIDToString(vv->thisVarST->datatype) ) ;
			if(vv->thisVarST->datatype == TK_ARRAY ) {
				printf ( "%s(%s,%s)",tokenIDToString(vv->thisVarST->arrayIndices->type) ,vv->thisVarST->arrayIndices->tokLeft->lexeme , vv->thisVarST->arrayIndices->tokRight->lexeme ) ;
			}
			printf ("\n") ;


			vv = vv->next ;
		}
	}
	printf("\nOUTPUT Variables :\n" ) ;
	for ( int i = 0 ; i<IO_BIN_COUNT ; i++ ){
		
		varSTentry * vv = thisModuleST->outputVars[i] ;

		while ( vv ) {
			printf ("\toffset: %d - %s ---- %s ",vv->thisVarST->offset,vv->thisVarST->lexeme , tokenIDToString(vv->thisVarST->datatype) ) ;
			if(vv->thisVarST->datatype == TK_ARRAY ) {
				printf ( "%s(%s,%s)",tokenIDToString(vv->thisVarST->arrayIndices->type),vv->thisVarST->arrayIndices->tokLeft->lexeme , vv->thisVarST->arrayIndices->tokRight->lexeme ) ;
			}
			printf ("\n") ;


			vv = vv->next ;
		}
	}
	printf("\nLOCAL Variables :\n" ) ;
	for ( int i = 0 ; i<VAR_BIN_COUNT ; i++ ){
		
		varSTentry * vv = thisModuleST->localVars[i] ;

		while ( vv ) 
		{
			printf ("\toffset: %d - %s ---- %s ",vv->thisVarST->offset,vv->thisVarST->lexeme , tokenIDToString(vv->thisVarST->datatype) ) ;

			if(vv->thisVarST->datatype == TK_ARRAY ) 
			{
				printf ( "%s(%s,%s)",tokenIDToString(vv->thisVarST->arrayIndices->type),vv->thisVarST->arrayIndices->tokLeft->lexeme , vv->thisVarST->arrayIndices->tokRight->lexeme ) ;
			}


			printf (" ---> %s" , varTypeToString (vv->thisVarST->varType)) ;

			printf ("\n") ;


			vv = vv->next ;
		}
	}
	
	printf("\n**************************************\n") ;
}

int isVarStaticArr (varST *arrayVar)
{
	return (arrayVar->offset >= 0 || arrayVar->offset == -2) ;
}



varST * checkIP (baseST *realBase, moduleST * thisModule ,moduleST * targetModule , astNode * inputNode ) 
{	
	varSTentry * varEntry = targetModule->inputVars[0] ;	
	astNode * inputIter = inputNode ;
	while(inputIter->next) {
		inputIter = inputIter->next ;
	}

	int sameArgNoErr = 0 ;
	
	while( inputIter && varEntry )
	{
		varST * searchedVar = searchVar(realBase, thisModule , inputIter->tok->lexeme ) ;

		if(searchedVar == NULL)
		{
			printf("ERROR : In \"%s\" at line %d, \"%s\" variable not declared\n",getParentModuleName(realBase, thisModule) ,inputIter->tok->lineNumber ,inputIter->tok->lexeme) ;
			realBase->semanticError = 1 ;
			sameArgNoErr = 1 ;
		}
		else {
			if(varEntry->thisVarST->datatype == searchedVar->datatype)
			{
				// Correct it
				if (varEntry->thisVarST->datatype == TK_ARRAY && isVarStaticArr (searchedVar))
				{
					if (strcmp(varEntry->thisVarST->arrayIndices->tokLeft->lexeme, searchedVar->arrayIndices->tokLeft->lexeme) || strcmp(varEntry->thisVarST->arrayIndices->tokRight->lexeme, searchedVar->arrayIndices->tokRight->lexeme))
					{
						printf ("ERROR : In \"%s\" at line %d, \"%s\" and \"%s\" do not have matching array limits\n" ,  getParentModuleName(realBase, thisModule) ,inputIter->tok->lineNumber , inputIter->tok->lexeme , varEntry->thisVarST->lexeme) ;
						realBase->semanticError = 1 ;
					}
				}
			}
			else 
			{
				printf ("ERROR : In \"%s\" at line %d, inputs \"%s\" and \"%s\" have conflicting types\n" ,  getParentModuleName(realBase, thisModule) ,inputIter->tok->lineNumber , inputIter->tok->lexeme , varEntry->thisVarST->lexeme) ;

				sameArgNoErr = 1 ;
				realBase->semanticError = 1 ;
			}
		}
		
		varEntry = varEntry->next ;
		inputIter = inputIter->prev ;
	}


	if (varEntry == NULL && inputIter)
	{
		printf("ERROR : In \"%s\" at line %d More parameters passed\n", getParentModuleName(realBase, thisModule), inputIter->tok->lineNumber) ;
		realBase->semanticError = 1 ;
		return (varST *) malloc (sizeof(varST)) ;
	}
	else if (varEntry && inputIter == NULL) 
	{
		printf("ERROR : In \"%s\" at line %d insufficient number of actual parameters\n", getParentModuleName(realBase, thisModule), inputNode->tok->lineNumber) ;
		realBase->semanticError = 1 ;

		return (varST *) malloc (sizeof(varST)) ;
	}
	else{

		if (sameArgNoErr)
		{
			realBase->semanticError = 1 ;
			return (varST *) malloc (sizeof(varST)) ;
		}
		else
			return NULL ;
	}
}


varST * checkOP (baseST *realBase, moduleST * thisModule ,moduleST * targetModule , astNode * outputNode ) 
{
	varSTentry * varEntry = targetModule->outputVars[0] ;
	int sameArgNoErr = 0 ;

	
	astNode * outputIter = outputNode ;
	while(outputIter->next) {
		outputIter = outputIter->next ;
	}
	
	while( outputIter && varEntry ){

		varST * searchedVar = searchVar(realBase, thisModule , outputIter->tok->lexeme ) ;

		if(searchedVar == NULL)
		{
			printf("ERROR : In \"%s\" at line %d, \"%s\" variable not declared\n", getParentModuleName(realBase, thisModule) , outputIter->tok->lineNumber , outputIter->tok->lexeme ) ;
			sameArgNoErr = 1 ;
			realBase->semanticError = 1 ;
		}
		else {
			if(varEntry->thisVarST->datatype != searchedVar->datatype)
			{
				printf ( "ERROR : In \"%s\" at line %d, \"%s\" and \"%s\" have conflicting return types\n" , getParentModuleName(realBase, thisModule), outputIter->tok->lineNumber, outputIter->tok->lexeme , varEntry->thisVarST->lexeme) ;
				sameArgNoErr = 1 ;
				realBase->semanticError = 1 ;
			}
		}
		
		varEntry = varEntry->next ;
		outputIter = outputIter->prev ;
	}

	if (varEntry==NULL && outputIter){
		printf("ERROR : In \"%s\" at line %d, more parameters passed\n", getParentModuleName(realBase, thisModule), outputIter->tok->lineNumber) ;
		realBase->semanticError = 1 ;
		return searchVar(realBase, thisModule , outputIter->tok->lexeme ) ;
	}
	else if (varEntry && outputIter==NULL) {
		printf("ERROR : In \"%s\" at line %d, Insufficient number of parameters\n", getParentModuleName(realBase, thisModule), outputNode->tok->lineNumber) ;
		realBase->semanticError = 1 ;
		return varEntry->thisVarST ;
	}
	else{
		if (sameArgNoErr)
		{
			realBase->semanticError = 1 ;
			return (varST *) malloc (sizeof(varST)) ;
		}
		else
			return NULL ;
	}
}



int isValidCall ( baseST * base, moduleST * thisModule , astNode * funcNode , int haveReturns ) {

	//printf ("\t> isValidCall\n") ;

	if(haveReturns == 0 ) {
		varST * varPtr = searchVarInbaseST(base , funcNode->tok->lexeme ) ;
		moduleST * modPtr = searchModuleInbaseST ( base , funcNode->tok->lexeme) ;

		if (modPtr != NULL)
		{
			if (modPtr->hasReturns)
			{
				printf ("ERROR : In \"%s\" at line %d, call to function \"%s\" has return data but no receiving variables\n", getParentModuleName(base, thisModule), funcNode->tok->lineNumber, modPtr->lexeme) ;
				base->semanticError = 1 ;
			}


			if (modPtr->tableType == MODULE_REDEC_ST)
			{
				printf ("ERROR : In \"%s\" at line %d, module \"%s\" found, but with clashing definition\n" , getParentModuleName(base, thisModule),funcNode->tok->lineNumber ,modPtr->lexeme) ;
				base->semanticError = 1 ;
				return -3 ;
			}

			if (checkIP(base, thisModule,modPtr,funcNode->next->child) !=NULL) {
				return -3 ;		// Errors printed in checkIP
			}
			else{
				return 1 ;
			}
		}
		else
		{
			if (varPtr == NULL)
				return -1 ;
			else
				return -2 ;
		}
	}
	else if ( haveReturns == 1 ) {
		varST * varPtr = searchVarInbaseST(base , funcNode->next->next->tok->lexeme ) ;
		moduleST * modPtr = searchModuleInbaseST ( base , funcNode->next->next->tok->lexeme) ;

		if (modPtr != NULL)
		{
			if (!modPtr->hasReturns)
			{
				printf ("ERROR : In \"%s\" at line %d, call to function \"%s\" has no return data but receiving variables are present\n", getParentModuleName(base, thisModule), funcNode->next->next->tok->lineNumber, modPtr->lexeme) ;
				base->semanticError = 1 ;
			}


			if (modPtr->tableType == MODULE_REDEC_ST)
			{
				printf ("ERROR : In \"%s\" at line %d, Module %s was found, but with repeating definition\n" ,getParentModuleName(base, thisModule) , funcNode->next->next->tok->lineNumber ,modPtr->lexeme) ;
				return -3 ;
			}
			else
				;
				//printf("\t> Definition found\n") ;
			
			if (checkIP(base, thisModule,modPtr,funcNode->next->next->next->child) !=NULL | checkOP(base, thisModule,modPtr,funcNode->child) !=NULL ) {
				return -3 ;		// Errors printed in checkIP and checkOP
			}
			else{
				return 1 ;
			}
		}
		else
		{
			if (varPtr == NULL)
				return -1 ;
			else
				return -2 ;
		}
	}
	
}

int getSize(baseST * realBase, varST * thisVar) {

	if(thisVar->datatype== TK_INTEGER)
		return 2 ;
	else if (thisVar->datatype == TK_BOOLEAN)
		return 1 ;
	else if(thisVar->datatype == TK_REAL)
		return 4 ;
	else if (thisVar->datatype == TK_ARRAY) 
	{
		int left, right, indexErrorFlag = 0 ;
		char *parentModule = getParentModuleName (realBase , (moduleST *)thisVar->scope) ;

		if (isdigit (thisVar->arrayIndices->tokLeft->lexeme[0]) && isdigit (thisVar->arrayIndices->tokRight->lexeme[0]))
		{
			int sz ;

			left = atoi(thisVar->arrayIndices->tokLeft->lexeme) ;
			right = atoi(thisVar->arrayIndices->tokRight->lexeme) ;


			if (thisVar->arrayIndices->type == TK_INTEGER)
				sz = 2 ;
			else if (thisVar->arrayIndices->type == TK_BOOLEAN)
				sz = 1 ;
			else if (thisVar->arrayIndices->type == TK_REAL)
				sz = 4 ;

			sz *= (right - left + 1) ;

			if (sz <= 0)
			{
				printf ("ERROR : In definition of \"%s\" at line %d, Left index of \"%s\" must be <= right index\n", parentModule, thisVar->arrayIndices->tokLeft->lineNumber, thisVar->lexeme) ;
				realBase->semanticError = 1 ;
				return -2 ;
			}
			else
				return sz  ;
		}
		else
		{
			// dynamic array index checks
			if (!isdigit (thisVar->arrayIndices->tokLeft->lexeme[0]) && !isdigit (thisVar->arrayIndices->tokRight->lexeme[0]))
			{
				if (searchVar (realBase, (moduleST *)thisVar->scope, thisVar->arrayIndices->tokLeft->lexeme) == NULL)
				{
					printf ("ERROR : In \"%s\", Left index \"%s\" of local variable \"%s\" is undeclared\n", parentModule, thisVar->arrayIndices->tokLeft->lexeme, thisVar->lexeme) ;
					indexErrorFlag = 1 ;
				}
				if (searchVar (realBase, (moduleST *)thisVar->scope, thisVar->arrayIndices->tokRight->lexeme) == NULL)
				{
					printf ("ERROR : In \"%s\", Right index \"%s\" of local variable \"%s\" is undeclared\n", parentModule, thisVar->arrayIndices->tokRight->lexeme, thisVar->lexeme) ;
					indexErrorFlag = 1 ;
				}

				if (indexErrorFlag)
				{
					realBase->semanticError = 1 ;
					return -3 ;		// incorect dynamic array
				}
			}
			else if (!isdigit (thisVar->arrayIndices->tokLeft->lexeme[0]) && isdigit (thisVar->arrayIndices->tokRight->lexeme[0]))
			{
				// Left dynamic, right static
				if (searchVar (realBase, (moduleST *)thisVar->scope, thisVar->arrayIndices->tokLeft->lexeme) == NULL)
				{
					printf ("ERROR : In \"%s\", Left index \"%s\" of local variable \"%s\" is undeclared\n", parentModule, thisVar->arrayIndices->tokLeft->lexeme, thisVar->lexeme) ;

					realBase->semanticError = 1 ;
					return -3 ;		// incorrect dynamic array
				}
			}
			else
			{
				// left static, right dynamic
				if (searchVar (realBase, (moduleST *)thisVar->scope, thisVar->arrayIndices->tokRight->lexeme) == NULL)
				{
					printf ("ERROR : In \"%s\", Left index \"%s\" of local variable \"%s\" is undeclared\n", parentModule, thisVar->arrayIndices->tokRight->lexeme, thisVar->lexeme) ;

					realBase->semanticError = 1 ;
					return -3 ;		// incorrect dynamic array
				}
			}

			return -1 ;		// correct dynamic array
		}
	}
}

char *getParentModuleName (baseST* realBase, moduleST *scope)
{
	if (scope->parent == realBase)
		return scope->lexeme ;
	else
		return getParentModuleName (realBase, scope->parent) ;
}

char *typeIDToString (tokenID id)
{
	switch (id)
	{
		case TK_INTEGER :
			return "integer" ;
		case TK_BOOLEAN :
			return "boolean" ;
		case TK_REAL :
			return "real" ;
	}
}

int caseValRepeat (astNode *caseAstNode)
{
	char *searchLex = caseAstNode->next->tok->lexeme ;

	caseAstNode = caseAstNode->prev->prev->prev ;
	while (caseAstNode)
	{
		if (strcmp(searchLex , caseAstNode->next->tok->lexeme) == 0)
			return 1 ;
		caseAstNode = caseAstNode->prev->prev->prev ;
	}

	return 0 ;
}


int validVarIndex (baseST *realBase, moduleST *baseModule, astNode *varIndexASTNode)
{
	// Assumes varIndexASTNode isn't NULL

	if (varIndexASTNode->id == TK_NUM)
		return TK_NUM ;

	varST *indexVar = searchVar (realBase, baseModule, varIndexASTNode->tok->lexeme) ;
	if (indexVar == NULL)
	{
		printf ("ERROR : In \"%s\" at line %d, index variable \"%s\" is undeclared\n", getParentModuleName(realBase, baseModule), varIndexASTNode->tok->lineNumber, varIndexASTNode->tok->lexeme) ;
		return 0 ;
	}
	else if (indexVar->datatype != TK_INTEGER)
	{
		printf ("ERROR : In \"%s\" at line %d, index variable \"%s\" needs to be of type integer\n", getParentModuleName(realBase, baseModule), varIndexASTNode->tok->lineNumber, varIndexASTNode->tok->lexeme) ;
		return 0 ;
	}

	return TK_ID ;
}

/*
searchVar ()
	--> NULL 											==> ERROR
	--> NOT NULL
		--> Primitive
		--> Array
				--> dynamic array
					--> dynamic index 					==> searchVar
						--> NULL						==> ERROR : index variable does not exist
						--> NOT NULL
							--> integer type
							--> non-integer type 		==> ERROR : index variable is not of integer type
					--> static index 					==> No checks 
				--> static array
					--> dynamic index 					==> searchVar
						--> NULL						==> ERROR : index variable does not exist
						--> NOT NULL
							--> integer type 			 
							--> non-integer type 		==> ERROR : index variable not of integer type
					--> static index 	==> Check bounds
						--> Correct bounds
						--> Incorrect bounds 			==> ERROR : incorrect bounds for static array
*/

int validStaticArrStaticIndex (baseST *realBase, moduleST *baseModule, varST *arrVar, astNode *indASTNode)
{
	int num = atoi(indASTNode->tok->lexeme) , leftLim = atoi(arrVar->arrayIndices->tokLeft->lexeme), rightLim = atoi(arrVar->arrayIndices->tokRight->lexeme) ;
	if (num < leftLim || num > rightLim)
	{
		printf ("ERROR : In \"%s\" at line %d, specified index %s is out of bounds for static array \"%s\" with limits [%d .. %d]\n", getParentModuleName(realBase, baseModule), indASTNode->tok->lineNumber, indASTNode->tok->lexeme, arrVar->lexeme, leftLim, rightLim) ;
		return 0 ;
	}

	return 1 ;
}


int validVar (baseST *realBase , moduleST *baseModule , astNode *varASTNode)
{
	int isValidVar = 1 , isValidVarIndex ;
	varST *searchedVar = searchVar (realBase, baseModule, varASTNode->tok->lexeme) ;

	if (searchedVar == NULL)
	{
		printf ("ERROR : In \"%s\" at line %d, variable \"%s\" is undeclared\n", getParentModuleName(realBase, baseModule), varASTNode->tok->lineNumber, varASTNode->tok->lexeme) ;

		isValidVar = 0 ;
		if (varASTNode->child != NULL)
			validVarIndex (realBase, baseModule, varASTNode->child) ;
	}
	else if (searchedVar->datatype != TK_ARRAY)
	{
		if (varASTNode->child != NULL)
		{
			isValidVar = 0 ;
			printf ("ERROR : In \"%s\" at line %d, primitive %s variable \"%s\" cannot be indexed\n", getParentModuleName(realBase, baseModule), varASTNode->child->tok->lineNumber, typeIDToString (searchedVar->datatype), searchedVar->lexeme) ;
			validVarIndex (realBase, baseModule, varASTNode->child) ;
		}
	}
	else if (varASTNode->child != NULL)		// It is array and has an index
	{
		isValidVarIndex = validVarIndex(realBase, baseModule, varASTNode->child) ;

		if (isVarStaticArr (searchedVar) && isValidVarIndex == TK_NUM)		// Static array
		{
			if (!validStaticArrStaticIndex (realBase, baseModule, searchedVar, varASTNode->child))
				isValidVar = 0 ;
		}
		else if (!isValidVarIndex)
			isValidVar = 0 ;
	}

	return isValidVar ;
}


void fillModuleST ( baseST* realBase , moduleST* baseModule , astNode * statementsAST , int depthSTPrint) 
{
	int getOffsetSize = 0 ;
	statementsAST->localST = baseModule ;
	astNode * statementAST = statementsAST->child ;

	while (statementAST) {
		if(statementAST->child->id == TK_DECLARE ) {
			// declare statement
			astNode * idAST = statementAST->child->next->child ;
			astNode * dataTypeAST = statementAST->child->next->next ;

			while ( idAST ) 
			{
				varST *searchedVar ;
				searchedVar = searchVar (realBase, baseModule, idAST->tok->lexeme) ;

				if (searchedVar != NULL && searchedVar->scope == baseModule && searchedVar->varType != VAR_INPUT)
				{
					printf ( "ERROR : In \"%s\" at line %d, %s variable \"%s\" already declared\n", getParentModuleName (realBase, baseModule) ,idAST->tok->lineNumber, varTypeToString(searchedVar->varType) , idAST->tok->lexeme) ;
					realBase->semanticError = 1 ;
				}
				else		// if input variable, or a local variable in some parent scope, or not found at all
				{
					varST * tmp = createVarST ( idAST , baseModule , VAR_LOCAL) ;

					if (dataTypeAST->dtTag  == ARRAY ){
						tmp->datatype = TK_ARRAY ;
						tmp->arrayIndices = dataTypeAST->dt->arrType ; 
					}
					else {
						tmp->datatype = dataTypeAST->dt->pType ;
					}

					// filling offset
					getOffsetSize = getSize(realBase, tmp) ;

					if (getOffsetSize > 0)
					{
						tmp->offset = baseModule->currOffset ;
						baseModule->currOffset += getOffsetSize ;
					}
					else	// correct dynamic array, or invalid static/dynamic array
						tmp->offset = getOffsetSize ;

					insertLocalVarST(baseModule , tmp) ;
				}

				idAST = idAST->next ;
			}

		}
		else if ( statementAST->child->id == TK_WHILE ) {
			// TODO type checking

			moduleST * tmp = createScopeST ( baseModule, WHILE_ST ) ;
			fillModuleST ( realBase , tmp , statementAST->child->next->next, depthSTPrint) ;
			printModuleST (tmp,  depthSTPrint) ;

			insertScopeST ( baseModule , tmp ) ;
		}
		else if ( statementAST->child->id == TK_FOR ) {

			varST *searchedVar = searchVar (realBase, baseModule, statementAST->child->next->tok->lexeme) ;
			if (searchedVar == NULL)
			{
				printf ("ERROR : In \"%s\" at line %d, loop variable \"%s\" is undeclared\n", getParentModuleName(realBase, baseModule), statementAST->child->next->tok->lineNumber, statementAST->child->next->tok->lexeme) ;
				realBase->semanticError = 1 ;
			}
			else if (searchedVar->datatype != TK_INTEGER)
			{
				printf ("ERROR : In \"%s\" at line %d, loop variable \"%s\" needs to be of integer type\n", getParentModuleName(realBase, baseModule), statementAST->child->next->tok->lineNumber, statementAST->child->next->tok->lexeme) ;
				realBase->semanticError = 1 ;
			}

			astNode *loopLim; 
			loopLim = statementAST->child->next->next ;

			if (atoi(loopLim->tok->lexeme) > atoi(loopLim->next->tok->lexeme))
			{
				printf ("ERROR : In \"%s\" at line %d, left loop limit must be <= right loop limit\n", getParentModuleName(realBase, baseModule), loopLim->tok->lineNumber) ;
				realBase->semanticError = 1 ;
			}

			moduleST *forScope = createScopeST (baseModule, FOR_ST) ;
			varST *loopVar = createVarST (statementAST->child->next, forScope, VAR_LOOP) ;
			loopVar->datatype = TK_INTEGER ;

			if (searchedVar == NULL)
				loopVar->offset = -2 ;		// VAR_LOOP with offset of -2 is undeclared loop counter
			else
				loopVar->offset = searchedVar->offset ;

			insertLocalVarST (forScope, loopVar) ;
			fillModuleST (realBase, forScope, loopLim->next->next, depthSTPrint) ;

			printModuleST (forScope, depthSTPrint) ;
			insertScopeST (baseModule , forScope) ;
		}
 		else if ( statementAST->child->id == TK_GET_VALUE ) {
			varST * thisVar = searchVar(realBase, baseModule , statementAST->child->next->tok->lexeme) ;

			if (thisVar == NULL)
			{
				printf ("ERROR : In \"%s\" at line %d, \"%s\" variable undeclared\n" ,  getParentModuleName(realBase, baseModule), statementAST->child->next->tok->lineNumber , statementAST->child->next->tok->lexeme ) ;
				realBase->semanticError = 1 ;
			}
			else
			{
				if (thisVar->varType == VAR_OUTPUT)
					thisVar->tinker = 1 ;
				else if (thisVar->varType == VAR_LOOP)
				{
					printf ("ERROR : In \"%s\" at line %d, loop variable \"%s\" cannot be modified\n" ,  getParentModuleName(realBase, baseModule), statementAST->child->next->tok->lineNumber , statementAST->child->next->tok->lexeme) ;
					realBase->semanticError = 1 ;
				}
			}
		}
 		else if ( statementAST->child->id == TK_PRINT) 
 		{
 			if (statementAST->child->next->id == TK_ID)
 			{
 				if (!validVar (realBase, baseModule, statementAST->child->next))
 					realBase->semanticError = 1 ;
			}
		}
		else if ( statementAST->child->id == /*TK_ID*/ TK_ASSIGNOP) 
		{
			// TODO typechecking
			if (validVar(realBase, baseModule, statementAST->child->child))
			{
				varST *searchedVar = searchVar(realBase, baseModule, statementAST->child->child->tok->lexeme) ;
				if (searchedVar != NULL)
				{
					if (searchedVar->varType == VAR_OUTPUT)
						searchedVar->tinker = 1 ;
					else if (searchedVar->varType == VAR_LOOP)
					{
						printf ("ERROR : In \"%s\" at line %d, loop variable \"%s\" cannot be modified\n",  getParentModuleName(realBase, baseModule),statementAST->child->child->tok->lineNumber ,  statementAST->child->child->tok->lexeme) ;
						realBase->semanticError = 1 ;
					}
				}
			}

			// RHS business will occur here.
		}
 		else if ( statementAST->child->id == idList) 
 		{
			// [a] = use module with parameters [ d ] ;

			if (strcmp(baseModule->lexeme, statementAST->child->next->next->tok->lexeme) == 0)
			{
				printf ("ERROR : In \"%s\" at line %d, recursion is not allowed!\n", getParentModuleName(realBase, baseModule), statementAST->child->next->next->tok->lineNumber) ;
				realBase->semanticError = 1 ;
			}
			else
			{
				int validCallFlag = isValidCall (realBase, baseModule,statementAST->child, 1) ;
				if ( validCallFlag == 1) {
					//printf ("%s calling %s ALL GOOD\n", baseModule->lexeme ,statementAST->child->next->next->tok->lexeme) ;
					;
				}
				else if (validCallFlag == -1 )	{
					printf("ERROR : In \"%s\" at line %d, \"%s\" Module neither declared nor defined \n", getParentModuleName(realBase, baseModule), statementAST->child->next->next->tok->lineNumber, statementAST->child->next->next->tok->lexeme) ;
					realBase->semanticError = 1 ;
				}
				else if( validCallFlag == -2 ) {
					printf( "ERROR : In \"%s\" at line %d, \"%s\" declared but not defined \n", getParentModuleName(realBase, baseModule), statementAST->child->next->next->tok->lineNumber, statementAST->child->next->next->tok->lexeme) ;
					realBase->semanticError = 1 ;
				}
				// else error messages is printed in internal functions
			}

			// Tinkering check
		}
		else if (statementAST->child->id == TK_ID) 
		{
			// use module with parameters ... (no return)

			if (strcmp(baseModule->lexeme, statementAST->child->tok->lexeme) == 0)
			{
				printf ("ERROR : In \"%s\" at line %d, recursion is not allowed!\n", getParentModuleName(realBase, baseModule), statementAST->child->tok->lineNumber) ;
				realBase->semanticError = 1 ;
			}
			else
			{
				int validCallFlag = isValidCall ( realBase , baseModule,statementAST->child , 0 ) ;
				if ( validCallFlag == 1 ) {
					//printf ("%s calling %s ALL GOOD\n", baseModule->lexeme ,statementAST->child->tok->lexeme) ;
					;
				}
				else if (validCallFlag == -1 )	{
					printf( "ERROR : In \"%s\" at line %d, \"%s\" Module neither declared nor defined \n", getParentModuleName(realBase, baseModule), statementAST->child->tok->lineNumber , statementAST->child->tok->lexeme) ;
					realBase->semanticError = 1 ;
				}
				else if( validCallFlag == -2 ) {
					printf( "ERROR : In \"%s\" at line %d, \"%s\" Module declared but not defined \n", getParentModuleName(realBase, baseModule),statementAST->child->tok->lineNumber , statementAST->child->tok->lexeme) ;
					realBase->semanticError = 1 ;
				}
			}
			// else error messages is printed in internal functions
		}
 		else if (statementAST->child->id == TK_SWITCH)
		{
			int switchTypeError = 0 ;

			varST *searchedVar = searchVar (realBase, baseModule , statementAST->child->next->tok->lexeme) ;
			if (searchedVar == NULL)
			{
				printf ("ERROR : In \"%s\" at line %d, \"%s\" switch variable undeclared\n",  getParentModuleName(realBase, baseModule),statementAST->child->next->tok->lineNumber ,  statementAST->child->next->tok->lexeme) ;
				realBase->semanticError = 1 ;
			}
			else
			{
				if (!(searchedVar->datatype == TK_INTEGER || searchedVar->datatype == TK_BOOLEAN))
				{
					printf ("ERROR : In \"%s\" at line %d, \"%s\" switch variable must be integer or boolean\n",  getParentModuleName(realBase, baseModule),statementAST->child->next->tok->lineNumber ,  statementAST->child->next->tok->lexeme) ;
					realBase->semanticError = 1 ;
					switchTypeError = 1 ;
				}
			}

			moduleST *switchST = createScopeST (baseModule, SWITCH_ST) ;
			astNode *caseAstNode = statementAST->child->next->next ;

			int hasDefault = 0 ;

			while (caseAstNode != NULL)
			{
				// case value mismatch
				if (!switchTypeError && caseAstNode->id != TK_DEFAULT)
				{
					if ((searchedVar->datatype == TK_INTEGER && (caseAstNode->next->id == TK_TRUE || caseAstNode->next->id == TK_FALSE))|| (searchedVar->datatype == TK_BOOLEAN && caseAstNode->next->id == TK_NUM)) 
					{
						printf ("ERROR : In \"%s\" at line %d, switch variable has type %s and case value has type %s\n", getParentModuleName(realBase, baseModule), caseAstNode->next->tok->lineNumber, typeIDToString (searchedVar->datatype) , (caseAstNode->next->id == TK_NUM)?"integer":"boolean") ;
						realBase->semanticError = 1 ;
					}
				}
				
				if (caseAstNode->id != TK_DEFAULT)
				{
					if (caseValRepeat (caseAstNode))
					{
						printf ("ERROR : In \"%s\" at line %d, case value is repeating\n", getParentModuleName(realBase, baseModule), caseAstNode->next->tok->lineNumber) ;
						realBase->semanticError = 1 ;
					}

					fillModuleST (realBase, switchST, caseAstNode->next->next, depthSTPrint) ;
				}
				else
				{
					hasDefault = 1 ;
					if (searchedVar->datatype == TK_BOOLEAN)
					{
						printf ("ERROR : In \"%s\" at line %d, switch variable is boolean, default case is not allowed\n", getParentModuleName(realBase, baseModule), caseAstNode->tok->lineNumber) ;
						realBase->semanticError = 1 ;
					}

					fillModuleST (realBase, switchST, caseAstNode->next, depthSTPrint) ;
				}

				if (caseAstNode->id != TK_DEFAULT)
					caseAstNode = caseAstNode->next->next->next ;
				else
					caseAstNode = caseAstNode->next->next ;
			}

			//printf ("Done with loop\n") ;
			if (!hasDefault && searchedVar->datatype == TK_INTEGER)
			{
				printf ("ERROR : In \"%s\", default case expected in switch statement beginning at line %d\n", getParentModuleName(realBase, baseModule), statementAST->child->tok->lineNumber) ;
				realBase->semanticError = 1 ;
			}

			insertScopeST (baseModule , switchST) ;
			printModuleST (switchST, depthSTPrint) ;
		}
		statementAST = statementAST->next ;
	}


	/*
	if (!checkAllOutputTinkered (baseModule))
	{
		printf ("Hello\n") ;
	}
	*/

}



baseST * fillSymbolTable (astNode * thisASTNode , int depthSTPrint) {

	astNode * currentASTNode = thisASTNode ;

	
	printf( "> Calling createBaseST()\n") ;
	baseST * base = createBaseST () ;

	//****************************************************************
	//moduleDeclarations
	astNode * moduleDECS = currentASTNode->child ;
	currentASTNode = moduleDECS->child ;
	//inserting declare module id ;
	while ( currentASTNode ) {
		
		varST * searchResult ;
		if( searchResult =  searchVarInbaseST (base , currentASTNode->tok->lexeme ) ) {
			printf("ERROR : In line %d, \"%s\" module already declared.\n" , currentASTNode->tok->lineNumber, searchResult->lexeme ) ;
			base->semanticError = 1 ;
		}
		else {
			varST * tmp = createVarST ( currentASTNode , base , VAR_MODULE) ;
			insertVarSTInbaseST ( base , tmp ) ;
		}

		currentASTNode = currentASTNode->next ;
	}

	//***************************************************************
	/* handle it */
	astNode * otherMODS = moduleDECS->next ;
	moduleST *searchCond ;
	

	for (int otherMODS_Count = 1 ; otherMODS_Count <= 2 ; otherMODS_Count++)
	{
		currentASTNode = otherMODS->child ;
		//currentASTNode is now a modulef
		while ( currentASTNode ) {

			if (otherMODS_Count == 2 && searchVarInbaseST (base , currentASTNode->child->tok->lexeme) == NULL)
			{
				printf ("ERROR : In line %d, module \"%s\" definition appearing after driver program, but not declared!\n",currentASTNode->child->tok->lineNumber ,currentASTNode->child->tok->lexeme) ;
				base->semanticError = 1 ;
				currentASTNode->child->prev = currentASTNode->child ;
				currentASTNode = currentASTNode->next ;
				continue ;
			}

			searchCond = searchModuleInbaseST (base , currentASTNode->child->tok->lexeme) ;
			
			if (searchCond == NULL) {
				// need to create and insert
				moduleST * moduleToInsert = createModuleST (base , currentASTNode->child->tok->lexeme, 0) ;

				// filling input_plist and output_plist
				astNode * input_plistAST = currentASTNode->child->next ;
				astNode * iplAST = input_plistAST->child ;

				while ( iplAST ) {

					if ( searchVarInCurrentModule (moduleToInsert , iplAST->child->tok->lexeme) != NULL ) {
						printf ("ERROR : In definition of \"%s\" at line %d,  \"%s\" input variable already declared\n", moduleToInsert->lexeme,iplAST->child->tok->lineNumber, iplAST->child->tok->lexeme) ;
						base->semanticError = 1 ;
					}
					else{
						varST * tmp = createVarST (iplAST->child , moduleToInsert , VAR_INPUT ) ;
					
						if ( iplAST->child->next->dtTag == PRIMITIVE) 
						{
							tmp->datatype = iplAST->child->next->dt->pType ;

							// Setting and increasing offset
							tmp->offset = moduleToInsert->currOffset ;
							moduleToInsert->currOffset += getSize (base , tmp) ;

						}
						else{
							// array 
							tmp->datatype = TK_ARRAY ;
							tmp->arrayIndices = iplAST->child->next->dt->arrType ;

							int arrSize = getSize (base, tmp) ;		// Return will be positive or -1	

							if (arrSize > 0)
							{
								tmp->offset = moduleToInsert->currOffset ;
								moduleToInsert->currOffset += arrSize ;
							}
							else
								tmp->offset = arrSize ;
						}

						insertInputVarST (moduleToInsert , tmp) ;
					}
				
					iplAST = iplAST->next ;
				}
				astNode * retAST = input_plistAST->next ;
				astNode * oplAST = retAST->child ;

				while ( oplAST ) {

					if (searchOutputVarInCurrentModule(moduleToInsert , oplAST->child->tok->lexeme) != NULL || (searchInputVarInCurrentModule(moduleToInsert , oplAST->child->tok->lexeme)!=NULL && searchInputVarInCurrentModule(moduleToInsert , oplAST->child->tok->lexeme)->datatype != oplAST->child->next->id)) 
					{
						printf ("ERROR : In definition of \"%s\" at line %d,  \"%s\" variable already declared\n", moduleToInsert->lexeme,oplAST->child->tok->lineNumber, oplAST->child->tok->lexeme) ;
						base->semanticError = 1 ;

					}
					else{
						varST * tmp = createVarST ( oplAST->child , moduleToInsert, VAR_OUTPUT) ;
						//array can't be here
						tmp->datatype = oplAST->child->next->id ;
						insertOutputVarST ( moduleToInsert , tmp ) ;

						tmp->offset = moduleToInsert->currOffset ;
						moduleToInsert->currOffset += getSize (base, tmp) ;
					}
					
					oplAST = oplAST->next ;
				}

				insertModuleSTInbaseST ( base , moduleToInsert ) ;

			}
			else {
				// Only valid when otherMODS_Count = 1
				printf ( "ERROR : In line %d, module \"%s\" already defined\n", currentASTNode->child->tok->lineNumber, currentASTNode->child->tok->lexeme) ;
				base->semanticError = 1 ;
				currentASTNode->child->prev = currentASTNode->child ;		// Encoding to be removed later
				searchCond->tableType = MODULE_REDEC_ST ;
			}

			currentASTNode = currentASTNode->next ;
		}

		if (otherMODS->next != NULL)
			otherMODS = otherMODS->next->next ;
	}

	// ------------------------------------------------- fillModuleST --------------------------------------

	otherMODS = moduleDECS->next ;


	for (int otherMODS_Count = 1 ; otherMODS_Count <= 2 ; otherMODS_Count++)
	{
		currentASTNode = otherMODS->child ;

		//currentASTNode is now a module
		while (currentASTNode) {

				if (currentASTNode->child->prev == currentASTNode->child)		// For invalid nodes
				{
					currentASTNode->child->prev = NULL ;
					currentASTNode = currentASTNode->next ;
					continue ;
				}
				moduleST *moduleToInsert = searchModuleInbaseST (base , currentASTNode->child->tok->lexeme) ;
				if (moduleToInsert->tableType == MODULE_ST)
				{
					fillModuleST ( base , moduleToInsert , currentASTNode->child->next->next->next, depthSTPrint) ;
					printModuleST ( moduleToInsert, depthSTPrint ) ;	
				}
				currentASTNode = currentASTNode->next ;
			}

		if (otherMODS->next != NULL)
			otherMODS = otherMODS->next->next ;
	}

	//*****************************************************************


	astNode * driverMODS = otherMODS->prev ;
	moduleST * driverST = createDriverST(base) ;
	
	fillModuleST (base,driverST , driverMODS->child, depthSTPrint) ;
	printModuleST (driverST, depthSTPrint) ;
	insertDriverSTInbaseST ( base , driverST ) ;	
			
	return base ;
}
