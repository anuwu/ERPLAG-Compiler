#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "symbolTable.h"
#include "typeChecker.h"
#include "error.h"

extern char* tokenIDToString (tokenID id) ;

int isLeftLimStatic (varST *arrayVar)
{
	return isdigit (arrayVar->arrayIndices->tokLeft->lexeme[0]) ;
}

int isRightLimStatic (varST *arrayVar)
{
	return isdigit (arrayVar->arrayIndices->tokRight->lexeme[0]) ;
}

int isVarStaticArr (varST *arrayVar)
{
	if (isLeftLimStatic(arrayVar) && isRightLimStatic(arrayVar))
		return 1 ;
	else
		return 0 ;
}

int validVarIndex (moduleST *baseModule, astNode *varIndexASTNode)	// Returns TK_NUM for static index, 0 for invalid dynamic index, TK_ID for valid dynamic index
{
	// Assumes varIndexASTNode isn't NULL

	if (varIndexASTNode->id == TK_NUM)
		return TK_NUM ;

	varST *indexVar = searchVar (baseModule, varIndexASTNode->tok->lexeme) ;
	if (indexVar == NULL)
	{
		errSemantic () ;
		#ifdef _WIN64
			printf ("In module \"%s\" at line %d, index variable \"%s\" is undeclared\n", getParentModuleName (baseModule), varIndexASTNode->tok->lineNumber, varIndexASTNode->tok->lexeme) ;
		#endif
		#if defined __linux__ || defined __MACH__
			printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", index variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " is undeclared\n", getParentModuleName (baseModule), varIndexASTNode->tok->lineNumber, varIndexASTNode->tok->lexeme) ;
		#endif
		realBase->semanticError = 1 ;
		return 0 ;
	}
	else if (indexVar->datatype != TK_INTEGER)
	{
		errSemantic () ;
		#ifdef _WIN64
			printf ("In module \"%s\" at line %d, index variable \"%s\" needs to be of type integer\n", getParentModuleName (baseModule), varIndexASTNode->tok->lineNumber, varIndexASTNode->tok->lexeme) ;
		#endif
		#if defined __linux__ || defined __MACH__
			printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", index variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " needs to be of type integer\n", getParentModuleName (baseModule), varIndexASTNode->tok->lineNumber, varIndexASTNode->tok->lexeme) ;
		#endif
		realBase->semanticError = 1 ;
		return 0 ;
	}

	return TK_ID ;
}

/*
searchVar ()
	--> NULL 											==> SEMANTIC ERROR
	--> NOT NULLValidVar
		--> Primitive
		--> Array
				--> dynamic array
					--> dynamic index 					==> searchVar
						--> NULL						==> SEMANTIC ERROR index variable does not exist
						--> NOT NULL
							--> integer type
							--> non-integer type 		==> SEMANTIC ERROR index variable is not of integer type
					--> static index 					==> No checks 
				--> static array
					--> dynamic index 					==> searchVar
						--> NULL						==> SEMANTIC ERROR index variable does not exist
						--> NOT NULL
							--> integer type 			 
							--> non-integer type 		==> SEMANTIC ERROR index variable not of integer type
					--> static index 	==> Check bounds
						--> Correct bounds
						--> Incorrect bounds 			==> SEMANTIC ERROR incorrect bounds for static array
*/

int validStaticArrStaticIndex (moduleST *baseModule, varST *arrayVar, astNode *indASTNode)
{
	int num = atoi(indASTNode->tok->lexeme) , leftLim = atoi(arrayVar->arrayIndices->tokLeft->lexeme), rightLim = atoi(arrayVar->arrayIndices->tokRight->lexeme) ;
	if (num < leftLim || num > rightLim)
	{
		errSemantic () ;
		#ifdef _WIN64
			printf ("In module \"%s\" at line %d, specified index \"%s\" is out of bounds for static array variable \"%s\"\n", getParentModuleName (baseModule), indASTNode->tok->lineNumber, indASTNode->tok->lexeme, arrayVar->lexeme) ;
		#endif
		#if defined __linux__ || defined __MACH__
			printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", specified index " ANSI_BOLD ANSI_CYAN "%s" ANSI_RESET " is out of bounds for " ANSI_BOLD ANSI_BLACK "static array" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s\n" ANSI_RESET, getParentModuleName (baseModule), indASTNode->tok->lineNumber, indASTNode->tok->lexeme, arrayVar->lexeme) ;
		#endif
		realBase->semanticError = 1 ;
		return 0 ;
	}

	return 1 ;
}


