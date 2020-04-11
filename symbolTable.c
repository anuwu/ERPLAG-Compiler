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

	return tmp ;
}
moduleST * createDriverST ( baseST * parent ) {
	moduleST * tmp = createModuleST ( parent , "|DRIVER|", 0 ) ;
	tmp->tableType = DRIVER_ST ;

	return tmp ;
}
moduleST * createScopeST ( moduleST * parent ) {
	char * lexeme = (char*) malloc(sizeof(char)*20) ;
	strcpy ( lexeme , generateString () ) ;

	moduleST * tmp = createModuleST ( (baseST*)parent , lexeme, parent->currOffset ) ;
	tmp->tableType = SCOPE_ST ;

	return tmp ;
}

varST * createVarST ( astNode * thisASTnode, void *scope, variableType varType ) {
	varST * tmp = (varST *) malloc ( sizeof(varST)) ;

	tmp->lexeme = thisASTnode->tok->lexeme ;
	tmp->datatype = thisASTnode->tok->id ;
	tmp->offset = -9999 ; //default value
	tmp->arrayIndices = NULL ;
	tmp->scope = scope ;
	tmp->varType = varType ;

	return tmp ;
}
///////////////////////////////////////////////////////////////////////////
baseST * insertModuleSTInbaseST ( baseST * base , moduleST * thisModule) {

	int index = hashFunction ( thisModule->lexeme , MODULE_BIN_COUNT ) ;

	moduleSTentry * tmp = ( moduleSTentry *) malloc ( sizeof( moduleSTentry)) ;
	tmp->thisModuleST = thisModule ;
	tmp->next = base->modules[index] ;
	base->modules[index] = tmp ;

	return base ;
}

baseST * insertVarSTInbaseST ( baseST * base , varST * thisVarST ) {
	int index = hashFunction ( thisVarST->lexeme , VAR_BIN_COUNT ) ;

	varSTentry * tmp = ( varSTentry *) malloc (sizeof(varSTentry)) ;
	tmp->thisVarST = thisVarST ;
	tmp->next = base->vars[index] ;
	base->vars[index] = tmp ;

	return base ;
}
baseST * insertDriverSTInbaseST ( baseST * base , moduleST * thisDriverModule ) {
	base->driverST = thisDriverModule ;
	return base ;
}

///////////////////////////////////////////////////////////////////////////
moduleST * insertScopeST ( moduleST* parent , moduleST * thisScopeST ) {
	int index = hashFunction ( thisScopeST->lexeme , MODULE_BIN_COUNT ) ;

	moduleSTentry * tmp = ( moduleSTentry * ) malloc ( sizeof ( moduleSTentry )) ;

	tmp->thisModuleST = thisScopeST ;
	tmp->next = (parent->scopeVars)[index] ;
	(parent->scopeVars)[index] = tmp ;

	return parent ;
}


