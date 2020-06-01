#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "symbolTable.h"
#include "ast.h"
#include "typeChecker.h"
#include "error.h"

// make a global initial lexeme
char current_lexeme[20] = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0" ;
int realPresent = 0 ;
baseST *realBase ;

extern char* tokenIDToString (tokenID id) ;

int hashFunction ( char* lexeme , int size ) 
{
	if (lexeme == NULL)
		return 0 ;

	int sm = 0 ;
	int i = 0 ;

	while( lexeme[i] != '\0'){
		sm += lexeme[i] ;
		i++ ;
	}
	return sm%size ;
}


char * generateString () 
{
	int i=0 ; 

	while ( 1 ) {	
		if(current_lexeme[i] == '\0') 
		{
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


baseST * createBaseST () 
{
	baseST * tmp = ( baseST *) malloc ( sizeof(baseST)) ;
	tmp->driverST =  NULL ;
	tmp->semanticError = 0 ;
	
	for ( int i = 0 ; i < MODULE_BIN_COUNT ; i++ ) {
		tmp->modules[i] = NULL ;
	}
	for ( int i = 0 ; i < LOCAL_BIN_COUNT ; i++ ) {
		tmp->vars[i] = NULL ;
	}
	return tmp ;
}

/////////////////////////////////////////////////////////////////////////
moduleST * createModuleST (void *parent, char * lexeme, int currOffset) 
{
	moduleST * tmp = ( moduleST *) malloc ( sizeof(moduleST)) ;

	tmp->lexeme = lexeme ;
	tmp->tableType = MODULE_ST ;

	for ( int i = 0 ; i < LOCAL_BIN_COUNT ; i++ ) {
		tmp->localVars[i] = NULL ;
		
	}

	for (int i = 0 ; i < IO_BIN_COUNT ; i++) {
		tmp->inputVars[i] = NULL ;
		tmp->outputVars[i] = NULL ;
	}

	for ( int i = 0 ; i < MODULE_BIN_COUNT ; i++ ) {
		tmp->scopeVars[i] = NULL ;
	}

	tmp->parent = parent ;
	tmp->currOffset = currOffset ;
	tmp->filledMod = 0 ;
	tmp->outputSize = 0 ;
	tmp->inputSize = 0 ;
	tmp->scopeSize = 0 ;

	return tmp ;
}
moduleST * createDriverST () 
{
	moduleST * tmp = createModuleST ((void *)realBase, "Driver", 0 ) ;
	tmp->tableType = DRIVER_ST ;

	return tmp ;
}

moduleST * createScopeST (moduleST* parent , stType scopeType) 
{
	char * lexeme = (char*) malloc(sizeof(char)*20) ;
	strcpy ( lexeme , generateString () ) ;

	moduleST * tmp = createModuleST ((void *)parent, lexeme, parent->currOffset) ;
	tmp->tableType = scopeType ;

	return tmp ;
}

varST * createVarST (char *lexeme, void *scope, variableType varType, tokenID datatype) 
{
	varST * tmp = (varST *) malloc ( sizeof(varST)) ;

	tmp->lexeme = lexeme ;
	tmp->datatype = datatype ;
	tmp->offset = 0 ; //default value
	tmp->size = 0 ;
	tmp->tinker = 0 ;
	tmp->arrayIndices = NULL ;
	tmp->scope = scope ;
	tmp->varType = varType ;

	return tmp ;
}

/* ----------------------------------------------------------------------------------------------------------------------*/


void insertModuleSTInbaseST (moduleST * thisModule) 
{

	int index = hashFunction ( thisModule->lexeme , MODULE_BIN_COUNT ) ;

	moduleSTentry * tmp = ( moduleSTentry *) malloc ( sizeof( moduleSTentry)) ;
	tmp->thisModuleST = thisModule ;
	tmp->next = realBase->modules[index] ;
	realBase->modules[index] = tmp ;
}

void insertVarSTInbaseST (varST * thisVarST) 
{
	int index = hashFunction ( thisVarST->lexeme , LOCAL_BIN_COUNT ) ;

	varSTentry * tmp = ( varSTentry *) malloc (sizeof(varSTentry)) ;
	tmp->thisVarST = thisVarST ;
	tmp->next = realBase->vars[index] ;
	realBase->vars[index] = tmp ;
}


void insertScopeST ( moduleST* parent , moduleST * thisScopeST ) 
{
	int index = hashFunction ( thisScopeST->lexeme , MODULE_BIN_COUNT ) ;

	moduleSTentry * tmp = ( moduleSTentry * ) malloc ( sizeof ( moduleSTentry )) ;

	tmp->thisModuleST = thisScopeST ;
	tmp->next = (parent->scopeVars)[index] ;
	(parent->scopeVars)[index] = tmp ;
}

void insertVar (moduleST *thisModule, varST *thisVarST, insertVarType flag)
{
	int index, binCount ;
	varSTentry **list ;

	switch (flag)
	{
		case INSERT_INPUT : 
			list = thisModule->inputVars ;
			binCount = IO_BIN_COUNT ;
			break ;

		case INSERT_OUTPUT :
			list = thisModule->outputVars ;
			binCount = IO_BIN_COUNT ;
			break ;

		case INSERT_DYNAMIC :
			list = thisModule->dynamicVars ;
			binCount = DYNAMIC_BIN_COUNT ;
			break ;

		case INSERT_LOCAL :
			list = thisModule->localVars ;
			binCount = LOCAL_BIN_COUNT ;
			break ;
	}

	index = hashFunction (thisVarST->lexeme , binCount) ;

	varSTentry * tmp = ( varSTentry * ) malloc ( sizeof(varSTentry)) ;
	tmp->thisVarST = thisVarST ;
	tmp->next = list[index] ; 
	list[index] = tmp ;
}


/* ----------------------------------------------------------------------------------------------------------------------*/


varST * searchVarInbaseST (char * lexeme ) 
{
	int index = hashFunction ( lexeme , LOCAL_BIN_COUNT ) ;

	varSTentry * tmp = realBase->vars[index] ;
	while ( tmp!= NULL ){
		if( strcmp(tmp->thisVarST->lexeme , lexeme) == 0 )
			return tmp->thisVarST ;
		
		tmp = tmp->next ;
	}
	return NULL ;
}


moduleST * searchModuleInbaseST (char * lexeme) 
{
	int index = hashFunction ( lexeme , MODULE_BIN_COUNT ) ;

	moduleSTentry * tmp = realBase->modules[index] ;
	while ( tmp!= NULL ) {
		if ((tmp->thisModuleST->tableType == MODULE_ST /*||  tmp->thisModuleST->tableType == MODULE_REDEC_ST*/)&& strcmp ( tmp->thisModuleST->lexeme , lexeme) == 0 )
			return tmp->thisModuleST ;

		tmp = tmp->next ;
	}
	return NULL ;
}

varST* searchVarModuleList (moduleST* thisModule, char *lexeme, searchVarType flag)
{
	int index, binCount ;
	varSTentry **list ;

	switch (flag)
	{
		case SEARCH_INPUT : 
			list = thisModule->inputVars ;
			binCount = IO_BIN_COUNT ;
			break ;

		case SEARCH_OUTPUT :
			list = thisModule->outputVars ;
			binCount = IO_BIN_COUNT ;
			break ;

		case INSERT_LOCAL :
			list = thisModule->localVars ;
			binCount = LOCAL_BIN_COUNT ;
			break ;
	}

	index = hashFunction (lexeme , binCount) ;

	varSTentry * tmp = list[index] ;
	while (tmp != NULL)
	{
		if(strcmp(tmp->thisVarST->lexeme , lexeme) == 0)
			return tmp->thisVarST ;
		
		tmp = tmp->next ;
	}

	return NULL ;
}

varST * searchVarModule ( moduleST * thisModule , char * lexeme ) 
{
	varST * tmp ;
	if((tmp = searchVarModuleList (thisModule , lexeme, SEARCH_LOCAL)))
		return tmp ;
	else if ((tmp = searchVarModuleList (thisModule, lexeme, SEARCH_INPUT)))
		return tmp ;
	else 
		return searchVarModuleList (thisModule, lexeme, SEARCH_OUTPUT) ;
}


varST * searchVar (moduleST * thisModule , char * lexeme ) 
{	
	if (thisModule->parent == realBase)
		return searchVarModule (thisModule , lexeme) ;
	else
	{
		varST *tmp = searchVarModuleList (thisModule, lexeme, SEARCH_LOCAL) ;
		if (tmp == NULL)
			return searchVar (thisModule->parent, lexeme) ;
		else
			return tmp ;
	}
}

/* ----------------------------------------------------------------------------------------------------------------------*/

char* varTypeToString (variableType varType)
{
	switch (varType)
	{
		case VAR_PLACEHOLDER :
			return "placeholder" ;
		case VAR_INPUT :
			return "input" ;
		case VAR_OUTPUT :
			return "output" ;
		case VAR_LOOP :
			return "loop" ;
		case VAR_MODULE :
			return "Module" ;
		case VAR_LOCAL :
			return "local" ;
	}
}


void printBaseST () 
{
	
	printf ("************ printBaseST **************\n") ;

	if(realBase == NULL)
		return ;

	printf ("DriverST : %s\n" , realBase->driverST ? realBase->driverST->lexeme : "NULL" ) ;

	printf ("\nModules of baseST:\n") ;
	for ( int i=0 ; i<MODULE_BIN_COUNT ; i++ ) {
		moduleSTentry * tt = realBase->modules[i] ;
		while( tt ){
			printf ("\t%s" , tt->thisModuleST->lexeme ) ;
			tt = tt->next ;
		}
	}

	printf ("\nVariables of baseST:\n") ;
	for ( int i=0 ; i<LOCAL_BIN_COUNT ; i++ ) {
		varSTentry * tt = realBase->vars[i] ;
		while( tt ){
			printf ("\t%s" , tt->thisVarST->lexeme ) ;
			tt = tt->next ;
		}
	}


	printf ("\n**************************************\n") ;
}



void printModuleST ( moduleST * thisModuleST, int printParam ) 
{

	if (!printParam)
		return ;

	if( thisModuleST->tableType == DRIVER_ST || thisModuleST->tableType == MODULE_ST ){
		printf ("**************printModuleST( \"%s\" )***************\n",thisModuleST->lexeme ) ;
	}
	else {
		printf ("**************printModuleST(SCOPE : \"%s\" )***************\n",thisModuleST->lexeme ) ;
		//printf ("It's a ScopeST \n") ;
		printf ("Parent : %s\n" , thisModuleST->parent ? ((moduleST*)thisModuleST->parent)->lexeme : "NULL") ;
	}
	
	moduleSTentry * tt ;
	printf ("Modules :\n" ) ;
	for ( int i=0 ; i<MODULE_BIN_COUNT ; i++ )
	{
		
		tt = thisModuleST->scopeVars[i] ;
		while ( tt != NULL ) {
			printf ("\t%s --> Label\n",tt->thisModuleST->lexeme) ;
			tt = tt->next ;
		}
	}

	printf ("\nINPUT Variables :\n" ) ;
	for ( int i = 0 ; i<IO_BIN_COUNT ; i++ )
	{
		
		varSTentry * vv = thisModuleST->inputVars[i] ;

		while ( vv ) {
			printf ("\toffset: %d - \"%s\" ---- \"%s\" ",vv->thisVarST->offset,vv->thisVarST->lexeme , tokenIDToString(vv->thisVarST->datatype) ) ;
			if(vv->thisVarST->datatype == TK_ARRAY ) {
				printf ("%s(%s,%s)",tokenIDToString(vv->thisVarST->arrayIndices->type) ,vv->thisVarST->arrayIndices->tokLeft->lexeme , vv->thisVarST->arrayIndices->tokRight->lexeme ) ;
			}
			printf ("\n") ;


			vv = vv->next ;
		}
	}
	printf ("\nOUTPUT Variables :\n" ) ;
	for ( int i = 0 ; i<IO_BIN_COUNT ; i++ )
	{
		
		varSTentry * vv = thisModuleST->outputVars[i] ;

		while ( vv ) {
			printf ("\toffset: %d - \"%s\" ---- \"%s\" ",vv->thisVarST->offset,vv->thisVarST->lexeme , tokenIDToString(vv->thisVarST->datatype) ) ;
			if(vv->thisVarST->datatype == TK_ARRAY ) {
				printf ("%s(%s,%s)",tokenIDToString(vv->thisVarST->arrayIndices->type),vv->thisVarST->arrayIndices->tokLeft->lexeme , vv->thisVarST->arrayIndices->tokRight->lexeme ) ;
			}
			printf ("\n") ;


			vv = vv->next ;
		}
	}
	printf ("\nLOCAL Variables :\n" ) ;
	for ( int i = 0 ; i<LOCAL_BIN_COUNT ; i++ )
	{	
		varSTentry * vv = thisModuleST->localVars[i] ;

		while ( vv ) 
		{
			printf ("\toffset: %d - \"%s\" ---- \"%s\" ",vv->thisVarST->offset,vv->thisVarST->lexeme , tokenIDToString(vv->thisVarST->datatype) ) ;

			if(vv->thisVarST->datatype == TK_ARRAY ) 
			{
				printf ("%s(%s,%s)",tokenIDToString(vv->thisVarST->arrayIndices->type),vv->thisVarST->arrayIndices->tokLeft->lexeme , vv->thisVarST->arrayIndices->tokRight->lexeme ) ;
			}


			printf (" ---> %s" , varTypeToString (vv->thisVarST->varType)) ;

			printf ("\n") ;


			vv = vv->next ;
		}
	}
	
	printf ("\n**************************************\n") ;
}

varST * checkIP (moduleST * thisModule, moduleST * targetModule, astNode * inputNode)
{	
	varSTentry * varEntry = targetModule->inputVars[0] ;	
	astNode * inputIter = inputNode ;
	varST *searchedVar, *entryVar ;

	while(inputIter->next) 
		inputIter = inputIter->next ;

	while (varEntry != NULL && varEntry->thisVarST->varType == VAR_PLACEHOLDER)
		varEntry = varEntry->next ;

	int sameArgNoErr = 0 ;
	
	while(inputIter && varEntry)
	{
		searchedVar = searchVar (thisModule , inputIter->tok->lexeme ) ;
		entryVar = varEntry->thisVarST  ;

		if(searchedVar == NULL)
		{
			errSemantic () ;
			#ifdef _WIN64
				printf ("In module \"%s\" at line %d, variable \"%s\" is undeclared in the call to module \"%s\"\n", getParentModuleName (thisModule) ,inputIter->tok->lineNumber ,inputIter->tok->lexeme, thisModule->lexeme) ;
			#endif
			#if defined __linux__ || __MACH__
				printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " is undeclared in the call to module " ANSI_BOLD "%s" ANSI_RESET "\n",getParentModuleName (thisModule) ,inputIter->tok->lineNumber ,inputIter->tok->lexeme, thisModule->lexeme) ;
			#endif
			realBase->semanticError = 1 ;
			sameArgNoErr = 1 ;
		}
		else 
		{
			if(entryVar->datatype == searchedVar->datatype)
			{
				if (entryVar->datatype == TK_ARRAY) 
				{
					if (entryVar->arrayIndices->type != searchedVar->arrayIndices->type)
					{
						errSemantic () ;
						#ifdef _WIN64
							printf ("In module \"%s\" at line %d,  input array variables \"%s\" and \"%s\" have conflicting base types in the call to module \"%s\"\n",  getParentModuleName (thisModule) ,inputIter->tok->lineNumber , inputIter->tok->lexeme , entryVar->lexeme, targetModule->lexeme) ;
						#endif
						#if defined __linux__ || __MACH__
							printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK " input array " ANSI_RESET "variables " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " and " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " have conflicting base types in the call to module " ANSI_BOLD "%s" ANSI_RESET "\n" ,  getParentModuleName (thisModule) ,inputIter->tok->lineNumber , inputIter->tok->lexeme , entryVar->lexeme, targetModule->lexeme) ;
						#endif
						realBase->semanticError = 1 ;
					}

					if (isLeftLimStatic(entryVar) && isLeftLimStatic(searchedVar) && strcmp (entryVar->arrayIndices->tokLeft->lexeme, searchedVar->arrayIndices->tokLeft->lexeme))
					{
						errSemantic () ;
						#ifdef _WIN64
							printf ("In module \"%s\" at line %d, array variables \"%s\" and \"%s\" do not have matching left limits in the call to module \"%s\"\n", getParentModuleName (thisModule), inputIter->tok->lineNumber, entryVar->lexeme, searchedVar->lexeme, targetModule->lexeme) ;
						#endif
						#if defined __linux__ || __MACH__
							printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variables " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " and " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " do not have matching left limits in the call to module " ANSI_BOLD "%s" ANSI_RESET "\n", getParentModuleName (thisModule), inputIter->tok->lineNumber, entryVar->lexeme, searchedVar->lexeme, targetModule->lexeme) ;
						#endif
						realBase->semanticError = 1 ;
					}

					if (isRightLimStatic(entryVar) && isRightLimStatic(searchedVar) && strcmp (entryVar->arrayIndices->tokRight->lexeme, searchedVar->arrayIndices->tokRight->lexeme))
					{
						errSemantic () ;
						#ifdef _WIN64
							printf ("In module \"%s\" at line %d, array variables \"%s\" and \"%s\" do not have right limits in the call to module \"%s\"\n", getParentModuleName (thisModule), inputIter->tok->lineNumber, entryVar->lexeme, searchedVar->lexeme, targetModule->lexeme) ;
						#endif
						#if defined __linux__ || __MACH__
							printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variables " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " and " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " do not have right limits in the call to module " ANSI_BOLD "%s" ANSI_RESET "\n", getParentModuleName (thisModule), inputIter->tok->lineNumber, entryVar->lexeme, searchedVar->lexeme, targetModule->lexeme) ;
						#endif
						realBase->semanticError = 1 ;
					}
				}
			}
			else 
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, input variables \"%s\" and \"%s\" have conflicting types in the call to module \"%s\"\n",  getParentModuleName (thisModule) ,inputIter->tok->lineNumber , inputIter->tok->lexeme , entryVar->lexeme, targetModule->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "input" ANSI_RESET " variables " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " and " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " have conflicting types in the call to module " ANSI_BOLD "%s" ANSI_RESET "\n" ,  getParentModuleName (thisModule) ,inputIter->tok->lineNumber , inputIter->tok->lexeme , entryVar->lexeme, targetModule->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
				sameArgNoErr = 1 ;
			}
		}
		
		varEntry = varEntry->next ;
		while (varEntry != NULL && varEntry->thisVarST->varType == VAR_PLACEHOLDER)
			varEntry = varEntry->next ;

		inputIter = inputIter->prev ;
	}


	if (varEntry == NULL && inputIter)
	{
		errSemantic () ;
		#ifdef _WIN64
			printf ("In module \"%s\" at line %d, extra actual parameters passed in the call to module \"%s\"\n", getParentModuleName (thisModule), inputIter->tok->lineNumber, targetModule->lexeme) ;
		#endif
		#if defined __linux__ || __MACH__
			printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", extra actual parameters passed in the call to module " ANSI_BOLD "%s" ANSI_RESET "\n", getParentModuleName (thisModule), inputIter->tok->lineNumber, targetModule->lexeme) ;
		#endif
		realBase->semanticError = 1 ;
		return (varST *) malloc (sizeof(varST)) ;
	}
	else if (varEntry && inputIter == NULL) 
	{
		errSemantic () ;
		#ifdef _WIN64
			printf ("In module \"%s\" at line %d, insufficient number of actual parameters in the call to module \"%s\"\n", getParentModuleName (thisModule), inputNode->tok->lineNumber, targetModule->lexeme) ;
		#endif
		#if defined __linux__ || __MACH__
			printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", insufficient number of actual parameters in the call to module " ANSI_BOLD "%s" ANSI_RESET "\n", getParentModuleName (thisModule), inputNode->tok->lineNumber, targetModule->lexeme) ;
		#endif
		realBase->semanticError = 1 ;

		return (varST *) malloc (sizeof(varST)) ;
	}
	else
	{
		if (sameArgNoErr)
			return (varST *) 1 ;
		else
			return NULL ;
	}
}