tokenID validateVar (moduleST *baseModule , astNode *varASTNode, varST **searchedVar)
{
	int isValidVarIndex ;
	varST *locSearchedVar ;

	locSearchedVar = searchVar (baseModule, varASTNode->tok->lexeme) ;

	if (searchedVar != NULL)
		*searchedVar = locSearchedVar ;

	if (locSearchedVar == NULL)
	{
		errSemantic () ;
		#ifdef _WIN64
			printf ("In module \"%s\" at line %d, variable \"%s\" is undeclared\n", getParentModuleName (baseModule), varASTNode->tok->lineNumber, varASTNode->tok->lexeme) ;
		#endif
		#if defined __linux__ || defined __MACH__
			printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " is undeclared\n", getParentModuleName (baseModule), varASTNode->tok->lineNumber, varASTNode->tok->lexeme) ;
		#endif
		realBase->semanticError = 1 ;

		if (varASTNode->child != NULL)
			validVarIndex (baseModule, varASTNode->child) ;

		return (tokenID)0 ;
	}
	else if (locSearchedVar->datatype != TK_ARRAY)
	{
		if (varASTNode->child != NULL)
		{
			errSemantic () ;
			#ifdef _WIN64
				printf ("In module \"%s\" at line %d, primitive \"%s\" variable \"%s\" cannot be indexed\n", getParentModuleName (baseModule), varASTNode->child->tok->lineNumber, typeIDToString (locSearchedVar->datatype), locSearchedVar->lexeme) ;
			#endif
			#if defined __linux__ || defined __MACH__
				printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "primitive %s" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " cannot be indexed\n", getParentModuleName (baseModule), varASTNode->child->tok->lineNumber, typeIDToString (locSearchedVar->datatype), locSearchedVar->lexeme) ;
			#endif
			realBase->semanticError = 1 ;

			validVarIndex (baseModule, varASTNode->child) ;
		}

		return locSearchedVar->datatype ;
	}
	else if (varASTNode->child != NULL)		// It is array and has an index
	{
		isValidVarIndex = validVarIndex(baseModule, varASTNode->child) ;	// 0 if invalid static/dyanmic, TK_NUM if valid static, TK_ID if valid dynamic

		if (!isValidVarIndex)
			realBase->semanticError = 1 ;
		else if (isVarStaticArr (locSearchedVar) && isValidVarIndex == TK_NUM)		// Static array
		{
			if (!validStaticArrStaticIndex (baseModule, locSearchedVar, varASTNode->child))
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

tokenID getExpressionType (moduleST *baseModule, astNode *exprNode)
{
	if (exprNode->child == NULL || exprNode->child->next == NULL)	// constant value, integer, or array[index]
	{
		if (exprNode->id != TK_ID)
		{
			if (exprNode->id == TK_NUM)
				return TK_INTEGER ;
			else if (exprNode->id == TK_RNUM)
			{
				realPresent = 1 ;
				return TK_REAL ;
			}
			else
				return TK_BOOLEAN ;
		}
		else
			return validateVar (baseModule, exprNode, NULL) ;
	}
	else
	{
		tokenID typeLeft , typeRight ;
		typeLeft = getExpressionType (baseModule, exprNode->child) ;
		typeRight = getExpressionType (baseModule, exprNode->child->next) ;

		if (typeLeft == TK_ARRAY || typeRight == TK_ARRAY)
		{
			errSemantic () ;
			#ifdef _WIN64
				printf ("In module \"%s\" at line %d, array variable(s) \"%s\" and \"%s\" cannot occur in an expression\n", getParentModuleName (baseModule), exprNode->tok->lineNumber, exprNode->child->tok->lexeme, exprNode->child->next->tok->lexeme) ;
			#endif
			#if defined __linux__ || defined __MACH__
				printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variable(s) " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " and " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " cannot occur in an expression\n", getParentModuleName (baseModule), exprNode->tok->lineNumber, exprNode->child->tok->lexeme, exprNode->child->next->tok->lexeme) ;
			#endif
			return (tokenID)0 ;
		}

		if (!typeLeft || !typeRight)
			return (tokenID)0 ;
		else if (typeLeft != typeRight)
		{
			errSemantic () ;
			#ifdef _WIN64
				printf ("In module \"%s\" at line %d, the operator \"%s\" is not allowed between types \"%s\" and \"%s\" respectively\n", getParentModuleName (baseModule), exprNode->tok->lineNumber,  exprNode->tok->lexeme, typeIDToString(typeLeft), typeIDToString(typeRight)) ;
			#endif
			#if defined __linux__ || defined __MACH__
				printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", the operator " ANSI_BOLD ANSI_CYAN "%s" ANSI_RESET " is not allowed between types " ANSI_BOLD ANSI_GREEN "%s" ANSI_RESET " and " ANSI_BOLD ANSI_GREEN "%s" ANSI_RESET " respectively\n", getParentModuleName (baseModule), exprNode->tok->lineNumber,  exprNode->tok->lexeme, typeIDToString(typeLeft), typeIDToString(typeRight)) ;
			#endif
			return (tokenID)0 ;
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
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, the operator \"%s\" cannot be executed between two operands of type \"%s\"\n", getParentModuleName (baseModule), exprNode->tok->lineNumber, exprNode->tok->lexeme, typeIDToString(typeLeft)) ;
				#endif
				#if defined __linux__ || defined __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", the operator " ANSI_BOLD ANSI_CYAN "%s" ANSI_RESET " cannot be executed between two operands of type " ANSI_BOLD ANSI_GREEN "%s" ANSI_RESET "\n", getParentModuleName (baseModule), exprNode->tok->lineNumber, exprNode->tok->lexeme, typeIDToString(typeLeft)) ;
				#endif
				realBase->semanticError = 1 ;
				return (tokenID)0 ;
			}
		}
	}
}

