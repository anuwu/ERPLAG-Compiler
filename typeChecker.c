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

int validVarIndex (baseST *realBase, moduleST *baseModule, astNode *varIndexASTNode)	// Returns TK_NUM for static index, 0 for invalid dynamic index, TK_ID for valid dynamic index
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
	varST *locSearchedVar ;

	locSearchedVar = searchVar (realBase, baseModule, varASTNode->tok->lexeme) ;

	if (searchedVar != NULL)
		*searchedVar = locSearchedVar ;

	if (locSearchedVar == NULL)
	{
		printf ("ERROR : In \"%s\" at line %d, variable \"%s\" is undeclared\n", getParentModuleName(realBase, baseModule), varASTNode->tok->lineNumber, varASTNode->tok->lexeme) ;

		realBase->semanticError = 1 ;
		if (varASTNode->child != NULL)
			validVarIndex (realBase, baseModule, varASTNode->child) ;

		return 0 ;
	}
	else if (locSearchedVar->datatype != TK_ARRAY)
	{
		if (varASTNode->child != NULL)
		{
			realBase->semanticError = 1 ;
			printf ("ERROR : In \"%s\" at line %d, primitive %s variable \"%s\" cannot be indexed\n", getParentModuleName(realBase, baseModule), varASTNode->child->tok->lineNumber, typeIDToString (locSearchedVar->datatype), locSearchedVar->lexeme) ;
			validVarIndex (realBase, baseModule, varASTNode->child) ;
		}

		return locSearchedVar->datatype ;
	}
	else if (varASTNode->child != NULL)		// It is array and has an index
	{
		isValidVarIndex = validVarIndex(realBase, baseModule, varASTNode->child) ;	// 0 if invalid static/dyanmic, TK_NUM if valid static, TK_ID if valid dynamic

		if (!isValidVarIndex)
			realBase->semanticError = 1 ;
		else if (isVarStaticArr (locSearchedVar) && isValidVarIndex == TK_NUM)		// Static array
		{
			if (!validStaticArrStaticIndex (realBase, baseModule, locSearchedVar, varASTNode->child))
				realBase->semanticError = 1 ;
		}
		
		return locSearchedVar->arrayIndices->type ;
	}
	else
		return TK_ARRAY ;
}

int isArithmeticOp (tokenID id)
{
	if (id == TK_PLUS || id == TK_MINUS || id == TK_MUL || id == TK_DIV)
		return 1 ;
	else
		return 0 ;
}

int isLogicalOp (tokenID id)
{
	if (id == TK_AND || id == TK_OR)
		return 1 ;
	else
		return 0 ;
}

int isRelationalOp (tokenID id)
{
	if (id == TK_LT || id == TK_LE || id == TK_GE || id == TK_GT || id == TK_EQ || id == TK_NE)
		return 1 ;
	else
		return 0 ;
}

int isArithmeticType (tokenID id)
{
	if (id == TK_INTEGER || id == TK_REAL)
		return 1 ;
	else
		return 0 ;
}