varST * checkOP (moduleST * thisModule ,moduleST * targetModule , astNode * outputNode) 
{
	varST *searchedVar, *entryVar ;
	varSTentry *varEntry = targetModule->outputVars[0] ;
	int sameArgNoErr = 0 ;

	astNode * outputIter = outputNode ;
	while(outputIter->next)
		outputIter = outputIter->next ;
	
	while( outputIter && varEntry )
	{
		searchedVar = searchVar (thisModule , outputIter->tok->lexeme) ;
		entryVar = varEntry->thisVarST ;

		if(searchedVar == NULL)
		{
			errSemantic () ;
			#ifdef _WIN64
				printf ("In module \"%s\" at line %d, variable \"%s\" is undeclared in the call to module \"%s\"\n", getParentModuleName (thisModule) , outputIter->tok->lineNumber , outputIter->tok->lexeme, targetModule->lexeme ) ;
			#endif
			#if defined __linux__ || __MACH__
				printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " is undeclared in the call to module " ANSI_BOLD "%s" ANSI_RESET "\n", getParentModuleName (thisModule) , outputIter->tok->lineNumber , outputIter->tok->lexeme, targetModule->lexeme ) ;
			#endif
			realBase->semanticError = 1 ;
			sameArgNoErr = 1 ;
		}
		else 
		{
			if(entryVar->datatype != searchedVar->datatype)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, \"%s\" and \"%s\" have conflicting return types in the call to module \"%s\"\n", getParentModuleName (thisModule), outputIter->tok->lineNumber, outputIter->tok->lexeme , entryVar->lexeme, targetModule->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " and " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " have conflicting return types in the call to module " ANSI_BOLD "%s" ANSI_RESET "\n" , getParentModuleName (thisModule), outputIter->tok->lineNumber, outputIter->tok->lexeme , entryVar->lexeme, targetModule->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
				sameArgNoErr = 1 ;
			}
		}
		
		varEntry = varEntry->next ;
		outputIter = outputIter->prev ;
	}

	if (varEntry==NULL && outputIter)
	{
		errSemantic () ;
		#ifdef _WIN64
			printf ("In module \"%s\" at line %d, extra output variables in the call to module \"%s\"\n", getParentModuleName (thisModule), outputIter->tok->lineNumber, targetModule->lexeme) ;
		#endif
		#if defined __linux__ || __MACH__
			printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", extra output variables in the call to module " ANSI_BOLD "%s" ANSI_RESET "\n", getParentModuleName (thisModule), outputIter->tok->lineNumber, targetModule->lexeme) ;
		#endif
		realBase->semanticError = 1 ;
		return searchVar (thisModule , outputIter->tok->lexeme ) ;
	}
	else if (varEntry && outputIter==NULL) 
	{
		errSemantic () ;
		#ifdef _WIN64
			printf ("In module \"%s\" at line %d, insufficient number of outputs in the call to module \"%s\"\n", getParentModuleName (thisModule), outputNode->tok->lineNumber, targetModule->lexeme) ;
		#endif
		#if defined __linux__ || __MACH__
			printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", insufficient number of outputs in the call to module " ANSI_BOLD "%s" ANSI_RESET "\n", getParentModuleName (thisModule), outputNode->tok->lineNumber, targetModule->lexeme) ;
		#endif
		realBase->semanticError = 1 ;
		return entryVar ;
	}
	else{
		if (sameArgNoErr)
			return (varST *) 1 ;
		else
			return NULL ;
	}
}


