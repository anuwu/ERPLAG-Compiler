#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "symbolTable.h"
#include "typeChecker.h"

extern char* tokenIDToString (tokenID id) ;

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


tokenID validateVar (baseST *realBase , moduleST *baseModule , astNode *varASTNode, varST **searchedVar)
{
	int isValidVarIndex ;
	*searchedVar = searchVar (realBase, baseModule, varASTNode->tok->lexeme) ;

	if (*searchedVar == NULL)
	{
		printf ("ERROR : In \"%s\" at line %d, variable \"%s\" is undeclared\n", getParentModuleName(realBase, baseModule), varASTNode->tok->lineNumber, varASTNode->tok->lexeme) ;

		realBase->semanticError = 1 ;
		if (varASTNode->child != NULL)
			validVarIndex (realBase, baseModule, varASTNode->child) ;

		return 0 ;
	}
	else if ((*searchedVar)->datatype != TK_ARRAY)
	{
		if (varASTNode->child != NULL)
		{
			realBase->semanticError = 1 ;
			printf ("ERROR : In \"%s\" at line %d, primitive %s variable \"%s\" cannot be indexed\n", getParentModuleName(realBase, baseModule), varASTNode->child->tok->lineNumber, typeIDToString ((*searchedVar)->datatype), (*searchedVar)->lexeme) ;
			validVarIndex (realBase, baseModule, varASTNode->child) ;
		}

		return (*searchedVar)->datatype ;
	}
	else if (varASTNode->child != NULL)		// It is array and has an index
	{
		isValidVarIndex = validVarIndex(realBase, baseModule, varASTNode->child) ;

		if (isVarStaticArr (*searchedVar) && isValidVarIndex == TK_NUM)		// Static array
		{
			if (!validStaticArrStaticIndex (realBase, baseModule, *searchedVar, varASTNode->child))
				realBase->semanticError = 1 ;
		}
		else if (!isValidVarIndex)
			realBase->semanticError = 1 ;

		return (*searchedVar)->arrayIndices->type ;
	}
	else
	{
		//printf ("hain?\n") ;
		return TK_ARRAY ;
	}
}

void expressionTypeCheck (baseST *realBase, moduleST *baseModule, astNode *assignopASTNode)
{
	if (assignopASTNode->child->next->child == NULL && assignopASTNode->child->next->id == TK_ID) // Simple assignment with RHS also as ID
	{
		//printf ("Simple id to id assignment\n") ;
		varST *searchedVarLeft, *searchedVarRight ;
		tokenID typeLeft, typeRight ;

		typeLeft = validateVar (realBase, baseModule, assignopASTNode->child, &searchedVarLeft) ;
		typeRight = validateVar (realBase, baseModule, assignopASTNode->child->next, &searchedVarRight) ;

		//printf ("After both validateVars %s %s\n", tokenIDToString (typeLeft), tokenIDToString(typeRight)) ;

		if (typeLeft == TK_ARRAY && typeRight == TK_ARRAY) 	// Array assignment
		{
			//printf ("Inside here 1\n") ;
			if (searchedVarLeft->arrayIndices->type == searchedVarRight->arrayIndices->type)
			{
				//printf ("Inside here 2\n") ;

				int leftLim1, rightLim1, leftLim2, rightLim2 ;	
				leftLim1 = atoi (searchedVarLeft->arrayIndices->tokLeft->lexeme) ;
				rightLim1 = atoi (searchedVarLeft->arrayIndices->tokRight->lexeme) ;
				leftLim2 = atoi (searchedVarRight->arrayIndices->tokLeft->lexeme) ;
				rightLim2 = atoi (searchedVarRight->arrayIndices->tokRight->lexeme) ;

				if (leftLim1 != leftLim2 || rightLim1 != rightLim2)
				{
					printf ("ERROR : In \"%s\" at line %d, arrays \"%s\" and \"%s\" do not have matching bounds for assignment\n", getParentModuleName (realBase, baseModule), assignopASTNode->tok->lineNumber, searchedVarLeft->lexeme, searchedVarRight->lexeme) ;
					realBase->semanticError = 1 ;
				}
			}
			else
			{
				//printf ("ERROR?\n") ;
				printf ("ERROR : In \"%s\" at line %d, arrays \"%s\" and \"%s\" do not have the same base type\n", getParentModuleName (realBase, baseModule), assignopASTNode->tok->lineNumber, searchedVarLeft->lexeme, searchedVarRight->lexeme) ;
				realBase->semanticError = 1 ;
			}
		}
		else if (typeLeft != 0 && typeRight != 0)
		{
			if (typeLeft != TK_ARRAY && typeRight == TK_ARRAY)
			{
				printf ("ERROR : In \"%s\" at line %d, array \"%s\" cannot be assigned to primitive \"%s\"\n", getParentModuleName (realBase, baseModule), assignopASTNode->tok->lineNumber, searchedVarRight->lexeme, searchedVarLeft->lexeme) ;
				realBase->semanticError = 1 ;
			}
			else if (typeRight != TK_ARRAY && typeLeft == TK_ARRAY)
			{
				printf ("ERROR : In \"%s\" at line %d, primitive \"%s\" cannot be assigned to array \"%s\"\n", getParentModuleName (realBase, baseModule), assignopASTNode->tok->lineNumber, searchedVarRight->lexeme, searchedVarLeft->lexeme) ;
				realBase->semanticError = 1 ;

			}
			else if (typeLeft != typeRight)
			{
				printf ("ERROR : In \"%s\" at line %d, assignment of \"%s\" to \"%s\" are in type conflict\n", getParentModuleName (realBase, baseModule), assignopASTNode->tok->lineNumber, searchedVarRight->lexeme, searchedVarLeft->lexeme) ;
				realBase->semanticError = 1 ;
			}
		}
	}
	else if (assignopASTNode->child->next->child->next == NULL) // <U>
		printf ("HEY : Going to U\n") ;
	else
		printf ("HEY : Skies clear\n") ;
}