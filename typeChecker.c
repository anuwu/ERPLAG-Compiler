#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "symbolTable.h"
#include "typeChecker.h"

int isVarStaticArr (varST *arrayVar)
{
	return (arrayVar->offset >= 0 || arrayVar->offset == -2) ;
}

int validVarIndex (baseST *realBase, moduleST *baseModule, astNode *varIndexASTNode)
{
	// Assumes varIndexASTNode isn't NULL

	if (varIndexASTNode->id == TK_NUM)
		return TK_NUM ;

	varST *indexVar = searchVar (realBase, baseModule, varIndexASTNode->tok->lexeme) ;
	if (indexVar == NULL)
	{
		realBase->semanticError = 1 ;
		printf ("ERROR : In \"%s\" at line %d, index variable \"%s\" is undeclared\n", getParentModuleName(realBase, baseModule), varIndexASTNode->tok->lineNumber, varIndexASTNode->tok->lexeme) ;
		return 0 ;
	}
	else if (indexVar->datatype != TK_INTEGER)
	{
		realBase->semanticError = 1 ;
		printf ("ERROR : In \"%s\" at line %d, index variable \"%s\" needs to be of type integer\n", getParentModuleName(realBase, baseModule), varIndexASTNode->tok->lineNumber, varIndexASTNode->tok->lexeme) ;
		return 0 ;
	}

	return TK_ID ;
}

/*
searchVar ()
	--> NULL 											==> ERROR
	--> NOT NULLValidVar
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
		realBase->semanticError = 1 ;
		printf ("ERROR : In \"%s\" at line %d, specified index %s is out of bounds for static array \"%s\" with limits [%d .. %d]\n", getParentModuleName(realBase, baseModule), indASTNode->tok->lineNumber, indASTNode->tok->lexeme, arrVar->lexeme, leftLim, rightLim) ;
		return 0 ;
	}

	return 1 ;
}


int validateVar (baseST *realBase , moduleST *baseModule , astNode *varASTNode, varST *searchedVar)
{
	int isValidVar = 1 , isValidVarIndex ;
	searchedVar = searchVar (realBase, baseModule, varASTNode->tok->lexeme) ;

	if (searchedVar == NULL)
	{
		printf ("ERROR : In \"%s\" at line %d, variable \"%s\" is undeclared\n", getParentModuleName(realBase, baseModule), varASTNode->tok->lineNumber, varASTNode->tok->lexeme) ;

		realBase->semanticError = 1 ;
		if (varASTNode->child != NULL)
			validVarIndex (realBase, baseModule, varASTNode->child) ;

		return 0 ;
	}
	else if (searchedVar->datatype != TK_ARRAY)
	{
		if (varASTNode->child != NULL)
		{
			isValidVar = 0 ;
			realBase->semanticError = 1 ;
			printf ("ERROR : In \"%s\" at line %d, primitive %s variable \"%s\" cannot be indexed\n", getParentModuleName(realBase, baseModule), varASTNode->child->tok->lineNumber, typeIDToString (searchedVar->datatype), searchedVar->lexeme) ;
			validVarIndex (realBase, baseModule, varASTNode->child) ;
		}

		return searchedVar->datatype ;
	}
	else if (varASTNode->child != NULL)		// It is array and has an index
	{
		isValidVarIndex = validVarIndex(realBase, baseModule, varASTNode->child) ;

		if (isVarStaticArr (searchedVar) && isValidVarIndex == TK_NUM)		// Static array
		{
			if (!validStaticArrStaticIndex (realBase, baseModule, searchedVar, varASTNode->child))
				realBase->semanticError = 1 ;
		}
		else if (!isValidVarIndex)
			realBase->semanticError = 1 ;

		return searchedVar->arrayIndices->type ;
	}
	else
		return TK_ARRAY ;

	return isValidVar ;
}