int isValidCall (moduleST * thisModule , astNode * funcNode , int haveReturns) 
{
	if(haveReturns == 0) 
	{
		varST * varPtr = searchVarInbaseST(funcNode->tok->lexeme) ;
		moduleST * modPtr = searchModuleInbaseST (funcNode->tok->lexeme) ;

		if (modPtr != NULL)
		{
			if (varPtr == NULL && !modPtr->filledMod)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, module \"%s\" has not been declared before its call\n", getParentModuleName (thisModule), funcNode->tok->lineNumber, funcNode->tok->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", module " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " has not been declared before its call\n", getParentModuleName (thisModule), funcNode->tok->lineNumber, funcNode->tok->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}

			if (modPtr->outputVars[0] != NULL)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, call to function \"%s\" has return data but no receiving variables\n", getParentModuleName (thisModule), funcNode->tok->lineNumber, modPtr->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", call to function " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " has return data but no receiving variables\n", getParentModuleName (thisModule), funcNode->tok->lineNumber, modPtr->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}

			if (checkIP(thisModule, modPtr, funcNode->next->child) !=NULL)
				return -3 ;		// Errors printed in checkIP
			else
				return 1 ;
		}
		else
		{
			if (varPtr == NULL)
				return -1 ;
			else
				return -2 ;
		}
	}
	else
	{
		varST * varPtr = searchVarInbaseST(funcNode->next->next->tok->lexeme ) ;
		moduleST * modPtr = searchModuleInbaseST (funcNode->next->next->tok->lexeme) ;

		if (modPtr != NULL)
		{
			if (varPtr == NULL && !modPtr->filledMod)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, module \"%s\" has not been declared before its call\n", getParentModuleName (thisModule), funcNode->next->next->tok->lineNumber, funcNode->next->next->tok->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", module " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " has not been declared before its call\n", getParentModuleName (thisModule), funcNode->next->next->tok->lineNumber, funcNode->next->next->tok->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}

			if (modPtr->outputVars[0] == NULL)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, call to function \"%s\" has no return data but receiving variables are present\n", getParentModuleName (thisModule), funcNode->next->next->tok->lineNumber, modPtr->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", call to function " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " has no return data but receiving variables are present\n", getParentModuleName (thisModule), funcNode->next->next->tok->lineNumber, modPtr->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}
			
			if (checkIP(thisModule, modPtr, funcNode->next->next->next->child) !=NULL | checkOP(thisModule, modPtr, funcNode->child) !=NULL )
				return -3 ;		// Errors printed in checkIP and checkOP
			else
				return 1 ;
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