void assignmentTypeCheck (moduleST *baseModule, astNode *assignopASTNode)
{
	varST *searchedVarLeft, *searchedVarRight ;
	tokenID typeLeft, typeRight ;

	if (assignopASTNode->child->next->child == NULL && assignopASTNode->child->next->id == TK_ID) // Simple assignment with RHS also as ID
	{	
		typeLeft = validateVar (baseModule, assignopASTNode->child, &searchedVarLeft) ;
		typeRight = validateVar (baseModule, assignopASTNode->child->next, &searchedVarRight) ;

		if (typeLeft == TK_ARRAY && typeRight == TK_ARRAY) 	// Array assignment
		{
			if (searchedVarLeft->arrayIndices->type == searchedVarRight->arrayIndices->type)
			{
				if (isVarStaticArr (searchedVarLeft) && isVarStaticArr (searchedVarRight))
				{
					int leftLim1, rightLim1, leftLim2, rightLim2 ;	
					leftLim1 = atoi (searchedVarLeft->arrayIndices->tokLeft->lexeme) ;
					rightLim1 = atoi (searchedVarLeft->arrayIndices->tokRight->lexeme) ;
					leftLim2 = atoi (searchedVarRight->arrayIndices->tokLeft->lexeme) ;
					rightLim2 = atoi (searchedVarRight->arrayIndices->tokRight->lexeme) ;

					if (leftLim1 != leftLim2 || rightLim1 != rightLim2)
					{
						errSemantic () ;
						#ifdef _WIN64
							printf ("In module \"%s\" at line %d, array variables \"%s\" and \"%s\" do not have matching bounds for assignment\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, searchedVarLeft->lexeme, searchedVarRight->lexeme) ;
						#endif
						#if defined __linux__ || defined __MACH__
							printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variables " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " and " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " do not have matching bounds for assignment\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, searchedVarLeft->lexeme, searchedVarRight->lexeme) ;
						#endif
						realBase->semanticError = 1 ;
					}
				}
			}
			else
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, array variables \"%s\" and \"%s\" do not have the same base type\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, searchedVarLeft->lexeme, searchedVarRight->lexeme) ;
				#endif
				#if defined __linux__ || defined __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variables " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " and " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " do not have the same base type\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, searchedVarLeft->lexeme, searchedVarRight->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}
		}
		else if (typeLeft != 0 && typeRight != 0)
		{
			if (typeLeft != TK_ARRAY && typeRight == TK_ARRAY)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, array variable \"%s\" cannot be assigned to primitive variable \"%s\"\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, searchedVarRight->lexeme, searchedVarLeft->lexeme) ;
				#endif
				#if defined __linux__ || defined __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " cannot be assigned to " ANSI_BOLD ANSI_BLACK "primitive" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET "\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, searchedVarRight->lexeme, searchedVarLeft->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}
			else if (typeRight != TK_ARRAY && typeLeft == TK_ARRAY)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, primitive variable \"%s\" cannot be assigned to array variable \"%s\"\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, searchedVarRight->lexeme, searchedVarLeft->lexeme) ;
				#endif
				#if defined __linux__ || defined __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", " ANSI_BOLD ANSI_BLACK "primitive" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " cannot be assigned to " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET "\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, searchedVarRight->lexeme, searchedVarLeft->lexeme) ;
				#endif
				realBase->semanticError = 1 ;

			}
			else if (typeLeft != typeRight)
			{
				errSemantic () ;
				#ifdef _WIN64
					printf ("In module \"%s\" at line %d, assignment of \"%s\" to \"%s\" are in type conflict\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, searchedVarRight->lexeme, searchedVarLeft->lexeme) ;
				#endif
				#if defined __linux__ || defined __MACH__
					printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", assignment of " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " to " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " are in type conflict\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, searchedVarRight->lexeme, searchedVarLeft->lexeme) ;
				#endif
				realBase->semanticError = 1 ;
			}

			tinkerVar (baseModule, searchedVarLeft, assignopASTNode->child) ;
		}

		return ;
	}
	else if (assignopASTNode->child->next->child != NULL && assignopASTNode->child->next->child->next == NULL && (assignopASTNode->child->next->id == TK_PLUS || assignopASTNode->child->next->id == TK_MINUS)) // <U>
		typeRight = getExpressionType (baseModule, assignopASTNode->child->next->child) ;
	else	// expression_new
		typeRight = getExpressionType (baseModule, assignopASTNode->child->next) ;

	/* ------------------------------------------------------------------------------------------------------------------*/

	typeLeft = validateVar (baseModule, assignopASTNode->child, &searchedVarLeft) ;
	if (!typeLeft)
		return ;
	else
	{ 
		tinkerVar (baseModule, searchedVarLeft, assignopASTNode->child) ;
		if (typeLeft == TK_ARRAY)
		{
			errSemantic () ;
			#ifdef _WIN64
				printf ("In module \"%s\" at line %d, the assignment of an expression to array variable \"%s\" cannot be made\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, assignopASTNode->child->tok->lexeme) ;
			#endif
			#if defined __linux__ || defined __MACH__
				printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", the assignment of an expression to " ANSI_BOLD ANSI_BLACK "array" ANSI_RESET " variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " cannot be made\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, assignopASTNode->child->tok->lexeme) ;
			#endif
			realBase->semanticError = 1 ;
			return ;
		}
	}

	if (!typeRight)
		return ;
	else if (typeLeft != typeRight)
	{
		errSemantic () ;
		#ifdef _WIN64
			printf ("In module \"%s\" at line %d, assignment of expression of type \"%s\" to the variable \"%s\" of type \"%s\" cannot be made\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, typeIDToString(typeRight), assignopASTNode->child->tok->lexeme, typeIDToString(typeLeft)) ;
		#endif
		#if defined __linux__ || defined __MACH__
			printf ("In module " ANSI_BOLD "%s" ANSI_RESET " at line " ANSI_BOLD ANSI_CYAN "%d" ANSI_RESET ", assignment of expression of type " ANSI_BOLD ANSI_GREEN "%s" ANSI_RESET " to the variable " ANSI_BOLD ANSI_RED "%s" ANSI_RESET " of type " ANSI_BOLD ANSI_GREEN "%s" ANSI_RESET " cannot be made\n", getParentModuleName (baseModule), assignopASTNode->tok->lineNumber, typeIDToString(typeRight), assignopASTNode->child->tok->lexeme, typeIDToString(typeLeft)) ;
		#endif
		realBase->semanticError = 1 ;
		return ;
	}
}