moduleST * insertLocalVarST ( moduleST* thisModule , varST* thisVarST ) {
	int index = hashFunction ( thisVarST->lexeme , VAR_BIN_COUNT ) ;

	varSTentry * tmp = ( varSTentry * ) malloc ( sizeof(varSTentry)) ;
	tmp->thisVarST = thisVarST ;
	tmp->next = thisModule->localVars[index] ; 
	thisModule->localVars[index] = tmp ;
	
	return thisModule ;
}
moduleST * insertInputVarST ( moduleST* thisModule , varST* thisVarST ) {
	int index = hashFunction ( thisVarST->lexeme , IO_BIN_COUNT ) ;

	varSTentry * tmp = ( varSTentry * ) malloc ( sizeof(varSTentry)) ;
	tmp->thisVarST = thisVarST ;
	tmp->next = thisModule->inputVars[index] ; 
	thisModule->inputVars[index] = tmp ;
	
	return thisModule ;
}
moduleST * insertOutputVarST ( moduleST* thisModule , varST* thisVarST ) {
	int index = hashFunction ( thisVarST->lexeme , IO_BIN_COUNT ) ;

	varSTentry * tmp = ( varSTentry * ) malloc ( sizeof(varSTentry)) ;
	tmp->thisVarST = thisVarST ;
	tmp->next = thisModule->outputVars[index] ; 
	thisModule->outputVars[index] = tmp ;
	
	return thisModule ;
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

	/*
	Do the recursive call later
	moduleST * tmpModule = thisModule ;
	while( tmpModule->tableType == SCOPE_ST ) { //means above is a moduleST
		tmp = searchVarInCurrentModule( (moduleST *) thisModule->parent , lexeme ) ;
		
		if ( tmp )
			return tmp ;

		tmpModule = (moduleST * ) tmpModule->parent ;
	}	
	// return searchVarInbaseST ( (baseST *) thisModule->parent , lexeme ) ;
	return NULL ;
	*/
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



void printModuleST ( moduleST * thisModuleST ) {

	//printf("**************printModuleST( %s )***************\n",thisModuleST->lexeme ) ;

	if( thisModuleST->tableType == DRIVER_ST || thisModuleST->tableType == MODULE_ST ){
		printf("**************printModuleST( %s )***************\n",thisModuleST->lexeme ) ;
		//printf("It's a Driver or Module ST \n") ;
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
				printf ( "%s(%s,%s)",tokenIDToString(vv->thisVarST->arrayIndices->dataType) ,vv->thisVarST->arrayIndices->startingPos , vv->thisVarST->arrayIndices->endingPos ) ;
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
				printf ( "%s(%s,%s)",tokenIDToString(vv->thisVarST->arrayIndices->dataType),vv->thisVarST->arrayIndices->startingPos , vv->thisVarST->arrayIndices->endingPos ) ;
			}
			printf ("\n") ;


			vv = vv->next ;
		}
	}
	printf("\nLOCAL Variables :\n" ) ;
	for ( int i = 0 ; i<VAR_BIN_COUNT ; i++ ){
		
		varSTentry * vv = thisModuleST->localVars[i] ;

		while ( vv ) {
			printf ("\toffset: %d - %s ---- %s ",vv->thisVarST->offset,vv->thisVarST->lexeme , tokenIDToString(vv->thisVarST->datatype) ) ;
			if(vv->thisVarST->datatype == TK_ARRAY ) {
				printf ( "%s(%s,%s)",tokenIDToString(vv->thisVarST->arrayIndices->dataType),vv->thisVarST->arrayIndices->startingPos , vv->thisVarST->arrayIndices->endingPos ) ;
			}
			printf ("\n") ;


			vv = vv->next ;
		}
	}
	
	printf("\n**************************************\n") ;
}




varST * checkIP (baseST *realBase, moduleST * thisModule ,moduleST * targetModule , astNode * inputNode ) {
	
	printf("> checkIP\n") ;

	varSTentry * varEntry = targetModule->inputVars[0] ;

	
	astNode * inputIter = inputNode ;
	while(inputIter->next) {
		inputIter = inputIter->next ;
	}
	

	while( inputIter && varEntry ){

		printf("---| %s & %s |---\n",inputIter->tok->lexeme , varEntry->thisVarST->lexeme ) ;
		
		varST * searchedVar = searchVar(realBase, thisModule , inputIter->tok->lexeme ) ;


		if( searchedVar == NULL ){
			printf("ERROR : %s variable not declared\n", inputIter->tok->lexeme ) ;
		}
		else {
			if(varEntry->thisVarST->datatype == searchedVar->datatype )
				printf("> GOOD : %s with %s\n",inputNode->tok->lexeme , varEntry->thisVarST->lexeme ) ;
			else 
				printf ( "ERROR : %s and %s conflicting dataType\n" , inputIter->tok->lexeme , varEntry->thisVarST->lexeme) ;
		}
		
		varEntry = varEntry->next ;
		inputIter = inputIter->prev ;
	}

	if ( varEntry==NULL && inputIter ){
		printf("ERROR : More parameters passed\n") ;
		return searchVar(realBase, thisModule , inputIter->tok->lexeme ) ;
	}
	else if ( varEntry && inputIter==NULL ) {
		printf("ERROR : Insufficient Number of parameters\n") ;
		return varEntry->thisVarST ;
	}
	else{
		printf("> All Good\n") ;
		return NULL ;
	}
	
}