int getSize(varST * thisVar) 
{
	if(thisVar->datatype == TK_INTEGER)
		return 2 ;
	else if (thisVar->datatype == TK_BOOLEAN)
		return 2 ;
	else if(thisVar->datatype == TK_REAL)
	{
		realPresent = 1 ;
		return 4 ;
	}	
	else // Array type 
	{
		int left, right ;
		char *parentModule = getParentModuleName ((moduleST *)thisVar->scope) ;

		if (thisVar->arrayIndices->type == TK_REAL)
			realPresent = 1 ;

		if (isVarStaticArr(thisVar))
		{
			int sz ;

			left = atoi(thisVar->arrayIndices->tokLeft->lexeme) ;
			right = atoi(thisVar->arrayIndices->tokRight->lexeme) ;


			if (thisVar->arrayIndices->type == TK_INTEGER)
				sz = 2 ;
			else if (thisVar->arrayIndices->type == TK_BOOLEAN)
				sz = 2 ;
			else if (thisVar->arrayIndices->type == TK_REAL)
				sz = 4 ;
			
			
			sz *= (right - left + 1) ;

			if (sz <= 0)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, the declaration of array variable \"%s\" must have left limit <= right limit\n", parentModule, thisVar->arrayIndices->tokLeft->lineNumber, thisVar->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", the declaration of " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " must have left limit <= right limit\n", parentModule, thisVar->arrayIndices->tokLeft->lineNumber, thisVar->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
				return -1 ;
			}
			else
			{
				if (thisVar->varType != VAR_INPUT)
					return sz  ;
				else
					return 8 ;
			}
		}
		else if (thisVar->varType != VAR_INPUT)		//dynamic array but not input to a module
		{
			// dynamic array index checks
			int indexErrorFlag = 0 ;
			varST *searchedVarLeft, *searchedVarRight ;

			if (!isLeftLimStatic(thisVar))
			{
				searchedVarLeft = searchVar ((moduleST *)thisVar->scope, thisVar->arrayIndices->tokLeft->lexeme) ;
				if (searchedVarLeft == NULL)
				{
					errSemantic () ;
					#ifdef _WIN64
						printf ("In module \"%s\" at line %d, left index variable \"%s\" of array variable \"%s\" is undeclared\n", parentModule, thisVar->arrayIndices->tokLeft->lineNumber, thisVar->arrayIndices->tokLeft->lexeme, thisVar->lexeme) ;
					#endif
					#if defined __linux__ || __MACH__
						printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", left " ANSI_BOLD ANSI_BLACK "index" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " of " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " is undeclared\n", parentModule, thisVar->arrayIndices->tokLeft->lineNumber, thisVar->arrayIndices->tokLeft->lexeme, thisVar->lexeme) ;
					#endif
					realBase->semanticError = 1 ;
					indexErrorFlag = 1 ;
				}
				else if (searchedVarLeft->datatype != TK_INTEGER)
				{
					errSemantic () ;
					#ifdef _WIN64
						printf ("In module \"%s\" at line %d, left index variable \"%s\" of array variable \"%s\" must be of integer type\n", parentModule, thisVar->arrayIndices->tokLeft->lineNumber, thisVar->arrayIndices->tokLeft->lexeme, thisVar->lexeme) ;
					#endif
					#if defined __linux__ || __MACH__
						printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", left " ANSI_BOLD ANSI_BLACK "index" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " of " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " must be of " ANSI_BOLD ANSI_GREEN "integer" ANSI_RESET " type\n", parentModule, thisVar->arrayIndices->tokLeft->lineNumber, thisVar->arrayIndices->tokLeft->lexeme, thisVar->lexeme) ;
					#endif
					realBase->semanticError = 1 ;
					indexErrorFlag = 1 ;
				}
				else if (((moduleST*)thisVar->scope)->tableType == SWITCH_ST && thisVar->scope == searchedVarLeft->scope)
				{
					errSemantic () ;
					#ifdef _WIN64
						printf ("In module \"%s\" at line %d, declaration of left index variable \"%s\" of array variable \"%s\", and the array itself, appear in the same switch scope\n", parentModule, thisVar->arrayIndices->tokLeft->lineNumber, thisVar->arrayIndices->tokLeft->lexeme, thisVar->lexeme) ;
					#endif
					#if defined __linux__ || __MACH__
						printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", declaration of left " ANSI_BOLD ANSI_BLACK "index" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " of " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET ", and the array itself, appear in the same switch scope\n", parentModule, thisVar->arrayIndices->tokLeft->lineNumber, thisVar->arrayIndices->tokLeft->lexeme, thisVar->lexeme) ;
					#endif
					realBase->semanticError = 1 ;
					indexErrorFlag = 1 ;
				}
			}

			if (!isRightLimStatic (thisVar))
			{
				searchedVarRight = searchVar ((moduleST *)thisVar->scope, thisVar->arrayIndices->tokRight->lexeme) ;
				if (searchedVarRight == NULL)
				{
					errSemantic () ;
					#ifdef _WIN64
						printf ("In module \"%s\" at line %d, right index variable \"%s\" of array variable \"%s\" is undeclared\n", parentModule, thisVar->arrayIndices->tokRight->lineNumber ,thisVar->arrayIndices->tokRight->lexeme, thisVar->lexeme) ;
					#endif
					#if defined __linux__ || __MACH__
						printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", right " ANSI_BOLD ANSI_BLACK "index" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " of " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " is undeclared\n", parentModule, thisVar->arrayIndices->tokRight->lineNumber ,thisVar->arrayIndices->tokRight->lexeme, thisVar->lexeme) ;
					#endif
					realBase->semanticError = 1 ;
					indexErrorFlag = 1 ;
				}
				else if (searchedVarRight->datatype != TK_INTEGER)
				{
					errSemantic () ;
					#ifdef _WIN64
						printf ("In module \"%s\" at line %d, right index variable \"%s\" of array variable \"%s\" must be of integer type\n", parentModule, thisVar->arrayIndices->tokRight->lineNumber, thisVar->arrayIndices->tokRight->lexeme, thisVar->lexeme) ;
					#endif
					#if defined __linux__ || __MACH__
						printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", right " ANSI_BOLD ANSI_BLACK "index" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " of " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " must be of " ANSI_BOLD ANSI_GREEN "integer" ANSI_RESET " type\n", parentModule, thisVar->arrayIndices->tokRight->lineNumber, thisVar->arrayIndices->tokRight->lexeme, thisVar->lexeme) ;
					#endif
					realBase->semanticError = 1 ;
					indexErrorFlag = 1 ;
				}
				else if (((moduleST*)thisVar->scope)->tableType == SWITCH_ST && thisVar->scope == searchedVarRight->scope)
				{
					errSemantic () ;
					#ifdef _WIN64
						printf ("In module \"%s\" at line %d, declaration of right index variable \"%s\" of array variable \"%s\", and the array itself, appear in the same switch scope\n", parentModule, thisVar->arrayIndices->tokRight->lineNumber, thisVar->arrayIndices->tokRight->lexeme, thisVar->lexeme) ;
					#endif
					#if defined __linux__ || __MACH__
						printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", declaration of right " ANSI_BOLD ANSI_BLACK "index" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " of " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET ", and the array itself, appear in the same switch scope\n", parentModule, thisVar->arrayIndices->tokRight->lineNumber, thisVar->arrayIndices->tokRight->lexeme, thisVar->lexeme) ;
					#endif
					realBase->semanticError = 1 ;
					indexErrorFlag = 1 ;
				}
			}

			if (indexErrorFlag)
				return -2 ;		// incorect dynamic array

			insertVar ((moduleST *) thisVar->scope, thisVar, INSERT_DYNAMIC) ;
			return 12 ;		// correct dynamic array
		}
		else		// dynamic array and input to a module
		{
			int indexErrorFlag = 0 ;
			varST *searchedVarLeft, *searchedVarRight ;
			int leftDigit , rightDigit ;
			moduleST *scope ;

			leftDigit = isLeftLimStatic (thisVar);
			rightDigit = isRightLimStatic(thisVar) ;
			scope = (moduleST *)thisVar->scope ;

			// Left limit
			if (!leftDigit)
			{
				searchedVarLeft = searchVar (scope, thisVar->arrayIndices->tokLeft->lexeme) ;
				
				if (searchedVarLeft != NULL)
				{
					errSemantic () ;
					#ifdef _WIN64
						printf ("In module \"%s\" at line %d, index variable \"%s\" of input array variable \"%s\" is already defined\n", parentModule, thisVar->arrayIndices->tokLeft->lineNumber, thisVar->arrayIndices->tokLeft->lexeme, thisVar->lexeme) ;
					#endif
					#if defined __linux__ || __MACH__
						printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "index" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " of " ANSI_BOLD ANSI_BLACK "input array" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " is already defined\n", parentModule, thisVar->arrayIndices->tokLeft->lineNumber, thisVar->arrayIndices->tokLeft->lexeme, thisVar->lexeme) ;
					#endif
					realBase->semanticError = 1 ;
					indexErrorFlag = 1 ;
				}
				else
				{
					searchedVarLeft = createVarST (thisVar->arrayIndices->tokLeft->lexeme, scope, VAR_PLACEHOLDER, TK_INTEGER) ;
					searchedVarLeft->offset = -(scope->currOffset + 10) ;
					searchedVarLeft->size = 2 ;
					insertVar (scope, searchedVarLeft, INSERT_INPUT) ;
				}
			}

			// Right limit
			if (!rightDigit)
			{
				searchedVarRight = searchVar (scope, thisVar->arrayIndices->tokRight->lexeme) ;

				if (searchedVarRight != NULL)
				{
					errSemantic () ;
					#ifdef _WIN64
						printf ("In module \"%s\" at line %d, index variable \"%s\" of input array variable \"%s\" is already defined\n", parentModule, thisVar->arrayIndices->tokRight->lineNumber, thisVar->arrayIndices->tokRight->lexeme, thisVar->lexeme) ;
					#endif
					#if defined __linux__ || __MACH__
						printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "index" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " of " ANSI_BOLD ANSI_BLACK "input array" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " is already defined\n", parentModule, thisVar->arrayIndices->tokRight->lineNumber, thisVar->arrayIndices->tokRight->lexeme, thisVar->lexeme) ;
					#endif
					realBase->semanticError = 1 ;
					indexErrorFlag = 1 ;
				}
				else
				{
					searchedVarRight = createVarST (thisVar->arrayIndices->tokRight->lexeme, scope, VAR_PLACEHOLDER, TK_INTEGER) ;
					searchedVarRight->offset = -(scope->currOffset + 8) ;
					searchedVarRight->size = 2 ;
					insertVar (scope, searchedVarRight, INSERT_INPUT) ;
				}
			}

			if (indexErrorFlag)
				return -2 ;
			else
				return 12 ;
		}
	}
}