tokenID getExpressionType (baseST *realBase, moduleST *baseModule, astNode *exprNode)
{
	if (exprNode->child == NULL || exprNode->child->next == NULL)	// constant value, integer, or array[index]
	{
		if (exprNode->id != TK_ID)
		{
			if (exprNode->id == TK_NUM)
				return TK_INTEGER ;
			else if (exprNode->id == TK_RNUM)
				return TK_REAL ;
			else
				return TK_BOOLEAN ;
		}
		else
			return validateVar (realBase, baseModule, exprNode, NULL) ;
	}
	else
	{
		tokenID typeLeft , typeRight ;
		typeLeft = getExpressionType (realBase, baseModule, exprNode->child) ;
		typeRight = getExpressionType (realBase, baseModule, exprNode->child->next) ;

		if (typeLeft == TK_ARRAY || typeRight == TK_ARRAY)
		{
			printf ("ERROR : In \"%s\" at line %d, array variable(s) \"%s\" and \"%s\" cannot occur in an expression\n", getParentModuleName(realBase, baseModule), exprNode->tok->lineNumber, exprNode->child->tok->lexeme, exprNode->child->next->tok->lexeme) ;
			return 0 ;
		}

		if (!typeLeft || !typeRight)
		{
			realBase->semanticError = 1 ;
			return 0 ;
		}
		else if (typeLeft != typeRight)
		{
			printf ("ERROR : In \"%s\" at line %d, the operation %s is not allowed between types \"%s\" and \"%s\" respectively\n", getParentModuleName(realBase, baseModule), exprNode->tok->lineNumber,  exprNode->tok->lexeme, typeIDToString(typeLeft), typeIDToString(typeRight)) ;
			return 0 ;
		}
		else
		{
			if (isArithmeticType(typeLeft) && isArithmeticOp (exprNode->id))
				return typeLeft ;
			else if (isArithmeticType(typeLeft) && isRelationalOp(exprNode->id))
				return TK_BOOLEAN ;
			else if (typeLeft == TK_BOOLEAN && isLogicalOp(exprNode->id))
				return typeLeft ;
			else
			{
				printf ("ERROR : In \"%s\" at line %d, the operator %s cannot be executed between two operands of type \"%s\"\n", getParentModuleName(realBase, baseModule), exprNode->tok->lineNumber, exprNode->tok->lexeme, typeIDToString(typeLeft)) ;
				realBase->semanticError = 1 ;
				return 0 ;
			}
		}
	}
}

void assignmentTypeCheck (baseST *realBase, moduleST *baseModule, astNode *assignopASTNode)
{
	varST *searchedVarLeft, *searchedVarRight ;
	tokenID typeLeft, typeRight ;

	if (assignopASTNode->child->next->child == NULL && assignopASTNode->child->next->id == TK_ID) // Simple assignment with RHS also as ID
	{	
		typeLeft = validateVar (realBase, baseModule, assignopASTNode->child, &searchedVarLeft) ;
		typeRight = validateVar (realBase, baseModule, assignopASTNode->child->next, &searchedVarRight) ;


		if (typeLeft == TK_ARRAY && typeRight == TK_ARRAY) 	// Array assignment
		{
			if (searchedVarLeft->arrayIndices->type == searchedVarRight->arrayIndices->type)
			{
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

		return ;
	}
	else if (assignopASTNode->child->next->child != NULL && assignopASTNode->child->next->child->next == NULL) // <U>
		typeRight = getExpressionType (realBase, baseModule, assignopASTNode->child->next->child) ;
	else	// expression_new
		typeRight = getExpressionType (realBase, baseModule, assignopASTNode->child->next) ;

	/* ------------------------------------------------------------------------------------------------------------------*/

	typeLeft = validateVar (realBase, baseModule, assignopASTNode->child, &searchedVarLeft) ;
	if (!typeLeft)
		return ;
	else
	{ 
		tinkerVar (realBase, baseModule, searchedVarLeft, assignopASTNode->child) ;
		if (typeLeft == TK_ARRAY)
		{
			printf ("ERROR : In \"%s\" at line %d, the assignment of an expression to array \"%s\" cannot be made\n", getParentModuleName(realBase, baseModule), assignopASTNode->tok->lineNumber, assignopASTNode->child->tok->lexeme) ;
			realBase->semanticError = 1 ;
			return ;
		}
	}

	if (!typeRight)
	{
		//printf ("ERROR : In \"%s\" at line %d, the RHS expression has unknown/badly formed type\n", getParentModuleName(realBase, baseModule), assignopASTNode->tok->lineNumber) ;
		realBase->semanticError = 1 ;
		return ;
	}
	else if (typeLeft != typeRight)
	{
		printf ("ERROR : In \"%s\" at line %d, assignment of expression of type \"%s\" to the variable \"%s\" of type \"%s\" cannot be made\n", getParentModuleName(realBase, baseModule), assignopASTNode->tok->lineNumber, typeIDToString(typeRight), assignopASTNode->child->tok->lexeme, typeIDToString(typeLeft)) ;
		realBase->semanticError = 1 ;
		return ;
	}
}