varST * checkOP (baseST *realBase, moduleST * thisModule ,moduleST * targetModule , astNode * outputNode ) {
	
	printf("> checkOP\n") ;

	varSTentry * varEntry = targetModule->outputVars[0] ;

	
	astNode * outputIter = outputNode ;
	while(outputIter->next) {
		outputIter = outputIter->next ;
	}
	
	while( outputIter && varEntry ){

		printf("---| %s & %s |---\n",outputIter->tok->lexeme , varEntry->thisVarST->lexeme ) ;
		varST * searchedVar = searchVar(realBase, thisModule , outputIter->tok->lexeme ) ;

		if( searchedVar == NULL )
			printf("ERROR : %s variable not declared\n", outputIter->tok->lexeme ) ;
		else {
			if(varEntry->thisVarST->datatype == searchedVar->datatype )
				printf("> GOOD : %s with %s\n",outputIter->tok->lexeme , varEntry->thisVarST->lexeme ) ;
			else 
				printf ( "ERROR : %s and %s conflicting dataType\n" , outputIter->tok->lexeme , varEntry->thisVarST->lexeme) ;
		}
		
		varEntry = varEntry->next ;
		outputIter = outputIter->prev ;
	}

	if ( varEntry==NULL && outputIter ){
		printf("ERROR : More parameters passed\n") ;
		return searchVar(realBase, thisModule , outputIter->tok->lexeme ) ;
	}
	else if ( varEntry && outputIter==NULL ) {
		printf("ERROR : Insufficient Number of parameters\n") ;
		return varEntry->thisVarST ;
	}
	else{
		printf("> All Good\n") ;
		return NULL ;
	}
	
}