char* getParentModuleName (moduleST *scope)
{
	if (scope->parent == realBase)
		return scope->lexeme ;
	else
		return getParentModuleName (scope->parent) ;
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
		default :
			return "" ;
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


void tinkerVar (moduleST *baseModule, varST *var, astNode *varASTNode)
{
	var->tinker++ ;
	if (var->varType == VAR_LOOP)
	{
		errSemantic () ;
		#ifdef _WIN64
			printf ("In module \"%s\" at line %d, loop variable \"%s\" cannot be modified\n", getParentModuleName (baseModule), varASTNode->tok->lineNumber, varASTNode->tok->lexeme) ;
		#endif
		#if defined __linux__ || __MACH__
			printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "loop" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " cannot be modified\n", getParentModuleName (baseModule), varASTNode->tok->lineNumber, varASTNode->tok->lexeme) ;
		#endif
		realBase->semanticError = 1 ;
	}
	else if (var->varType == VAR_PLACEHOLDER)
	{
		errSemantic () ;
		#ifdef _WIN64
			printf ("In module \"%s\" at line %d, placeholder variable \"%s\" cannot be modified\n", getParentModuleName (baseModule), varASTNode->tok->lineNumber, varASTNode->tok->lexeme) ;
		#endif
		#if defined __linux__ || __MACH__
			printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "placeholder" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " cannot be modified\n", getParentModuleName (baseModule), varASTNode->tok->lineNumber, varASTNode->tok->lexeme) ;
		#endif
		realBase->semanticError = 1 ;
	}
}

void idListTinker (moduleST* baseModule, astNode *idListHead)
{
	varST *searchedVar ;
	while (idListHead)
	{
		searchedVar = searchVar (baseModule, idListHead->tok->lexeme) ;
		if (searchedVar != NULL)
			tinkerVar (baseModule, searchedVar, idListHead) ;

		idListHead = idListHead->next ;
	}
}

int checkAllOutputsTinkered (moduleST *baseModule)
{
	for (int i = 0 ; i<IO_BIN_COUNT ; i++ )
	{
		varSTentry * vv = baseModule->outputVars[i] ;
		while (vv) 
		{
			if (!vv->thisVarST->tinker)
				return 0 ;
			vv = vv->next ;
		}
	}

	return 1 ;
}

void printOutputsNotTinkered (moduleST *baseModule)
{
	for (int i = 0 ; i<IO_BIN_COUNT ; i++ )
	{
		varSTentry * vv = baseModule->outputVars[i] ;
		while (vv != NULL)		
		{	if (!vv->thisVarST->tinker)
			{
				#ifdef _WIN64
					printf ("%s", vv->thisVarST->lexeme) ;
				#endif
				#if defined __linux__ || defined __MACH__
					printf (ANSI_BOLD ANSI_RED "%s" ANSI_RESET, vv->thisVarST->lexeme) ;
				#endif
					
		 		if (vv->next != NULL && !vv->next->thisVarST->tinker)
					printf (", ") ;
			}
			vv = vv->next ;
		}
	}
}

void addTinkerList (guardTinkerNode *tinkerHead, int tinker)
{
	if (tinkerHead->tinker == -1)
		tinkerHead->tinker = tinker ;
	else
	{
		while (tinkerHead->next != NULL)
			tinkerHead = tinkerHead->next ;

		guardTinkerNode *tinkerNode = (guardTinkerNode *) malloc (sizeof(guardTinkerNode)) ;
		tinkerNode->tinker = tinker ;
		tinkerNode->next = NULL ;

		tinkerHead->next = tinkerNode ;
	}
}

void getExprVars (moduleST *baseModule, guardTinkerNode *tinkerHead, astNode *exprNode)
{
	if (exprNode == NULL)
		return ;

	if (exprNode->child == NULL || exprNode->child->next == NULL)	// array[index]
	{
		varST *searchedVar, *searchedVarIndex ;

		searchedVar = searchVar (baseModule, exprNode->tok->lexeme) ;
		if (searchedVar != NULL)
			addTinkerList (tinkerHead, searchedVar->tinker) ;

		if (exprNode->child != NULL && exprNode->child->id == TK_ID)
		{
			searchedVarIndex = searchVar (baseModule, exprNode->child->tok->lexeme) ;
			if (searchedVarIndex != NULL)
				addTinkerList (tinkerHead, searchedVarIndex->tinker) ;
		}
	}
	else
	{
		getExprVars (baseModule, tinkerHead, exprNode->child) ;
		if (exprNode->child != NULL)
			getExprVars (baseModule, tinkerHead, exprNode->child->next) ;
	}
}

guardTinkerNode* getGuardTinkerList (moduleST *baseModule, astNode *exprNode)
{
	guardTinkerNode *tinkerHead = (guardTinkerNode *) malloc (sizeof(guardTinkerNode)) ;
	tinkerHead->tinker = -1 ;
	tinkerHead->next = NULL ;

	getExprVars (baseModule, tinkerHead, exprNode) ;
	return tinkerHead ;
}

int hasTinkerListChanged (guardTinkerNode *tinkerHeadBefore, guardTinkerNode *tinkerHeadAfter)
{
	while (tinkerHeadBefore && tinkerHeadAfter)
	{
		if (tinkerHeadAfter->tinker > tinkerHeadBefore->tinker)
			return 1 ;

		tinkerHeadAfter = tinkerHeadAfter->next ;
		tinkerHeadBefore = tinkerHeadBefore->next ;
	}

	return 0 ;
}

void fillModuleST (moduleST* baseModule , astNode * statementsAST , int depthSTPrint) 
{
	int retSize = 0 ;
	varST *searchedVar ;
	statementsAST->localST = baseModule ;
	astNode * statementAST = statementsAST->child ;

	while (statementAST) 
	{
		if(statementAST->child->id == TK_DECLARE) 
		{
			// declare statement
			astNode * idAST = statementAST->child->next->child ;
			astNode * dataTypeAST = statementAST->child->next->next ;

			while (idAST) 
			{
				searchedVar = searchVar (baseModule, idAST->tok->lexeme) ;

				if (searchedVar != NULL && searchedVar->scope == baseModule && searchedVar->varType != VAR_INPUT)
				{
					errSemantic () ;
					#ifdef _WIN64
						printf ("In module \"%s\" at line %d, %s variable \"%s\" already declared\n", getParentModuleName (baseModule) ,idAST->tok->lineNumber, varTypeToString(searchedVar->varType) , idAST->tok->lexeme) ;
					#endif
					#if defined __linux__ || __MACH__
						printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "%s" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " already declared\n", getParentModuleName (baseModule) ,idAST->tok->lineNumber, varTypeToString(searchedVar->varType) , idAST->tok->lexeme) ;
					#endif
					realBase->semanticError = 1 ;
				}
				else		// if input variable, or a local variable in some parent scope, or not found at all
				{
					varST * tmp = createVarST (idAST->tok->lexeme , baseModule , VAR_LOCAL, -9999) ;

					if (dataTypeAST->dtTag == ARRAY)
					{
						tmp->datatype = TK_ARRAY ;
						tmp->arrayIndices = dataTypeAST->dt->arrType ; 
					}
					else 
						tmp->datatype = dataTypeAST->dt->pType ;
					

					// filling offset
					retSize = getSize(tmp) ;

					if (retSize > 0)
					{
						tmp->offset = baseModule->currOffset + retSize ;
						tmp->size = retSize ;
						baseModule->currOffset += retSize ;
					}
					else	// correct dynamic array, or invalid static/dynamic array
						tmp->offset = retSize ;

					insertVar (baseModule , tmp, INSERT_LOCAL) ;
				}

				idAST = idAST->next ;
			}

		}
		else if ( statementAST->child->id == TK_WHILE) 
		{
			guardTinkerNode *tinkerHeadBefore, *tinkerHeadAfter ;

			if (getExpressionType (baseModule, statementAST->child->next) != TK_BOOLEAN)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, the guard condition must be of type boolean\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", the guard condition must be of type " ANSI_BOLD ANSI_GREEN "boolean" ANSI_RESET "\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber) ;
				#endif
				realBase->semanticError = 1 ;
			}

			tinkerHeadBefore = getGuardTinkerList (baseModule, statementAST->child->next) ;
			moduleST * tmp = createScopeST (baseModule, WHILE_ST) ;
			fillModuleST (tmp , statementAST->child->next->next, depthSTPrint) ;
			tinkerHeadAfter = getGuardTinkerList (baseModule, statementAST->child->next) ;
			
			if (tinkerHeadBefore->tinker == -1)
			{
				errWarning () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, guard condition of while loop is static\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", guard condition of while loop is static\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber) ;
				#endif
			}
			else if (!hasTinkerListChanged (tinkerHeadBefore, tinkerHeadAfter))
			{
				errWarning () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, none of the identifiers of the guard condition are being changed in the while loop body\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", none of the identifiers of the guard condition are being changed in the while loop body\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber) ;
				#endif
			}

			printModuleST (tmp,  depthSTPrint) ;
			insertScopeST ( baseModule , tmp ) ;
		}
		else if ( statementAST->child->id == TK_FOR) 
		{
			searchedVar = searchVar (baseModule, statementAST->child->next->tok->lexeme) ;
			if (searchedVar == NULL)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, loop variable \"%s\" is undeclared\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber, statementAST->child->next->tok->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", loop variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " is undeclared\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber, statementAST->child->next->tok->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}
			else if (searchedVar->datatype != TK_INTEGER)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, loop variable \"%s\" needs to be of integer type\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber, statementAST->child->next->tok->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", loop variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " needs to be of " ANSI_BOLD ANSI_GREEN "integer" ANSI_RESET " type\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber, statementAST->child->next->tok->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}

			astNode *loopLim; 
			loopLim = statementAST->child->next->next ;

			if (atoi(loopLim->tok->lexeme) > atoi(loopLim->next->tok->lexeme))
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, left loop limit must be <= right loop limit\n", getParentModuleName (baseModule), loopLim->tok->lineNumber) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", left loop limit must be <= right loop limit\n", getParentModuleName (baseModule), loopLim->tok->lineNumber) ;
				#endif
				realBase->semanticError = 1 ;
			}

			moduleST *forScope = createScopeST (baseModule, FOR_ST) ;
			varST *loopVar = createVarST (statementAST->child->next->tok->lexeme, forScope, VAR_LOOP, TK_INTEGER) ;

			if (searchedVar == NULL)
				loopVar->offset = -2 ;		// VAR_LOOP with offset of -2 is undeclared loop counter
			else
			{
				loopVar->offset = searchedVar->offset ;
				loopVar->size = searchedVar->size ;
			}

			insertVar (forScope, loopVar, INSERT_LOCAL) ;
			fillModuleST (forScope, loopLim->next->next, depthSTPrint) ;

			printModuleST (forScope, depthSTPrint) ;
			insertScopeST (baseModule , forScope) ;
		}
 		else if (statementAST->child->id == TK_GET_VALUE) 
 		{
			searchedVar = searchVar (baseModule , statementAST->child->next->tok->lexeme) ;

			if (searchedVar == NULL)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, variable \"%s\" is undeclared\n" ,  getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber , statementAST->child->next->tok->lexeme ) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " is undeclared\n" ,  getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber , statementAST->child->next->tok->lexeme ) ;
				#endif
				realBase->semanticError = 1 ;
			}
			else
				tinkerVar (baseModule, searchedVar, statementAST->child->next) ;

		}
 		else if ( statementAST->child->id == TK_PRINT)
 		{
 			if (statementAST->child->next->id == TK_ID)
 				validateVar (baseModule, statementAST->child->next, &searchedVar) ;
 			else if (statementAST->child->next->id == TK_RNUM)
 				realPresent = 1 ;
 		} 
		else if ( statementAST->child->id == TK_ASSIGNOP) 
			assignmentTypeCheck (baseModule, statementAST->child) ;
 		else if (statementAST->child->id == idList) 
 		{
			// [a] = use module with parameters [ d ] ;
			int validCallFlag = isValidCall (baseModule, statementAST->child, 1) ;

			if (validCallFlag == -1 )	
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, \"%s\" Module neither declared nor defined \n", getParentModuleName (baseModule), statementAST->child->next->next->tok->lineNumber, statementAST->child->next->next->tok->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " Module neither declared nor defined \n", getParentModuleName (baseModule), statementAST->child->next->next->tok->lineNumber, statementAST->child->next->next->tok->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}
			else if( validCallFlag == -2 ) 
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, \"%s\" declared but not defined \n", getParentModuleName (baseModule), statementAST->child->next->next->tok->lineNumber, statementAST->child->next->next->tok->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " declared but not defined \n", getParentModuleName (baseModule), statementAST->child->next->next->tok->lineNumber, statementAST->child->next->next->tok->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}
			// else error messages is printed in internal functions

			idListTinker (baseModule, statementAST->child->child) ;
		}
		else if (statementAST->child->id == TK_ID) 
		{
			// use module with parameters ... (no return)
			int validCallFlag = isValidCall (baseModule, statementAST->child, 0) ;
			if (validCallFlag == -1 )	
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, \"%s\" Module neither declared nor defined \n", getParentModuleName (baseModule), statementAST->child->tok->lineNumber , statementAST->child->tok->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " Module neither declared nor defined \n", getParentModuleName (baseModule), statementAST->child->tok->lineNumber , statementAST->child->tok->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}
			else if( validCallFlag == -2 ) 
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, \"%s\" Module declared but not defined \n", getParentModuleName (baseModule),statementAST->child->tok->lineNumber , statementAST->child->tok->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " Module declared but not defined \n", getParentModuleName (baseModule),statementAST->child->tok->lineNumber , statementAST->child->tok->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}
		}
 		else if (statementAST->child->id == TK_SWITCH)
		{
			int switchTypeError = 0 ;

			searchedVar = searchVar (baseModule , statementAST->child->next->tok->lexeme) ;
			if (searchedVar == NULL)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, switch variable \"%s\" is undeclared\n",  getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber ,  statementAST->child->next->tok->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "switch" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " is undeclared\n",  getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber ,  statementAST->child->next->tok->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}
			else
			{
				if (!(searchedVar->datatype == TK_INTEGER || searchedVar->datatype == TK_BOOLEAN))
				{
					errSemantic () ;
					#ifdef _WIN64
						printf ("In module \"%s\" at line %d, switch variable \"%s\" must be integer or boolean\n",  getParentModuleName (baseModule),statementAST->child->next->tok->lineNumber ,  statementAST->child->next->tok->lexeme) ;
					#endif
					#if defined __linux__ || __MACH__
						printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "switch" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " must be " ANSI_BOLD ANSI_GREEN "integer" ANSI_RESET " or " ANSI_BOLD ANSI_GREEN "boolean" ANSI_RESET "\n",  getParentModuleName (baseModule),statementAST->child->next->tok->lineNumber ,  statementAST->child->next->tok->lexeme) ;
					#endif
					realBase->semanticError = 1 ;
					switchTypeError = 1 ;
				}
			}

			moduleST *switchST = createScopeST (baseModule, SWITCH_ST) ;
			astNode *caseAstNode = statementAST->child->next->next ;

			int hasDefault = 0 ;
			int hasTrue = 0, hasFalse = 0 ;
			while (caseAstNode != NULL)
			{
				// case value mismatch
				if (searchedVar != NULL && !switchTypeError && caseAstNode->id != TK_DEFAULT)
				{
					if ((searchedVar->datatype == TK_INTEGER && (caseAstNode->next->id == TK_TRUE || caseAstNode->next->id == TK_FALSE)) || (searchedVar->datatype == TK_BOOLEAN && caseAstNode->next->id == TK_NUM)) 
					{
						errSemantic () ;
						#ifdef _WIN64
							printf ("In module \"%s\" at line %d, switch variable has type \"%s\" and case value has type \"%s\"\n", getParentModuleName (baseModule), caseAstNode->next->tok->lineNumber, typeIDToString (searchedVar->datatype) , (caseAstNode->next->id == TK_NUM)?"integer":"boolean") ;
						#endif
						#if defined __linux__ || __MACH__
							printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "switch" ANSI_RESET " variable has type " ANSI_BOLD ANSI_GREEN "%s" ANSI_RESET " and case value has type " ANSI_BOLD ANSI_GREEN "%s" ANSI_RESET "\n", getParentModuleName (baseModule), caseAstNode->next->tok->lineNumber, typeIDToString (searchedVar->datatype) , (caseAstNode->next->id == TK_NUM)?"integer":"boolean") ;
						#endif
						realBase->semanticError = 1 ;
					}
				}
				
				if (caseAstNode->id != TK_DEFAULT)
				{
					if (caseValRepeat (caseAstNode))
					{
						errSemantic () ;
						#ifdef _WIN64
							printf ("In module \"%s\" at line %d, case value is repeating\n", getParentModuleName (baseModule), caseAstNode->next->tok->lineNumber) ;
						#endif
						#if defined __linux__ || __MACH__
							printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", case value is repeating\n", getParentModuleName (baseModule), caseAstNode->next->tok->lineNumber) ;
						#endif
						realBase->semanticError = 1 ;
					}

					if (searchedVar != NULL && searchedVar->datatype == TK_BOOLEAN && caseAstNode->next->tok->lexeme[0] == 't')
						hasTrue = 1 ;
					else if (searchedVar != NULL && searchedVar->datatype == TK_BOOLEAN && caseAstNode->next->tok->lexeme[0] == 'f')
						hasFalse = 1 ;

					fillModuleST (switchST, caseAstNode->next->next, depthSTPrint) ;
				}
				else
				{
					hasDefault = 1 ;
					if (searchedVar != NULL && searchedVar->datatype == TK_BOOLEAN)
					{
						errSemantic () ;
						#ifdef _WIN64
							printf ("In module \"%s\" at line %d, switch variable has type \"boolean\", default case is not allowed\n", getParentModuleName (baseModule), caseAstNode->tok->lineNumber) ;
						#endif
						#if defined __linux__ || __MACH__
							printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "switch" ANSI_RESET " variable has type " ANSI_BOLD ANSI_GREEN "boolean" ANSI_RESET ", default case is not allowed\n", getParentModuleName (baseModule), caseAstNode->tok->lineNumber) ;
						#endif
						realBase->semanticError = 1 ;
					}

					fillModuleST (switchST, caseAstNode->next, depthSTPrint) ;
				}

				if (caseAstNode->id != TK_DEFAULT)
					caseAstNode = caseAstNode->next->next->next ;
				else
					caseAstNode = caseAstNode->next->next ;
			}

			if (searchedVar != NULL && !hasDefault && searchedVar->datatype == TK_INTEGER)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, default case expected in switch statement\n",  getParentModuleName (baseModule), statementAST->child->tok->lineNumber) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", default case expected in switch statement\n",  getParentModuleName (baseModule), statementAST->child->tok->lineNumber) ;
				#endif
				realBase->semanticError = 1 ;
			}

			if (searchedVar != NULL && searchedVar->datatype == TK_BOOLEAN && (!hasTrue || !hasFalse))
			{
				if (!hasTrue)
				{
					errSemantic () ;
					#ifdef _WIN64
						printf ("In module \"%s\" at line %d, boolean switch statement is missing 'true' case\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber) ;
					#endif
					#if defined __linux__ || __MACH__
						printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_GREEN "boolean" ANSI_RESET " switch statement is missing 'true' case\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber) ;
					#endif
					realBase->semanticError = 1 ;
				}

				if (!hasFalse)
				{
					errSemantic () ;
					#ifdef _WIN64
						printf ("In module \"%s\" at line %d, boolean switch statement is missing 'false' case\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber) ;
					#endif
					#if defined __linux__ || __MACH__
						printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_GREEN "boolean" ANSI_RESET " switch statement is missing 'false' case\n", getParentModuleName (baseModule), statementAST->child->next->tok->lineNumber) ;
					#endif
					realBase->semanticError = 1 ;
				}
			}

			insertScopeST (baseModule , switchST) ;
			printModuleST (switchST, depthSTPrint) ;
		}
		statementAST = statementAST->next ;
	}

	if (baseModule->parent != realBase)
		baseModule->scopeSize = baseModule->currOffset - ((moduleST *)baseModule->parent)->currOffset ;
	else
		baseModule->scopeSize = baseModule->currOffset ;
}