int isValidCall ( baseST * base, moduleST * thisModule , astNode * funcNode , int haveReturns ) {

	printf ("\t> isValidCall\n") ;

	if(haveReturns == 0 ) {
		varST * varPtr = searchVarInbaseST(base , funcNode->tok->lexeme ) ;
		moduleST * modPtr = searchModuleInbaseST ( base , funcNode->tok->lexeme) ;

		if (modPtr != NULL)
		{
			if (modPtr->tableType == MODULE_REDEC_ST)
			{
				printf ("ERROR : Module %s found, but with clashing definition\n" , modPtr->lexeme) ;
				return -3 ;
			}
			else
				printf("\t> Definition found\n") ;

			if (checkIP(base, thisModule,modPtr,funcNode->next->child) !=NULL) {
				return -3 ;
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
			if (modPtr->tableType == MODULE_REDEC_ST)
			{
				printf ("ERROR : Module %s found, but with clashing definition\n" , modPtr->lexeme) ;
				return -3 ;
			}
			else
				printf("\t> Definition found\n") ;
			
			if ( checkIP(base, thisModule,modPtr,funcNode->next->next->next->child) !=NULL || checkOP(base, thisModule,modPtr,funcNode->child) !=NULL ) {
				return -3 ;
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

	if(thisVar->datatype== TK_INTEGER )
		return 2 ;
	else if (thisVar->datatype == TK_BOOLEAN )
		return 1 ;
	else if(thisVar->datatype == TK_REAL )
		return 4 ;
	else if (thisVar->datatype == TK_ARRAY ) 
	{
		int left, right, indexErrorFlag = 0 ;
		char *parentModule = getParentModule (realBase , (moduleST *)thisVar->scope) ;

		if (isdigit (thisVar->arrayIndices->startingPos[0]) && isdigit (thisVar->arrayIndices->endingPos[0]))
		{
			int sz ;

			left = atoi(thisVar->arrayIndices->startingPos) ;
			right = atoi(thisVar->arrayIndices->endingPos) ;


			if (thisVar->arrayIndices->dataType == TK_INTEGER)
				sz = 2 ;
			else if (thisVar->arrayIndices->dataType == TK_BOOLEAN)
				sz = 1 ;
			else if (thisVar->arrayIndices->dataType == TK_REAL)
				sz = 4 ;

			sz *= (right - left + 1) ;

			if (sz < 0)
			{
				printf ("ERROR : In %s, Left index of \"%s\" must be <= right index", parentModule, thisVar->lexeme) ;
				return -1 ;
			}
			else
				return sz  ;
		}
		else
		{
			// dynamic array index checks
			if (!isdigit (thisVar->arrayIndices->startingPos[0]) && !isdigit (thisVar->arrayIndices->endingPos[0]))
			{
				if (searchVar (realBase, (moduleST *)thisVar->scope, thisVar->arrayIndices->startingPos) == NULL)
				{
					printf ("ERROR : In %s, Left index \"%s\" of local variable \"%s\" is undeclared\n", parentModule, thisVar->arrayIndices->startingPos, thisVar->lexeme) ;
					indexErrorFlag = 1 ;
				}
				if (searchVar (realBase, (moduleST *)thisVar->scope, thisVar->arrayIndices->endingPos) == NULL)
				{
					printf ("ERROR : In %s, Right index \"%s\" of local variable \"%s\" is undeclared\n", parentModule, thisVar->arrayIndices->endingPos, thisVar->lexeme) ;
					indexErrorFlag = 1 ;
				}

				if (indexErrorFlag)
					return -1 ;
			}
			else if (!isdigit (thisVar->arrayIndices->startingPos[0]) && isdigit (thisVar->arrayIndices->endingPos[0]))
			{
				// Left dynamic, right static
				if (searchVar (realBase, (moduleST *)thisVar->scope, thisVar->arrayIndices->startingPos) == NULL)
				{
					printf ("ERROR : In %s, Left index \"%s\" of local variable \"%s\" is undeclared\n", parentModule, thisVar->arrayIndices->startingPos, thisVar->lexeme) ;
					return -1 ;
				}
			}
			else
			{
				// left static, right dynamic
				if (searchVar (realBase, (moduleST *)thisVar->scope, thisVar->arrayIndices->endingPos) == NULL)
				{
					printf ("ERROR : In %s, Left index \"%s\" of local variable \"%s\" is undeclared\n", parentModule, thisVar->arrayIndices->endingPos, thisVar->lexeme) ;
					return -1 ;
				}
			}

			return 0 ;	// correct dynamic array
		}
	}
	else {
		return -1 ;		// Error code
	}
}

char *getParentModule (baseST* realBase, moduleST *scope)
{
	if (scope->parent == realBase)
		return scope->lexeme ;
	else
		return getParentModule (realBase, scope->parent) ;
}


moduleST * fillModuleST ( baseST* realBase , moduleST* baseModule , astNode * statementAST ) 
{
	int getOffsetSize = 0 ;
	//printf ("INSIDE FILL MODULE!\n") ;

	while ( statementAST ) {
		if(statementAST->child->id == TK_DECLARE ) {
			// declare statement
			//printf ("Inside DECLARE statement\n") ;
			astNode * idAST = statementAST->child->next->child ;
			astNode * dataTypeAST = statementAST->child->next->next ;
			int i = 1 ;
			while ( idAST ) 
			{

				varST *searchedVar ;
				//printf ("%d. Before searching VAR!\n", i) ;
				searchedVar = searchVar (realBase, baseModule, idAST->tok->lexeme) ;
				//printf ("%d. After searching VAR!\n", i) ;

				if (searchedVar == NULL)
					;

				if (searchedVar != NULL && searchedVar->varType == VAR_OUTPUT)
				{
					printf ( "ERROR in %s : %s Output variable already exists\n", getParentModule (realBase, baseModule) ,idAST->tok->lexeme) ;
				}
				else if (searchedVar != NULL && searchedVar->scope == baseModule && (searchedVar->varType == VAR_LOCAL || searchedVar->varType == VAR_LOOP))
				{
					if (searchedVar->varType == VAR_LOCAL)
						printf ( "ERROR in %s : %s Local variable already declared\n", getParentModule (realBase, baseModule) ,idAST->tok->lexeme) ;
					else
						printf ( "ERROR in %s : %s Loop variable already declared\n", getParentModule (realBase, baseModule) ,idAST->tok->lexeme) ;
				}
				else		// if input variable, or a local variable in some parent scope
				{
					//printf ("TO BE LOCAL\n");
					varST * tmp = createVarST ( idAST , baseModule , VAR_LOCAL) ;

					if (dataTypeAST->dtTag  == ARRAY ){
						tmp->datatype = TK_ARRAY ;

						tmp->arrayIndices = (arrayInST *) malloc(sizeof(arrayInST)) ;
						tmp->arrayIndices->startingPos = dataTypeAST->dt->arrType->lex1 ;
						tmp->arrayIndices->endingPos = dataTypeAST->dt->arrType->lex2 ;
						tmp->arrayIndices->dataType = dataTypeAST->dt->arrType->type ;

					}
					else {
						tmp->datatype = dataTypeAST->dt->pType ;
					}

					// filling offset
					getOffsetSize = getSize(realBase, tmp) ;

					if (getOffsetSize == 0)
						tmp->offset = -1 ;		// Offset of -1 will denote a dynamic array
					else if (getOffsetSize > 0)
					{
						tmp->offset = baseModule->currOffset ;
						baseModule->currOffset += getOffsetSize ;
					}
					else
					{
						// This will probably never come here.
						printf ("ERROR : getSize() method\n") ;
						tmp->offset = -2 ;		// invalid dynamic array declaration
					}
					baseModule = insertLocalVarST(baseModule , tmp) ;
				}

				idAST = idAST->next ;
				i++ ;
			}

			//printf ("Done with idLIST\n") ;
		}
		else if ( statementAST->child->id == TK_WHILE ) {
			// while statement
			printf ("AT WHILE STATEMENT\n") ;
			moduleST * tmp = createScopeST ( baseModule ) ;

			tmp = fillModuleST ( realBase , tmp , statementAST->child->next->next->child ) ;

			printModuleST ( tmp ) ;

			tmp = insertScopeST ( baseModule , tmp ) ;

		}
		else if ( statementAST->child->id == TK_FOR ) {
			//for stmt
			moduleST * tmp = createScopeST ( baseModule ) ;

			varST * iterat = createVarST ( statementAST->child->next , tmp , VAR_LOOP) ;
			iterat->datatype = TK_INTEGER ;
			
			tmp = insertLocalVarST ( tmp , iterat ) ;

			tmp = fillModuleST ( realBase , tmp , statementAST->child->next->next->next->next->child ) ;

			printModuleST ( tmp ) ;

			tmp = insertScopeST ( baseModule , tmp ) ;
		}
		else if ( statementAST->child->id == TK_GET_VALUE ) {
			varST * thisVar = searchVar(realBase, baseModule , statementAST->child->next->tok->lexeme ) ;

			if ( thisVar == NULL )
				printf ( "ERROR : %s Variable undeclared\n" , statementAST->child->next->tok->lexeme ) ;
		}
		else if ( statementAST->child->id == TK_PRINT ) {
			varST * thisVar = searchVar(realBase, baseModule , statementAST->child->next->tok->lexeme ) ;

			if ( thisVar == NULL ){
				printf ( "ERROR : %s undeclared\n" , statementAST->child->next->tok->lexeme ) ;
			}
			else {
				// generate code
			}
		}
		else if ( statementAST->child->id == /*TK_ID*/ TK_ASSIGNOP) {
			// Insert new code here - Anwesh.
			// 1. Tinkering code. Assume that variable existence checking has already been done. That is if A is an output varialble, and A[i] := <expression> is the statement, it would have been detected before.


			if (searchVar(realBase, baseModule, statementAST->child->child->tok->lexeme) == NULL)
				printf (" ERROR : %s LHS variable undeclared\n", statementAST->child->child->tok->lexeme) ;


			// RHS business will occur here.
		}
		else if ( statementAST->child->id == idList) {
			// [a] = use module with parameters [ d ] ;

			if (strcmp(baseModule->lexeme, statementAST->child->next->next->tok->lexeme) == 0)
				printf ("Error : %s recursion is not allowed!\n", baseModule->lexeme) ;
			else
			{
				int validCallFlag = isValidCall ( realBase , baseModule,statementAST->child , 1 ) ;
				if ( validCallFlag == 1) {
					// valid call code
					printf ("%s calling %s ALL GOOD\n", baseModule->lexeme ,statementAST->child->next->next->tok->lexeme) ;
				}
				else if (validCallFlag == -1 )	{
					printf( "ERROR %s : %s Module neither declared nor defined \n", baseModule->lexeme, statementAST->child->next->next->tok->lexeme) ;
				}
				else if( validCallFlag == -2 ) {
					printf( "ERROR %s : %s Module declared but not defined \n", baseModule->lexeme, statementAST->child->next->next->tok->lexeme) ;
				}
			}
		}
		else if (statementAST->child->id == TK_ID) {
			// use module with parameters ... (no return)
			int validCallFlag = isValidCall ( realBase , baseModule,statementAST->child , 0 ) ;
			if ( validCallFlag == 1 ) {
				printf ("%s calling %s ALL GOOD\n", baseModule->lexeme ,statementAST->child->tok->lexeme) ;
			}
			else if (validCallFlag == -1 )	{
				printf( "ERROR %s : %s Module neither declared nor defined \n", baseModule->lexeme, statementAST->child->tok->lexeme) ;
			}
			else if( validCallFlag == -2 ) {
				printf( "ERROR %s : %s Module declared but not defined \n", baseModule->lexeme, statementAST->child->tok->lexeme) ;
			}
		}

		statementAST = statementAST->next ;
	}
	
	return baseModule ;
}



baseST * fillSymbolTable (astNode * thisASTNode ) {

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
			printf("ERROR : %s Module Already Declared\n" , searchResult->lexeme ) ;
		}
		else {
			varST * tmp = createVarST ( currentASTNode , base , VAR_MODULE) ;
			base = insertVarSTInbaseST ( base , tmp ) ;
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
				printf ("ERROR : Module %s definition appearing in latter half, but not declared!\n", currentASTNode->child->tok->lexeme) ;
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
						printf ("ERROR : Module %s variable already declared\n",iplAST->child->tok->lexeme) ;
					}
					else{
						varST * tmp = createVarST ( iplAST->child , moduleToInsert , VAR_INPUT ) ;
					
						if ( iplAST->child->next->dtTag == PRIMITIVE) {
							tmp->datatype = iplAST->child->next->dt->pType ;

							// Setting and increasing offset
							tmp->offset = moduleToInsert->currOffset ;
							moduleToInsert->currOffset += getSize (base , tmp) ;

						}
						else{
							// array 
							tmp->datatype = TK_ARRAY ;
							tmp->arrayIndices = (arrayInST* ) malloc ( sizeof(arrayInST)) ;
							tmp->arrayIndices->startingPos = iplAST->child->next->dt->arrType->lex1 ;
							tmp->arrayIndices->endingPos = iplAST->child->next->dt->arrType->lex2 ;
							tmp->arrayIndices->dataType = iplAST->child->next->dt->arrType->type ;

							int arrSize = getSize (base, tmp) ;
							if (arrSize <= 0)
								printf ("ERROR : Left index needs to be <= right index\n") ;
							
							tmp->offset = moduleToInsert->currOffset ;
							moduleToInsert->currOffset += arrSize ;
						}

						moduleToInsert = insertInputVarST ( moduleToInsert , tmp ) ;
					}
				
					iplAST = iplAST->next ;
				}
				astNode * retAST = input_plistAST->next ;
				astNode * oplAST = retAST->child ;

				while ( oplAST ) {

					if ( searchOutputVarInCurrentModule(moduleToInsert , oplAST->child->tok->lexeme) != NULL || (searchInputVarInCurrentModule(moduleToInsert , oplAST->child->tok->lexeme)!=NULL && searchInputVarInCurrentModule(moduleToInsert , oplAST->child->tok->lexeme)->datatype != oplAST->child->next->id )) {
						printf ("ERROR : %s variable already declared\n",oplAST->child->tok->lexeme) ;
					}
					else{
						varST * tmp = createVarST ( oplAST->child , moduleToInsert, VAR_OUTPUT) ;
						//array can't be here
						tmp->datatype = oplAST->child->next->id ;
						moduleToInsert = insertOutputVarST ( moduleToInsert , tmp ) ;

						tmp->offset = moduleToInsert->currOffset ;
						moduleToInsert->currOffset += getSize (base, tmp) ;
					}
					
					oplAST = oplAST->next ;
				}

				base = insertModuleSTInbaseST ( base , moduleToInsert ) ;

			}
			else {
				// Only valid when otherMODS_Count = 1
				printf ( "ERROR : %s already defined %d\n",currentASTNode->child->tok->lexeme, otherMODS_Count) ;
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


	//printf ("Before fillMOduleST\n") ;

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
					moduleToInsert = fillModuleST ( base , moduleToInsert , currentASTNode->child->next->next->next->child ) ;
					printModuleST ( moduleToInsert ) ;	
				}


				currentASTNode = currentASTNode->next ;
			}

		if (otherMODS->next != NULL)
		{
			//printf ("OM1 done\n") ;
			otherMODS = otherMODS->next->next ;
		}
	}

	//*****************************************************************

	astNode * driverMODS = otherMODS->prev ;
	moduleST * driverST = createDriverST(base) ;
	
	driverST = fillModuleST (base,driverST , driverMODS->child->child) ;
	printModuleST ( driverST ) ;
	base = insertDriverSTInbaseST ( base , driverST ) ;	
			
	return base ;
}