baseST * fillSymbolTable (astNode * thisASTNode , int depthSTPrint) 
{
	astNode * currentASTNode = thisASTNode ;
	realBase = createBaseST () ;

	//****************************************************************
	astNode * moduleDECS = currentASTNode->child ;
	currentASTNode = moduleDECS->child ;
	while (currentASTNode) 
	{
		varST * searchResult ;
		if((searchResult =  searchVarInbaseST (currentASTNode->tok->lexeme))) 
		{
			errSemantic () ;
			#ifdef _WIN64
				printf ("In line %d, \"%s\" module already declared.\n" , currentASTNode->tok->lineNumber, searchResult->lexeme ) ;
			#endif
			#if defined __linux__ || __MACH__
				printf ("In line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " module already declared.\n" , currentASTNode->tok->lineNumber, searchResult->lexeme ) ;
			#endif
			realBase->semanticError = 1 ;
		}
		else {
			varST * tmp = createVarST (currentASTNode->tok->lexeme , realBase , VAR_MODULE, TK_ID) ;
			insertVarSTInbaseST (tmp) ;
		}

		currentASTNode = currentASTNode->next ;
	}

	//***************************************************************
	astNode * otherMODS = moduleDECS->next ;
	moduleST *searchCond ;
	
	for (int otherMODS_Count = 1 ; otherMODS_Count <= 2 ; otherMODS_Count++)
	{
		currentASTNode = otherMODS->child ;
		//currentASTNode is now a modulef
		while (currentASTNode) 
		{
			searchCond = searchModuleInbaseST (currentASTNode->child->tok->lexeme) ;
			
			if (searchCond == NULL) {
				// need to create and insert
				moduleST * moduleToInsert = createModuleST ((void *)realBase, currentASTNode->child->tok->lexeme, 16) ;

				// filling input_plist and output_plist
				astNode * input_plistAST = currentASTNode->child->next ;
				astNode * iplAST = input_plistAST->child ;

				while (iplAST) 
				{

					if (searchVarModule (moduleToInsert , iplAST->child->tok->lexeme) != NULL ) 
					{
						errSemantic () ;
						#ifdef _WIN64
							printf ("In the signature of \"%s\" at line %d, input variable \"%s\" already declared\n", moduleToInsert->lexeme,iplAST->child->tok->lineNumber, iplAST->child->tok->lexeme) ;
						#endif
						#if defined __linux__ || __MACH__
							printf ("In the signature of " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "input" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " already declared\n", moduleToInsert->lexeme,iplAST->child->tok->lineNumber, iplAST->child->tok->lexeme) ;
						#endif
						realBase->semanticError = 1 ;
					}
					else{
						varST * tmp = createVarST (iplAST->child->tok->lexeme, moduleToInsert , VAR_INPUT, -9999) ;
						int retSize ;
					
						if ( iplAST->child->next->dtTag == PRIMITIVE) 
						{
							tmp->datatype = iplAST->child->next->dt->pType ;

							// Setting and increasing offset
							retSize = getSize (tmp) ;
							tmp->offset = -moduleToInsert->currOffset ;
							moduleToInsert->currOffset += retSize ;
							moduleToInsert->inputSize += retSize ;
						}
						else{
							// array 
							tmp->datatype = TK_ARRAY ;
							tmp->arrayIndices = iplAST->child->next->dt->arrType ;

							retSize = getSize (tmp) ;		// Return will be positive or -1	

							if (retSize > 0)
							{
								tmp->offset = -moduleToInsert->currOffset ;
								tmp->size = retSize ;
								moduleToInsert->currOffset += retSize ;
								moduleToInsert->inputSize += retSize ;
							}
							else
								tmp->offset = retSize ;
						}

						insertVar (moduleToInsert , tmp, INSERT_INPUT) ;
					}
				
					iplAST = iplAST->next ;
				}

				astNode * retAST = input_plistAST->next ;
				astNode * oplAST = retAST->child ;
				

				// Handle exact offsets later

				while (oplAST) 
				{
					// Input and output parameters CANNOT share identifiers
					varST *searchedInput, *searchedOutput ;
					searchedInput = searchVarModuleList (moduleToInsert , oplAST->child->tok->lexeme, SEARCH_INPUT) ;
					searchedOutput = searchVarModuleList (moduleToInsert , oplAST->child->tok->lexeme, SEARCH_OUTPUT) ;

					if (searchedInput != NULL) 
					{
						errSemantic () ;
						#ifdef _WIN64
							printf ("In the signature of \"%s\" at line %d, input  variable \"%s\" is already declared\n", moduleToInsert->lexeme,oplAST->child->tok->lineNumber, oplAST->child->tok->lexeme) ;
						#endif
						#if defined __linux__ || __MACH__
							printf ("In the signature of " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "input " ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " is already declared\n", moduleToInsert->lexeme,oplAST->child->tok->lineNumber, oplAST->child->tok->lexeme) ;
						#endif
						realBase->semanticError = 1 ;
					}
					else if (searchedOutput != NULL)
					{
						errSemantic () ;
						#ifdef _WIN64
							printf ("In the signature of \"%s\" at line %d, output variable \"%s\" is already declared\n", moduleToInsert->lexeme,oplAST->child->tok->lineNumber, oplAST->child->tok->lexeme) ;
						#endif
						#if defined __linux__ || __MACH__
							printf ("In the signature of " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "output" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " is already declared\n", moduleToInsert->lexeme,oplAST->child->tok->lineNumber, oplAST->child->tok->lexeme) ;
						#endif
						realBase->semanticError = 1 ;
					}
					else
					{
						varST * tmp = createVarST ( oplAST->child->tok->lexeme , moduleToInsert, VAR_OUTPUT, oplAST->child->next->id) ;
						int retSize = getSize (tmp) ;

						tmp->offset = -moduleToInsert->currOffset ;
						tmp->size = retSize ;
						moduleToInsert->currOffset += retSize ;
						moduleToInsert->outputSize += retSize ;
						insertVar (moduleToInsert, tmp, INSERT_OUTPUT) ;
					}
					
					oplAST = oplAST->next ;
				}

				moduleToInsert->currOffset = 0 ;	//resetting for input values
				insertModuleSTInbaseST (moduleToInsert) ;

			}
			else {
				// Only valid when otherMODS_Count = 1
				errSemantic () ;
				#ifdef _WIN64
					printf ("In line %d, module \"%s\" already defined\n", currentASTNode->child->tok->lineNumber, currentASTNode->child->tok->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", module " ANSI_BOLD "%s" ANSI_RESET " already defined\n", currentASTNode->child->tok->lineNumber, currentASTNode->child->tok->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
				currentASTNode->child->prev = currentASTNode->child ;		// Encoding to be removed later
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
		while (currentASTNode)
		{
			if (currentASTNode->child->prev == currentASTNode->child)		// For invalid nodes
			{
				currentASTNode->child->prev = NULL ;
				currentASTNode = currentASTNode->next ;
				continue ;
			}
			
			moduleST *moduleToInsert = searchModuleInbaseST (currentASTNode->child->tok->lexeme) ;
			fillModuleST (moduleToInsert , currentASTNode->child->next->next->next, depthSTPrint) ;

			if (!checkAllOutputsTinkered(moduleToInsert))
			{
				errWarning () ;
				#ifdef _WIN64
					printf ("In the signature of \"%s\", the following output variables are not assigned - ", moduleToInsert->lexeme) ;
				#endif
				#if defined __linux__ || __MACH__
					printf ("In the signature of " ANSI_BOLD "%s" ANSI_RESET ", the following " ANSI_BOLD ANSI_BLACK "output" ANSI_RESET " variables are not assigned - ", moduleToInsert->lexeme) ;
				#endif
				printOutputsNotTinkered (moduleToInsert) ;
				printf ("\n") ;
			}

			moduleToInsert->filledMod = 1 ;
			printModuleST (moduleToInsert, depthSTPrint) ;

			currentASTNode = currentASTNode->next ;
		}

		if (otherMODS->next != NULL)
		{
			astNode * driverMODS = otherMODS->next ;
			moduleST * driverST = createDriverST() ;
			realBase->driverST = driverST ;
			
			fillModuleST (driverST , driverMODS->child, depthSTPrint) ;
			printModuleST (driverST, depthSTPrint) ;
			
			otherMODS = otherMODS->next->next ;
		}
	}

	//*****************************************************************
			
	return realBase ;
}
