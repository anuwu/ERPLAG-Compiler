#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/time.h>
#include "typeChecker.h"
#include "codegen.h"
#include "error.h"

int switchLabelCount = 0 ;
int forLabelCount = 0 ;
int whileLabelCount = 0 ;
int otherLabelCount = 0 ;
int divZeroLabelCount = 0 ;

int asmLines = 0 ;
int tf = 0 ;
int df = 0 ;
char offsetStr[10] ;

int get_label (labelType lt)
{
	switch (lt)
	{
		case LABEL_SWITCH :
			switchLabelCount++ ;
			return switchLabelCount ;

		case LABEL_FOR :
			forLabelCount++ ;
			return forLabelCount ;

		case LABEL_WHILE :
			whileLabelCount++ ;
			return whileLabelCount ;

		case LABEL_DIV_ZERO :
			divZeroLabelCount++ ;
			return divZeroLabelCount ;

		case LABEL_OTHER :
			otherLabelCount++ ;
			return otherLabelCount ;
	}
}

void codeComment (int tabCount, char *comment)
{
	int i ;
	for (i = 0 ; i < tabCount ; i++)
		codePrint ("\t") ;

	codePrint ("; %s\n", comment) ;
}

char* getOffsetStr (int offset)
{	
	if (offset > 0)
		sprintf (offsetStr, " - %d", offset) ;
	else if (offset < 0)
		sprintf (offsetStr, " + %d", -offset) ;
	else
		memset (offsetStr, 0, 10) ;

	return offsetStr ;
}

int isIndexStatic (astNode *node)
{
	return isdigit (node->child->tok->lexeme[0]) ;
}

void loadRegLeftLim (varST *vst, char *reg)
{
	if (isLeftLimStatic (vst))
		codePrint ("\t\tMOV %s, %s\n", reg, vst->arrayIndices->tokLeft->lexeme) ;
	else
		codePrint ("\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(vst->offset-10)) ;
}

void loadRegRightLim (varST *vst, char *reg)
{
	if (isRightLimStatic (vst))
		codePrint ("\t\tMOV %s, %s\n", reg, vst->arrayIndices->tokRight->lexeme) ;		
	else
		codePrint ("\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(vst->offset-8)) ;
}

void loadArrBase (varST *vst, char *reg)
{
	if (isVarStaticArr (vst))
	{
		if (vst->varType == VAR_LOCAL)
		{
			codePrint ("\t\tMOV %s, RBP\n", reg) ;
			codePrint ("\t\tSUB %s, %d\n", reg, vst->offset) ;			
		}
		else
			codePrint ("\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr (vst->offset)) ;
	}
	else
		codePrint ("\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(vst->offset)) ;
}


int getStaticOffset (varST *vst, astNode *node, int size)
{
	return vst->offset - size*(atoi(node->child->tok->lexeme) - atoi(vst->arrayIndices->tokLeft->lexeme)) ;
}

void arrBoundCheck (astNode *node, moduleST *lst, varST *vst)
{
	varST *indexVar = NULL ;
	if (!isIndexStatic (node))	
		indexVar = searchVar (lst, node->child->tok->lexeme) ;

	tf |= 1 << boundCheck ;

	loadArrBase (vst, "RDI") ;
	loadRegLeftLim (vst, "BX") ;

	if (indexVar == NULL)
		codePrint ("\t\tMOV CX, %s\n", node->child->tok->lexeme) ;
	else
		codePrint ("\t\tMOV CX, [RBP%s]\n", getOffsetStr (indexVar->offset)) ;

	loadRegRightLim (vst, "DX") ;

	codePrint ("\t\tCALL @boundCheck") ;
	codeComment (10, "checking array index bound") ;
}

int isExprLeaf (tokenID id)
{
	switch (id)
	{
		case TK_ID :
			return 1 ;
		case TK_NUM :
			return 1 ;
		case TK_TRUE :
			return 1 ;
		case TK_FALSE :
			return 1 ;

		default :
			return 0 ;
	}
}

int isUnary (tokenID id)
{
	return (id == TK_PLUS || id == TK_MINUS) ;
}

int isAssignUnary (astNode *assignNode)
{
	return (isUnary (assignNode->child->next->id) && assignNode->child->next->child->next == NULL) ;
}

int isArrayRHS (astNode *assignNode, moduleST *lst)
{
	varST *searchedVar ;

	if (assignNode->child->next->id == TK_ID)
	{
		searchedVar = searchVar (lst, assignNode->child->next->tok->lexeme) ;
		if (searchedVar->datatype == TK_ARRAY && assignNode->child->next->child == NULL)
			return 1 ;
	}

	return 0 ;
}

int isSingleRHS (astNode *assignNode)
{
	return isExprLeaf (assignNode->child->next->id) ;
}

void dynAsgnArr (varST *lhs, varST *rhs)
{
	tf |= 1 << asgnLimERROR ;
	int leftLab, rightLab ;

	leftLab = get_label (LABEL_OTHER) ;
	rightLab = get_label (LABEL_OTHER) ;

	loadRegLeftLim (lhs, "AX") ;
	loadRegLeftLim (rhs, "CX") ;

	codePrint ("\t\tCMP AX, CX\n") ;
	codePrint ("\t\tJE @LABEL%d\n", leftLab) ;
	codePrint ("\t\tCALL @asgnLimERROR\n") ;

	codePrint ("\n\t@LABEL%d:\n", leftLab) ;
	loadRegRightLim (lhs, "BX") ;
	loadRegRightLim (rhs, "DX") ;

	codePrint ("\t\tCMP BX, DX\n") ;
	codePrint ("\t\tJE @LABEL%d\n", rightLab) ;
	codePrint ("\t\tCALL @asgnLimERROR\n") ;

	codePrint ("\n\t@LABEL%d:\n", rightLab) ;
	loadArrBase (rhs, "RDI") ;
	codePrint ("\t\tMOV[RBP%s], RDI", getOffsetStr(lhs->offset)) ;
}

void exprAssign (astNode *node, moduleST *lst, int singleAssign)
{
	if (!singleAssign)
		codePrint ("\t\tPOP AX\n") ;

	if(node->child == NULL)
	{
		varST *searchedVar = searchVar (lst, node->tok->lexeme) ;
		if (node->next->id == TK_MINUS && node->next->child->next == NULL)
			codePrint ("\t\tNEG AX\n") ;

		codePrint ("\t\tMOV [RBP%s], AX", getOffsetStr (searchedVar->offset)) ;
	}
	else
	{
		varST* vst ;
		vst = searchVar (lst,node->tok->lexeme);

		if (isVarStaticArr(vst) && node->child->id == TK_NUM)
		{
			if (node->next->id == TK_MINUS && node->next->child->next == NULL)
				codePrint ("\t\tNEG AX\n") ;

			codePrint ("\t\tMOV [RBP%s], AX" , getOffsetStr(getStaticOffset(vst,node,2))) ;
		}
		else
		{
			if (node->next->id == TK_MINUS && node->next->child->next == NULL)
				codePrint ("\t\tNEG AX\n") ;

			arrBoundCheck (node, lst, vst) ;
			codePrint ("\t\tMOV [RDI + RBX], AX") ;
		}
	}
	codeComment (10, "store variable") ;

}

void exprLeaf (astNode *node, moduleST *lst, int singleAssign)
{
	char reg[3] ;

	if (!singleAssign && node->prev != NULL)
		strcpy (reg, "BX") ;
	else
		strcpy (reg, "AX") ;

	switch (node->id)
	{
		case TK_ID :
			if (node->child == NULL)
			{
				varST *searchedVar = searchVar (lst, node->tok->lexeme) ;
				codePrint ("\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(searchedVar->offset)) ;
			}
			else
			{
				varST *vst ;
				vst = searchVar (lst,node->tok->lexeme) ;

				if (isVarStaticArr(vst) && node->child->id == TK_NUM)
					codePrint ("\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(getStaticOffset (vst, node, 2))) ;						
				else
				{
					arrBoundCheck (node, lst, vst) ;
					codePrint ("\t\tMOV %s, [RDI + RBX]\n\n", reg) ;
				}				
			}

			break ;

		case TK_RNUM :
			break ;

		case TK_NUM : 
			codePrint ("\t\tMOV %s, %s\n", reg, node->tok->lexeme);
			break ;

		case TK_TRUE :
			codePrint ("\t\tMOV %s, 1\n", reg);
			break ;

		case TK_FALSE :
			codePrint ("\t\tMOV %s, 0\n", reg);
			break ;
	}

	if (!singleAssign)
		codePrint ("\t\tPUSH %s\n", reg);	
}

void expr (astNode *node, moduleST *lst, int singleAssign)
{
	if (node->child == NULL || node->child->next == NULL)
	{
		exprLeaf (node, lst, singleAssign) ;
		return ;
	}

	expr (node->child->next, lst, singleAssign) ;
	expr (node->child, lst, singleAssign); 
	codePrint ("\t\tPOP AX\n");
	codePrint ("\t\tPOP BX\n");

	switch (node->id)
	{
		case TK_PLUS :
			codePrint ("\t\tADD AX,BX\n");
			break ;

		case TK_MINUS :
			codePrint ("\t\tSUB AX,BX\n");
			break ;

		case TK_MUL :
			codePrint ("\t\tIMUL BX\n");
			break ;

		case TK_DIV :
		{
			codePrint ("\t\tCMP BX, 0\n") ;

			tf |= 1 << divZeroERROR ;
			int lab = get_label (LABEL_DIV_ZERO) ;
			codePrint ("\t\tJNE @DIV_ZERO%d\n", lab) ;
			codePrint ("\t\tCALL @divZeroERROR\n") ;

			codePrint ("\n\t@DIV_ZERO%d:\n", lab) ;
			codePrint ("\t\tCWD\n") ;
			codePrint ("\t\tIDIV BX\n");
			break ;
		}

		case TK_LT : case TK_GT : case TK_LE : case TK_GE : case TK_NE : case TK_EQ :
		{
			codePrint ("\t\tCMP AX,BX\n");

			switch (node->id)
			{
				case TK_LT :
					codePrint ("\t\tSETL AL\n");
					break ;
				case TK_GT :
					codePrint ("\t\tSETG AL\n");
					break ;
				case TK_LE :
					codePrint ("\t\tSETLE AL\n");
					break ;
				case TK_GE :
					codePrint ("\t\tSETGE AL\n");
					break ;
				case TK_NE :
					codePrint ("\t\tSETNE AL\n");
					break ;
				case TK_EQ :
					codePrint ("\t\tSETE AL\n");
					break ;
			}
			codePrint ("\t\tMOVSX AX, AL\n") ;

			break ;
		}

		case TK_AND : case TK_OR :
		{
			if (node->id == TK_AND)
				codePrint ("\t\tAND AX, BX\n");
			else
				codePrint ("\t\tOR AX, BX\n");

			break ;
		}
	}

	codePrint ("\t\tPUSH AX\n") ;
}

void print (astNode *node, moduleST *lst)
{
	int start_label, end_label ;
	int reserveLabel[2] ;

	char valReg[5] ;
	#if defined __linux__ || defined __MACH__
		strcpy (valReg, "SI") ;
	#endif
	#ifdef _WIN64
		strcpy (valReg, "DX") ;
	#endif

	varST *searchedVar ;
	if (node->id == TK_ID)
		searchedVar = searchVar (lst, node->tok->lexeme) ;
	else
	{
		if (node->id == TK_NUM)
		{
			tf |= 1 << printInteger ;
			codePrint ("\t\tMOV %s, %s\n", valReg, node->tok->lexeme) ;
			codePrint ("\t\tCALL @printInteger\n\n") ;
		}
		else if (node->id == TK_TRUE || node->id == TK_FALSE)
		{
			tf |= 1 << printBoolean ;
			codePrint ("\t\tMOV %s, %d\n", valReg, node->tok->lexeme[0]=='t'?1:0) ;
			codePrint ("\t\tCALL @printBoolean\n\n") ;
		}

		return ;
	}

	if (searchedVar->datatype == TK_INTEGER)
	{
		tf |= 1 << printInteger ;
		codePrint ("\t\tMOV %s, [RBP%s]\n", valReg, getOffsetStr(searchedVar->offset)) ;
		codePrint ("\t\tCALL @printInteger\n\n") ;
	}
	else if (searchedVar->datatype == TK_BOOLEAN)
	{
		tf |= 1 << printBoolean ;
		codePrint ("\t\tMOV %s, [RBP%s]\n", valReg, getOffsetStr(searchedVar->offset)) ;
		codePrint ("\t\tCALL @printBoolean\n\n") ;
	}
	else // Array type
	{	
		if (node->child != NULL)
		{
			// Special Case
			if (node->child->id == TK_NUM && isVarStaticArr (searchedVar))
				codePrint ("\t\tMOV %s, [RBP%s]\n", valReg, getOffsetStr(getStaticOffset(searchedVar,node,2))) ;
			else
			{
				// General case of static array with dynamic indices, or dynamic array
				arrBoundCheck (node, lst, searchedVar) ;
				codePrint ("\t\tMOV %s, [RDI + RBX]\n", valReg) ;
			}

			if (searchedVar->arrayIndices->type == TK_INTEGER)
			{
				tf |= 1 << printInteger ;
				codePrint ("\t\tCALL @printInteger\n\n") ;
			}
			else
			{
				tf |= 1 << printBoolean ;
				codePrint ("\t\tCALL @printBoolean\n\n") ;
			}
		}
		else
		{
			loadArrBase (searchedVar, "RDI") ;

			#if defined __linux__ || defined __MACH__
				loadRegLeftLim (searchedVar, "CX") ;
				loadRegRightLim (searchedVar, "DX") ;
				codePrint ("\t\tSUB DX, CX\n") ;
				codePrint ("\t\tADD DX, 1\n") ;
				codePrint ("\t\tADD DX, DX\n") ;
			#endif
			#ifdef _WIN64
				loadRegLeftLim (searchedVar, "AX") ;
				loadRegRightLim (searchedVar, "BX") ;
				codePrint ("\t\tSUB BX, AX\n") ;
				codePrint ("\t\tADD BX, 1\n") ;
				codePrint ("\t\tADD BX, BX\n") ;
			#endif

			if (searchedVar->arrayIndices->type == TK_INTEGER)
			{
				tf |= 1 << printIntegerArr ;
				codePrint ("\t\tCALL @printIntegerArr\n\n") ;
			}
			else
			{
				tf |= 1 << printBooleanArr ;
				codePrint ("\t\tCALL @printBooleanArr\n\n") ;
			}
		}
	}
}

void RSPAlign ()
{
	codePrint ("\t\tMOV RAX, RSP") ;
	codeComment (9, "Stack Alignment") ;
	codePrint ("\t\tAND RAX, 15\n") ;
	codePrint ("\t\tADD RAX, 8\n") ;
	codePrint ("\t\tSUB RSP, RAX\n") ;
	codePrint ("\t\tPUSH RAX\n") ;
}

void RSPRestore ()
{
	codePrint ("\n\t\tPOP RAX\n") ;
	codePrint ("\t\tADD RSP, RAX") ;
	codeComment (9, "Restoring Stack Alignment") ;
}

void getValue (moduleST *lst, varST *vst)
{		
	int rspAlign ;
	df |= 1 << inputInt ;
	char promptReg[5] ;

	#if defined __linux__ || defined __MACH__
		strcpy (promptReg, "RDI") ;
	#endif
	#ifdef _WIN64
		strcpy (promptReg, "RCX") ;
	#endif

	if (vst->datatype == TK_INTEGER || vst->datatype == TK_BOOLEAN)
	{
		if (vst->datatype == TK_INTEGER)
		{
			df |= 1 << inputIntPrompt ;
			codePrint ("\n\t\tMOV %s, @inputIntPrompt", promptReg) ;
		}
		else if (vst->datatype == TK_BOOLEAN)
		{
			df |= 1 << inputBoolPrompt ;
			codePrint ("\n\t\tMOV %s, @inputBoolPrompt", promptReg) ;
		}
		codeComment (9, "get_value") ;


		tf |= 1 << getValuePrimitive ;
		codePrint ("\t\tMOV RBX, -%d\n", vst->offset) ;
		codePrint ("\t\tCALL @getValuePrimitive\n\n") ;		
 	}
	else // Array type
	{	
		if (vst->arrayIndices->type == TK_INTEGER)
		{
			df |= 1 << inputIntArrPrompt ;
			codePrint ("\t\tMOV %s, @inputIntArrPrompt\n", promptReg) ;
		}
		else
		{
			df |= 1 << inputBoolArrPrompt ;
			codePrint ("\t\tMOV %s, @inputBoolArrPrompt\n", promptReg) ;
		}

		tf |= 1 << printGetArrPrompt ;
		tf |= 1 << getArr ;

		#if defined __linux__ || defined __MACH__
			loadRegLeftLim (vst, "BX") ;
			loadRegRightLim (vst, "CX") ;
		#endif
		#ifdef _WIN64
			loadRegLeftLim (vst, "SI") ;
			loadRegRightLim (vst, "DI") ;
		#endif
		codePrint ("\t\tCALL @printGetArrPrompt\n\n") ;

		#if defined __linux__ || defined __MACH__		
			codePrint ("\t\tMOV DX, CX\n") ;
			codePrint ("\t\tSUB DX, BX\n") ;
			codePrint ("\t\tADD DX, 1\n") ;
			codePrint ("\t\tADD DX, DX\n") ;
			codePrint ("\t\tMOVSX RDX, DX\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tMOV BX, DI\n") ;
			codePrint ("\t\tSUB BX, SI\n") ;
			codePrint ("\t\tADD BX, 1\n") ;
			codePrint ("\t\tADD BX, BX\n") ;
			codePrint ("\t\tMOVSX RBX, BX\n") ;
		#endif

		loadArrBase (vst, "RDI") ;
		codePrint ("\t\tCALL @getArr\n\n") ;
	}
}

void dynamicDeclarationCheck (moduleST *lst, varST *vst)
{
	int start_label , end_label ;
	varST *leftVar , *rightVar ;
	tf |= 1 << dynamicDeclCheck ;

	codePrint ("\n") ;
	if (isLeftLimStatic (vst))
		codePrint ("\t\tMOV AX, %s\n", vst->arrayIndices->tokLeft->lexeme) ;
	else
	{
		leftVar = searchVar (lst, vst->arrayIndices->tokLeft->lexeme) ;
		codePrint ("\t\tMOV AX, [RBP%s]\n", getOffsetStr (leftVar->offset)) ;
	}

	if (isRightLimStatic (vst))
		codePrint ("\t\tMOV BX, %s\n", vst->arrayIndices->tokRight->lexeme) ;
	else
	{
		rightVar = searchVar (lst, vst->arrayIndices->tokRight->lexeme) ;
		codePrint ("\t\tMOV BX, [RBP%s]\n", getOffsetStr(rightVar->offset)) ;
	}

	codePrint ("\t\tCALL @dynamicDeclCheck") ;
	codeComment (10, "checking dynamic array declaration limits") ;

	codePrint ("\n\n") ;
}

void dynamicDeclaration (moduleST *lst, varST *vst, int declCount)
{
	RSPAlign () ;
	dynamicDeclarationCheck (lst, vst) ;

	codePrint ("\t\tPUSH BX") ;
	codeComment (12, "saving register for malloc") ;
	codePrint ("\t\tPUSH AX") ;
	codeComment (12, "saving register for malloc") ;

	int RSPsub ;

	if (declCount > 1)
	{
		char counterReg[3], mallocReg[5] ;
		
		#if defined __linux__ || defined __MACH__
			strcpy (counterReg, "CX") ;
			strcpy (mallocReg, "RDI") ;
			RSPsub = 10 ;
		#endif
		#ifdef _WIN64
			strcpy (counterReg, "AX") ;
			strcpy (mallocReg, "RCX") ;
			RSPsub = 42 ;
		#endif

		codePrint ("\t\tMOV RBX, %d\n", -vst->offset) ;
		codePrint ("\n\t\tMOV %s, 0\n", counterReg) ;

		codePrint ("\n\t.declLoop:\n") ;

		codePrint ("\t\tPUSH %s\n", counterReg) ;
		codePrint ("\t\tPUSH %s\n", mallocReg) ;
		codePrint ("\t\tPUSH RBX\n") ;
		codePrint ("\t\tSUB RSP, %d\n", RSPsub) ;

		#if defined __linux__ || defined _WIN64
			codePrint ("\t\tCALL malloc\n") ;
		#endif
		#ifdef __MACH__
			codePrint ("\t\tCALL _malloc\n") ;
		#endif

		codePrint ("\t\tADD RSP, %d\n", RSPsub) ;	
		codePrint ("\t\tPOP RBX\n") ;
		codePrint ("\t\tMOV [RBP + RBX], RAX\n") ;
		codePrint ("\t\tPOP %s\n", mallocReg) ;
		codePrint ("\t\tPOP %s\n", counterReg) ;
		
		codePrint ("\t\tPOP SI\n") ;
		codePrint ("\t\tMOV [RBP + RBX + 10], SI\n") ;
		codePrint ("\t\tPOP DI\n") ;
		codePrint ("\t\tMOV [RBP + RBX + 8], DI\n") ;
		codePrint ("\t\tPUSH DI\n") ;
		codePrint ("\t\tPUSH SI\n") ;
		codePrint ("\t\tADD RBX, 12\n") ;

		codePrint ("\t\tINC %s\n", counterReg) ;
		codePrint ("\t\tCMP %s, %d\n", counterReg, declCount) ;

		codePrint ("\t\tJL .declLoop\n") ;

		codePrint ("\t\tADD RSP, 4") ;
		codeComment (8, "Restoring array limits") ;
	}
	else
	{
		#if defined __linux__ || defined __MACH__
			RSPsub = 12 ;
		#endif
		#ifdef _WIN64
			RSPsub = 44 ;
		#endif

		codePrint ("\t\tSUB RSP, %d\n", RSPsub) ;

		#if defined __linux__ || defined _WIN64
			codePrint ("\t\tCALL malloc\n") ;
		#endif
		#ifdef __MACH__
			codePrint ("\t\tCALL _malloc\n") ;
		#endif

		codePrint ("\t\tADD RSP, %d\n", RSPsub) ;

		codePrint ("\t\tMOV [RBP%s], RAX\n", getOffsetStr(vst->offset)) ;
		codePrint ("\t\tPOP SI\n") ;
		codePrint ("\t\tMOV [RBP%s], SI\n", getOffsetStr(vst->offset - 10)) ;
		codePrint ("\t\tPOP DI\n") ;
		codePrint ("\t\tMOV [RBP%s], DI\n\n", getOffsetStr(vst->offset - 8)) ;
	}

	RSPRestore () ;
}

/*	---------------------------------- SWITCH CASE ------------------------------------- */

int getCaseCount (astNode *statementsNode)
{
	int cnt = 0 ;

	while (statementsNode != NULL)
	{
		cnt++ ;

		if (statementsNode->next != NULL)
		{
			if (statementsNode->next->id == TK_DEFAULT)
				return cnt ;

			statementsNode = statementsNode->next->next->next ;
		}
		else
			statementsNode = NULL ;
	}

	return cnt ;
}


void switchDeclareVars (astNode *statementsNode)
{
	astNode *statementNode ;

	// Reserving space for switch scope variables
	while (statementsNode != NULL)
	{
		statementNode = statementsNode->child ;
		while (statementNode != NULL)
		{
			if (statementNode->child->id == TK_DECLARE)
			{
				moduleGeneration (statementNode->child, statementsNode->localST) ;
				statementNode->child->parent = statementNode->child ;		// tieing a knot
			}
			statementNode = statementNode->next ;
		}

		if (statementsNode->next != NULL)
		{
			if (statementsNode->next->id == TK_DEFAULT)
			{
				statementsNode = statementsNode->next->next ;
				statementNode = statementsNode->child ;
				while (statementNode != NULL)
				{
					if (statementNode->child->id == TK_DECLARE)
					{
						moduleGeneration (statementNode->child, statementsNode->localST) ;
						statementNode->child->parent = statementNode->child ;		// tieing a knot
					}
					statementNode = statementNode->next ;
				}

				break ;
			}
			statementsNode = statementsNode->next->next->next ;
		}
		else
			statementsNode = NULL ;
	}
}

int switchCaseLabels (astNode *node, moduleST *lst, int caseCount , int *caseLabels)
{
	int i, def_label = -1 ;

	for (i = 0 ; i < caseCount ; i++)
		caseLabels[i] = get_label (LABEL_SWITCH) ;

	varST *switchVar = searchVar (lst, node->next->tok->lexeme) ;
	codePrint ("\n\t\tMOV AX, [RBP%s]", getOffsetStr(switchVar->offset)) ;
	codeComment (10, "loading switch variable") ;

	astNode *caseValNode =  node->next->next->next ;
	i = 0 ;

	if (switchVar->datatype == TK_INTEGER)
	{
		while (caseValNode != NULL)
		{
			codePrint ("\n\t\tCMP AX, %s", caseValNode->tok->lexeme) ;
			codeComment (11, "switch case") ;
			codePrint ("\t\tJE @SWITCH%d\n", caseLabels[i]) ;

			if (caseValNode->next->next->next->id == statements)
			{
				def_label = get_label (LABEL_SWITCH) ;
				codePrint ("\n\t\tJMP @SWITCH%d", def_label) ;
				codeComment (11, "default case") ;
				break ;
			}

			i++ ;
			caseValNode = caseValNode->next->next->next ;
		}
	}
	else
	{		
		codePrint ("\n\t\tCMP AX, 0\n") ;
		if (caseValNode->tok->lexeme[0] == 't')
		{
			codePrint ("\t\tJNE @SWITCH%d", caseLabels[0]) ;
			codeComment (11, "true case") ;
		}
		else
		{
			codePrint ("\t\tJE @SWITCH%d", caseLabels[0]) ;
			codeComment (11, "false case") ;
		}

		codePrint ("\t\tJMP @SWITCH%d", caseLabels[1]) ;
		if (caseValNode->tok->lexeme[0] == 't')
			codeComment (11, "false case") ;
		else
			codeComment (11, "true case") ;
	}

	return def_label ;
}

void pushInputDynamicArr (varST *vst, varSTentry *varEntry, char *reg, pushArrLim flag)
{
	int lab, sub ;

	if (flag == LEFT)
		sub = 10 ;
	else
		sub = 8 ;

	codePrint ("\t\tMOV %s, [RBP%s]\n", reg, getOffsetStr(vst->offset - sub)) ;

	if (isLeftLimStatic(varEntry->thisVarST) && !isLeftLimStatic(vst))
	{
		lab = get_label (LABEL_OTHER) ;
		tf |= 1 << argLimERROR ;

		if (flag == LEFT)
			codePrint ("\t\tMOV CX, %s\n", varEntry->thisVarST->arrayIndices->tokLeft->lexeme) ;
		else
			codePrint ("\t\tMOV CX, %s\n", varEntry->thisVarST->arrayIndices->tokRight->lexeme) ;

		codePrint ("\t\tCMP %s, CX\n", reg) ;
		codePrint ("\t\tJE @LABEL%d\n", lab) ;
		codePrint ("\t\tCALL @argLimERROR\n") ;
		codePrint ("\n\t@LABEL%d:\n", lab) ; 
	}

	if (!isVarStaticArr (varEntry->thisVarST))
		codePrint ("\t\tPUSH %s\n", reg) ;
}


void pushInput (astNode *inputEnd, varSTentry *varEntry, moduleST *lst)
{
	varST *searchedVar ;

	while (varEntry->thisVarST->varType == VAR_PLACEHOLDER)
		varEntry = varEntry->next ;

	while (inputEnd != NULL)
	{
		searchedVar = searchVar (lst, inputEnd->tok->lexeme) ;

		if (searchedVar->datatype == TK_INTEGER || searchedVar->datatype == TK_BOOLEAN)
		{
			codePrint ("\t\tMOV AX, [RBP%s]\n", getOffsetStr(searchedVar->offset)) ;
			codePrint ("\t\tPUSH AX\n") ;
		}
		else if (isVarStaticArr (searchedVar))
		{
			loadArrBase (searchedVar, "RAX") ;
			codePrint ("\t\tPUSH RAX\n") ;
		}
		else
		{
			pushInputDynamicArr (searchedVar, varEntry, "AX", LEFT) ;
			pushInputDynamicArr (searchedVar, varEntry, "BX", RIGHT) ;
			loadArrBase (searchedVar, "RAX") ;
			codePrint ("\t\tPUSH RAX\n") ;
		}

		inputEnd = inputEnd->prev ;
		varEntry = varEntry->next ;
		while (varEntry != NULL && varEntry->thisVarST->varType == VAR_PLACEHOLDER)
			varEntry = varEntry->next ;
	}
}

void popOutput (astNode *outputHead, moduleST *lst)
{
	varST* searchedVar ;

	while (outputHead != NULL)
	{
		searchedVar = searchVar (lst, outputHead->tok->lexeme) ;
		codePrint ("\t\tPOP AX\n") ;
		codePrint ("\t\tMOV [RBP%s], AX\n", getOffsetStr(searchedVar->offset)) ;

		outputHead = outputHead->next ;
	}

	codePrint ("\n") ;
}

void retModule (moduleST *lst)
{
	codePrint ("\n\t\tMOV RSP, RBP\n") ;
	codePrint ("\t\tPOP RBP\n") ;

	if (lst == realBase->driverST)		// driver module
	{
		#ifdef __linux__
			codePrint ("\t\tMOV RAX, 60\n") ;
			codePrint ("\t\tXOR RDI, RDI\n") ;
			codePrint ("\t\tsyscall\n") ;
		#endif
			
		#ifdef __MACH__
			codePrint ("\t\tMOV RAX, 0x2000001\n") ;
			codePrint ("\t\tXOR RDI, RDI\n") ;
			codePrint ("\t\tsyscall\n") ;
		#endif

		#ifdef _WIN64
			codePrint ("\t\tret\n") ;
		#endif
	}
	else
		codePrint ("\t\tret\n") ;

	printCommentLineNASM () ;
}

void freeDynamic (varSTentry *varEntry)
{
	if (varEntry == NULL)
		return ;

	char freeReg[5] ;
	RSPAlign () ;

	#if defined __linux__ || defined __MACH__
		strcpy (freeReg, "RDI") ;
	#endif
	#ifdef _WIN64
		strcpy (freeReg, "RCX") ;
		codePrint ("\n\t\tSUB RSP, 32\n") ;
	#endif

	while (varEntry != NULL)
	{
		codePrint ("\n\t\tMOV %s, [RBP%s]\n", freeReg, getOffsetStr (varEntry->thisVarST->offset)) ;

		#if defined __linux__ || defined _WIN64
			codePrint ("\t\tCALL free") ;
		#endif
		#ifdef __MACH__ 
			codePrint ("\t\tCALL _free") ;
		#endif
		codeComment (8, "freeing local dynamic arrays\n") ;

		varEntry = varEntry->next ;
	}

	#ifdef _WIN64
		codePrint ("\t\tADD RSP, 32\n") ;
	#endif

	RSPRestore () ;
}

/*
Carried over convention from symbolTable.c
Module refers to not only a module, but any scope
*/
void moduleGeneration (astNode *node, moduleST *lst)
{
	if (node == NULL)
		return ;

	varST *searchedVar ;
	astNode *idListHead ;
	moduleST *calledModule ;
	varSTentry *varEntry ;

	switch (node->id)
	{
		int start_label, end_label ;
		int reserveLabel[10] ;

		case statements :
			if (node->child == NULL && node->localST->parent == realBase)
				retModule (node->localST) ;
			else
				moduleGeneration (node->child, node->localST);		// Access local scope and move below

			break ;

		case statement :
		{
			moduleGeneration(node->child, lst);
			if (node->next == NULL)
			{
				if (lst->tableType == SWITCH_ST)
					break ;

				varEntry = lst->dynamicVars[0] ;
				freeDynamic (varEntry) ;

				if (lst->parent == realBase)
					retModule (lst) ;
				else if (lst->scopeSize > 0)
				{
					codePrint ("\n\t\tADD RSP, %d", lst->scopeSize) ;
					codeComment (10, "restoring to parent scope") ;
				}

			}
			else
				moduleGeneration(node->next, lst);

			break ;
		}

		case TK_DECLARE :
		{
			;
			switchDeclareStatus stat ;
			if (lst->tableType != SWITCH_ST)
				stat = NOT_SWITCH ; // Subtract RSP, and allocate memory if dynamic
			else if (node->parent != node)
				stat = SWITCH_INIT ; // Subtract RSP only, and do not allocate memory for dynamic
			else
			{
				stat = SWITCH_GEN ; // Do not subtract RSP, allocate memory if dynamic AND restore parent
				node->parent = node->next->parent ;
			}

			int declCount = 1 ;
			idListHead = node->next->child ;
			astNode *dtNode = node->next->next ;

			while (idListHead->next != NULL)
			{
				declCount++ ;
				idListHead = idListHead->next ;
			}

			searchedVar = searchVar (lst, idListHead->tok->lexeme) ;
			if (searchedVar->size > 0 && stat != SWITCH_GEN)
			{	
				codePrint ("\t\tSUB RSP, %d", searchedVar->size * declCount) ;
				if (stat == NOT_SWITCH)
					codeComment (11, "making space for declaration") ;
				else if (stat == SWITCH_INIT)
					codeComment (11, "declaring before switch") ;

				codePrint ("\n") ;
			}

			if (dtNode->dtTag == ARRAY && !isVarStaticArr(searchedVar) && stat != SWITCH_GEN)
				dynamicDeclaration (lst, searchedVar, declCount) ;

			break ;	
		}									

		case TK_ASSIGNOP :
		{
			if (isSingleRHS(node))
			{
				if (isArrayRHS (node, lst))
				{
					//printf ("CANNOT HANDLE ARRAY ASSIGNMENT YET\n") ;
					varST *lhs, *rhs ;
					lhs = searchVar (lst, node->child->tok->lexeme) ;
					rhs = searchVar (lst, node->child->next->tok->lexeme) ;
				
					if (isVarStaticArr (lhs) && isVarStaticArr (rhs))
					{
						tf |= 1 << asgnArr ;

						loadRegLeftLim (lhs, "CX") ;
						loadRegRightLim (lhs, "DX") ;
						loadArrBase (lhs, "RDI") ;
						loadArrBase (rhs, "RSI") ;
						codePrint ("\t\tSUB DX, CX\n") ;
						codePrint ("\t\tADD DX, 1\n") ;
						codePrint ("\t\tADD DX, DX\n") ;
						codePrint ("\t\tCALL @asgnArr\n") ;
					}
					else
						dynAsgnArr (lhs, rhs) ;
				}
				else
				{
					expr(node->child->next, lst, 1);
					exprAssign (node->child, lst, 1) ;
				}
			}
			else
			{
				int singleAssign = 0 ;

				if (isAssignUnary (node))
				{
					if (isExprLeaf (node->child->next->child->id))
						singleAssign = 1 ;

					expr(node->child->next->child, lst, singleAssign) ;
				}
				else
					expr(node->child->next, lst, 0);	

				exprAssign (node->child, lst, singleAssign) ;
			}

			codePrint ("\n") ;
			break ;
		}

		case TK_FOR :
		{
			node=node->next;
			start_label = get_label (LABEL_FOR) ;
			end_label = get_label (LABEL_FOR) ;

			searchedVar = searchVar (lst, node->tok->lexeme) ;

			codePrint ("\t\tMOV CX,%s\n", node->next->tok->lexeme);
			codePrint ("\t\tMOV [RBP%s], CX" , getOffsetStr(searchedVar->offset)) ;

			codePrint ("\n\t@FOR%d:\n", start_label) ;
			codePrint ("\t\tMOV AX, %s\n", node->next->next->tok->lexeme) ;
			codePrint ("\t\tCMP CX, AX\n");
			codePrint ("\t\tJG @FOR%d\n\n", end_label);

			moduleGeneration(node->next->next->next, lst);		// Statements

			codePrint ("\n\t\tMOV CX, [RBP%s]", getOffsetStr(searchedVar->offset)) ;
			codeComment (8, "For loop increment") ;
			codePrint ("\t\tINC CX\n");
			codePrint ("\t\tMOV [RBP%s],CX\n", getOffsetStr(searchedVar->offset)) ;
			codePrint ("\t\tJMP @FOR%d\n", start_label) ;
			codePrint ("\n\t@FOR%d:\n", end_label) ;
			break ;
		}

		
		case TK_WHILE :
		{
			node=node->next;
			start_label = get_label(LABEL_WHILE);
			end_label =  get_label(LABEL_WHILE);

			codePrint ("\n\t@WHILE%d:\n", start_label) ;

			expr (node, lst, 0);	// expr

			codePrint ("\t\tPOP AX\n");
			codePrint ("\t\tCMP AX, 0");
			codeComment (11, "checking while loop condition") ;
			codePrint ("\t\tJE @WHILE%d\n\n", end_label) ;

			moduleGeneration(node->next, lst);		// statements

			codePrint ("\t\tJMP @WHILE%d\n", start_label) ;
			codePrint ("\n\t@WHILE%d:\n", end_label) ;

			break ;
		}
		
		case TK_PRINT :
			node = node->next ;
			print (node, lst) ;
			break ;

		case TK_GET_VALUE :
			searchedVar = searchVar (lst, node->next->tok->lexeme) ;
			getValue (lst, searchedVar) ;
			break ;

		case TK_SWITCH :
		{
			int i, caseCount, def_label ;
			int *caseLabels ;
			astNode *statementsNode = node->next->next->next->next ;

			switchDeclareVars (node->next->next->next->next) ;
			caseCount = getCaseCount (node->next->next->next->next) ;
			caseLabels = (int *) malloc (sizeof(int) * caseCount) ;
			def_label = switchCaseLabels (node , lst, caseCount, caseLabels) ;

			end_label = get_label (LABEL_SWITCH) ;
			i = 0 ;
			while (statementsNode != NULL)
			{
				codePrint ("\n@SWITCH%d:\n", caseLabels[i]) ;
				moduleGeneration (statementsNode, lst) ;
				codePrint ("\n\t\tJMP @SWITCH%d\n", end_label) ;

				i++ ;
				if (statementsNode->next != NULL)
				{
					if (statementsNode->next->id == TK_DEFAULT)
					{
						statementsNode = statementsNode->next->next ;
						codePrint ("\n@SWITCH%d:\n", def_label) ;
						moduleGeneration (statementsNode, lst) ;
						codePrint ("\n\t\tJMP @SWITCH%d\n", end_label) ;

						break ;
					}
					statementsNode = statementsNode->next->next->next ;
				}
				else
					statementsNode = NULL ;
			}

			statementsNode = node->next->next->next->next ;
			codePrint ("\n@SWITCH%d:\n", end_label) ;

			varEntry = statementsNode->localST->dynamicVars[0] ;
			freeDynamic (varEntry) ;

			if (statementsNode->localST->scopeSize > 0)
			{
				codePrint ("\t\tADD RSP, %d", statementsNode->localST->scopeSize) ;
				codeComment (11, "restoring to parent scope") ;
			}

			break ;
		}

		case TK_ID :
		{
			calledModule = searchModuleInbaseST (node->tok->lexeme) ;
			varEntry = calledModule->inputVars[0] ;

			idListHead = node->next->child ; 
			while (idListHead->next != NULL)
				idListHead = idListHead->next ;

			pushInput (idListHead, varEntry, lst) ;

			codePrint ("\t\tCALL %s", strcmp(node->tok->lexeme, "main") ? node->tok->lexeme : "@main") ;
			codeComment (11, "calling user function") ;
			codePrint ("\t\tADD RSP, %d\n", calledModule->inputSize) ;
			break ;
		}

		case idList :
		{
			calledModule = searchModuleInbaseST (node->next->next->tok->lexeme) ;
			varEntry = calledModule->inputVars[0] ;

			idListHead = node->next->next->next->child ;
			while (idListHead->next != NULL)
				idListHead = idListHead->next ;

			codePrint ("\t\tSUB RSP, %d\n", calledModule->outputSize) ;
			pushInput (idListHead, varEntry, lst) ;

			codePrint ("\t\tCALL %s", node->next->next->tok->lexeme) ;
			codeComment (11, "calling user function") ;
			codePrint ("\t\tADD RSP, %d\n", calledModule->inputSize) ;

			popOutput (node->child, lst) ;
			break ;
		}
	}

}

void codeGeneration(astNode *node)
{
	if (node == NULL)
		return;

	switch (node->id)
	{
		case program :
			codeGeneration (node->child->next) ;		// <otherModules>
			break ;

		case otherModules :
			codeGeneration (node->child) ;					// Do module definitions
			codeGeneration (node->next) ;					// <driverModule> or NULL
			break ;

		case module :
			codePrint ("\n%s:\n", strcmp(node->child->tok->lexeme , "main") ? node->child->tok->lexeme : "@main") ;
			codePrint ("\t\tPUSH RBP\n") ;
			codePrint ("\t\tMOV RBP, RSP\n\n") ;

			moduleGeneration (node->child->next->next->next, NULL) ;
			codeGeneration (node->next) ;					// Moving on to the next module

			break ;

		case driverModule :
			#ifdef __linux__
				codePrint ("\nmain:\n") ;
			#endif

			#ifdef __MACH__
				codePrint ("\n_main:\n") ;
			#endif

			#ifdef _WIN64
				codePrint ("\nWinMain:\n") ;
			#endif

			codePrint ("\t\tPUSH RBP\n") ;
			codePrint ("\t\tMOV RBP, RSP\n\n") ;

			moduleGeneration(node->child, NULL); 				// <statements>
			codeGeneration(node->next); 				// Move to the next child of program

			break ;
	}
}

void preamble()
{
	codePrint (";;;;;;;;;;; Compiled for 64-bit ") ;

	#ifdef __linux__
		codePrint ("Linux ") ;
	#endif
	#ifdef __MACH__
		codePrint ("macOS ") ;
	#endif
	#ifdef _WIN64
		codePrint ("Windows ") ;
	#endif

	codePrint (";;;;;;;;;;;\n\n") ;

	#if defined __linux__ || defined _WIN64
		codePrint ("extern printf\n") ;
		codePrint ("extern scanf\n") ;
		codePrint ("extern malloc\n") ;
		codePrint ("extern free\n") ;
		codePrint ("extern exit\n") ;

		#ifdef __linux__
			codePrint ("\nglobal main\n") ;
		#endif

		#ifdef _WIN64
			codePrint ("\nglobal WinMain\n") ;
		#endif
	#endif

	#ifdef __MACH__
		codePrint ("extern _printf\n") ;
		codePrint ("extern _scanf\n") ;
		codePrint ("extern _malloc\n") ;
		codePrint ("extern _free\n") ;
		codePrint ("extern _exit\n") ;

		codePrint ("\nglobal _main\n") ;
	#endif

	codePrint ("section .text\n") ;
}


int isFlagSet (int flagInt , int id)
{
	int test = flagInt ;
	test = test >> id ;

	return test % 2 ? 1 : 0 ;
}

void errorTF (char *errStr)
{
	RSPAlign () ;

	#if defined __linux__ || defined __MACH__
		codePrint ("\t\tMOV RDI, %s\n", errStr) ;
		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
	#endif
	#ifdef _WIN64
		codePrint ("\t\tMOV RCX, %s\n", errStr) ;
		codePrint ("\t\tXOR RDX, RDX\n") ;
		codePrint ("\t\tSUB RSP, 32\n") ;
	#endif

	#if defined __linux__ || defined _WIN64
		codePrint ("\t\tCALL printf\n") ;
		#ifdef _WIN64
			codePrint ("\t\tADD RSP, 32\n") ;
		#endif
	#endif
	#ifdef __MACH__
		codePrint ("\t\tCALL _printf\n") ;
	#endif

	#if defined __linux__ || defined __MACH__
		codePrint ("\t\tMOV RDI, 1\n") ;
	#endif
	#ifdef _WIN64
		codePrint ("\t\tMOV RCX, 1\n") ;
	#endif

	#if defined __linux__ || defined _WIN64
		codePrint ("\t\tCALL exit\n") ;
	#endif
	#ifdef __MACH__
		codePrint ("\t\tCALL _exit\n") ;
	#endif
}

void printArrStart ()
{
	codePrint ("\t\tPUSH RDI\n") ;

	#if defined __linux__ || defined __MACH__
		codePrint ("\t\tMOV RDI, @printFormatArray\n") ;
		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
		codePrint ("\t\tPUSH AX\n") ;
		codePrint ("\t\tPUSH BX\n") ;
		codePrint ("\t\tPUSH CX\n") ;
		codePrint ("\t\tPUSH DX\n") ;
	#endif
	#ifdef _WIN64
		codePrint ("\t\tMOV RCX, @printFormatArray\n") ;
		codePrint ("\t\tXOR RDX, RDX\n") ;
		codePrint ("\t\tPUSH BX\n") ;
		codePrint ("\t\tSUB RSP, 38\n") ;
	#endif

	#if defined __linux__ || defined _WIN64
		codePrint ("\t\tCALL printf\n") ;
		#ifdef _WIN64
			codePrint ("\t\tADD RSP, 38\n") ;
		#endif
	#endif
	#ifdef __MACH__
		codePrint ("\t\tCALL _printf\n") ;
	#endif

	#if defined __linux__ || defined __MACH__
		codePrint ("\t\tPOP DX\n") ;
		codePrint ("\t\tPOP CX\n") ;
		codePrint ("\t\tPOP BX\n") ;
		codePrint ("\t\tPOP AX\n") ;
		codePrint ("\t\tPOP RDI\n\n") ;
		codePrint ("\t\tMOV CX, 0\n") ;
	#endif
	#ifdef _WIN64
		codePrint ("\t\tPOP BX\n") ;
		codePrint ("\t\tPOP RDI\n\n") ;
		codePrint ("\t\tMOV AX, 0\n") ;
	#endif

	codePrint ("\n\t.printArr:") ;
	codeComment (8, "printing array") ;
}

void printArrEnd ()
{
	#if defined __linux__ || defined _WIN64
		codePrint ("\t\tCALL printf\n") ;
		#ifdef _WIN64
			codePrint ("\t\tADD RSP, 36\n") ;
		#endif
	#endif
	#ifdef __MACH__
		codePrint ("\t\tCALL _printf\n") ;
	#endif

	#if defined __linux__ || defined __MACH__
		codePrint ("\t\tPOP DX\n") ;
		codePrint ("\t\tPOP CX\n") ;
		codePrint ("\t\tPOP BX\n") ;
		codePrint ("\t\tPOP AX\n") ;
		codePrint ("\t\tPOP RDI\n\n") ;
		codePrint ("\t\tADD CX, 2\n") ;
		codePrint ("\t\tCMP CX, DX\n") ;
	#endif
	#ifdef _WIN64
		codePrint ("\t\tPOP BX\n") ;
		codePrint ("\t\tPOP AX\n") ;
		codePrint ("\t\tPOP RDI\n\n") ;
		codePrint ("\t\tADD AX, 2\n") ;
		codePrint ("\t\tCMP AX, BX\n") ;
	#endif

	codePrint ("\t\tJNE .printArr\n\n") ;

	#if defined __linux__ || defined __MACH__
		codePrint ("\t\tMOV RDI, @printNewLine\n") ;
		codePrint ("\t\tXOR RSI, RSI\n") ;
		codePrint ("\t\tXOR RAX, RAX\n") ;
	#endif
	#ifdef _WIN64
		codePrint ("\t\tMOV RCX, @printNewLine\n") ;
		codePrint ("\t\tXOR RDX, RDX\n") ;
		codePrint ("\t\tSUB RSP, 32\n") ;
	#endif

	#if defined __linux__ || defined _WIN64
		codePrint ("\t\tCALL printf\n") ;
		#ifdef _WIN64
			codePrint ("\t\tADD RSP, 32\n") ;
		#endif
	#endif
	#ifdef __MACH__
		codePrint ("\t\tCALL _printf\n") ;
	#endif

	RSPRestore () ;
	codePrint ("\n\t\tret\n") ;
}


void postamble()
{
	if (isFlagSet (tf, boundCheck))
	{
		tf |= 1 << boundERROR ;

		codePrint ("\n@boundCheck:\n") ;
		codePrint ("\t\tCMP CX, BX\n") ;
		codePrint ("\t\tJGE .leftLim\n") ;
		codePrint ("\t\tCALL @boundERROR\n") ;

		codePrint ("\n\t.leftLim:\n") ;
		codePrint ("\t\tCMP DX, CX\n") ;
		codePrint ("\t\tJGE .rightLim\n") ;
		codePrint ("\t\tCALL @boundERROR\n") ;

		codePrint ("\n\t.rightLim:\n") ;
		codePrint ("\t\tSUB CX, BX\n") ;
		codePrint ("\t\tADD CX, CX\n") ;
		codePrint ("\t\tMOVSX RBX, CX\n") ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf, dynamicDeclCheck))
	{
		tf |= 1 << declNegERROR ;
		tf |= 1 << declERROR ;

		codePrint ("\n@dynamicDeclCheck:\n") ;
		codePrint ("\t\tCMP AX, 0\n") ;
		codePrint ("\t\tJGE .leftNotNeg\n") ;
		codePrint ("\t\tCALL @declNegERROR\n") ;

		codePrint ("\n\t.leftNotNeg:\n") ;
		codePrint ("\t\tCMP BX, 0\n") ;
		codePrint ("\t\tJGE .rightNotNeg\n") ;
		codePrint ("\t\tCALL @declNegERROR\n") ;

		codePrint ("\n\t.rightNotNeg:\n") ;
		codePrint ("\t\tCMP BX, AX\n") ;
		codePrint ("\t\tJGE .dynChecked\n") ;
		codePrint ("\t\tCALL @declERROR\n") ;
		codePrint ("\n\t.dynChecked:\n") ;

		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tMOV DX, BX\n") ;
			codePrint ("\t\tSUB DX, AX\n") ;
			codePrint ("\t\tADD DX, 1\n") ;
			codePrint ("\t\tADD DX, DX\n") ;
			codePrint ("\t\tMOVSX RDI, DX\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tMOV CX, BX\n") ;
			codePrint ("\t\tSUB CX, AX\n") ;
			codePrint ("\t\tADD CX, 1\n") ;
			codePrint ("\t\tADD CX, CX\n") ;
			codePrint ("\t\tMOVSX RCX, CX\n") ;
		#endif

		codePrint ("\n\t\tret\n") ;
	}

	/* ----------------------------------- Errors ------------------------------------ */

	if (isFlagSet (tf, boundERROR))
	{
		df |= 1 << boundPrint ;

		codePrint ("\n@boundERROR:\n") ;
		errorTF ("@boundPrint") ;
	}

	if (isFlagSet (tf, declERROR))
	{
		df |= 1 << declPrint ;

		codePrint ("\n@declERROR:\n") ;
		errorTF ("@declPrint") ;
	}

	if (isFlagSet (tf, declNegERROR))
	{
		df |= 1 << declNeg ;

		codePrint ("\n@declNegERROR:\n") ;
		errorTF ("@declNeg") ;
	}

	if (isFlagSet (tf, argLimERROR))
	{
		df |= 1 << arrArgMismatch ;
		errorTF ("@arrArgMismatch") ;
	}

	if (isFlagSet (tf, asgnLimERROR))
	{
		df |= 1 << asgnArgMismatch ;

		codePrint ("\n@asgnLimERROR:\n") ;
		errorTF ("@asgnArgMismatch") ;
	}

	if (isFlagSet (tf, divZeroERROR))
	{
		df |= 1 << divZero ;

		codePrint ("\n@divZeroERROR:\n") ;
		errorTF ("@divZero") ;
	}

	/* ----------------------------------- Helper Functions ------------------------------------ */

	if (isFlagSet (tf, getValuePrimitive))
	{
		codePrint ("\n@getValuePrimitive:\n") ;
		RSPAlign () ;

		#ifdef _WIN64
			codePrint ("\t\tXOR RDX, RDX\n") ;
			codePrint ("\t\tPUSH RBX\n") ;
			codePrint ("\t\tSUB RSP, 40\n") ;
		#endif
		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tXOR RSI, RSI\n") ;
			codePrint ("\t\tXOR RAX, RAX\n") ;
			codePrint ("\t\tPUSH RBX\n") ;
			codePrint ("\t\tPUSH RCX\n") ;
		#endif

		#if defined __linux__ || defined _WIN64
			codePrint ("\t\tCALL printf\n") ;
			#ifdef _WIN64
				codePrint ("\t\tADD RSP, 40\n") ;
			#endif
		#endif

		#ifdef __MACH__
			codePrint ("\t\tCALL _printf\n") ;
		#endif

		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tPOP RCX\n") ;
			codePrint ("\t\tPOP RBX\n\n") ;
			codePrint ("\t\tMOV RDI, @inputInt") ;
			codeComment (10, "get_value") ;
			codePrint ("\t\tMOV RSI, RSP\n") ;
			codePrint ("\t\tSUB RSI, 16\n") ;
			codePrint ("\t\tPUSH RBX\n") ;
			codePrint ("\t\tPUSH RSI\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tPOP RBX\n") ;
			codePrint ("\t\tMOV RCX, @inputInt") ;
			codeComment (10, "get_value") ;
			codePrint ("\t\tMOV RDX, RSP\n") ;
			codePrint ("\t\tSUB RDX, 16\n") ;
			codePrint ("\t\tPUSH RBX\n") ;
			codePrint ("\t\tSUB RSP, 40\n") ;
		#endif

		#if defined __linux__ || defined _WIN64
			codePrint ("\t\tCALL scanf\n") ;
			#ifdef _WIN64
				codePrint ("\t\tADD RSP, 40\n") ;
			#endif
		#endif

		#ifdef __MACH__
			codePrint ("\t\tCALL _scanf\n") ;
		#endif

		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tPOP RSI\n") ;
		#endif

		codePrint ("\t\tPOP RBX\n") ;
		codePrint ("\t\tMOV AX, [RSP - 16]\n") ;
		codePrint ("\t\tMOV [RBP + RBX], AX\n") ;

		RSPRestore () ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf, printGetArrPrompt))
	{
		df |= 1 << leftRange ;
		df |= 1 << rightRange ;

		codePrint ("\n@printGetArrPrompt:\n") ;
		RSPAlign () ;

		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tMOV SI, CX\n") ;
			codePrint ("\t\tSUB SI, BX\n") ;
			codePrint ("\t\tADD SI, 1\n") ; 
			codePrint ("\t\tMOVSX RSI, SI\n") ;
			codePrint ("\t\tXOR RAX, RAX\n") ;
			codePrint ("\t\tPUSH RSI\n") ;
			codePrint ("\t\tPUSH AX\n") ;
			codePrint ("\t\tPUSH BX\n") ;
			codePrint ("\t\tPUSH CX\n") ;
			codePrint ("\t\tPUSH DX\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tMOV DX, DI\n") ;
			codePrint ("\t\tSUB DX, SI\n") ;
			codePrint ("\t\tADD DX, 1\n") ; 
			codePrint ("\t\tMOVSX RDX, DX\n") ;
			codePrint ("\t\tPUSH SI\n") ;
			codePrint ("\t\tPUSH DI\n") ;
			codePrint ("\t\tSUB RSP, 44\n") ;
		#endif


		#if defined __linux__ || defined _WIN64
			codePrint ("\t\tCALL printf\n") ;
			#ifdef _WIN64
				codePrint ("\t\tADD RSP, 44\n") ;
			#endif
		#endif
		#ifdef __MACH__
			codePrint ("\t\tCALL _printf\n") ;
		#endif

		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tPOP DX\n") ;
			codePrint ("\t\tPOP CX\n") ;
			codePrint ("\t\tPOP BX\n") ;
			codePrint ("\t\tPOP AX\n") ;
			codePrint ("\t\tPOP RSI\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tPOP DI\n") ;
			codePrint ("\t\tPOP SI\n") ;
		#endif

		#if defined __linux__ || defined __MACH__
			codePrint ("\n\t\tMOV RDI, @leftRange\n") ;
			codePrint ("\t\tMOVSX RSI, BX\n") ;
			codePrint ("\t\tXOR RAX, RAX\n") ;
			codePrint ("\t\tPUSH RSI\n") ;
			codePrint ("\t\tPUSH AX\n") ;
			codePrint ("\t\tPUSH BX\n") ;
			codePrint ("\t\tPUSH CX\n") ;
			codePrint ("\t\tPUSH DX\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\n\t\tMOV RCX, @leftRange\n") ;
			codePrint ("\t\tMOVSX RDX, SI\n") ;
			codePrint ("\t\tPUSH SI\n") ;
			codePrint ("\t\tPUSH DI\n") ;
			codePrint ("\t\tSUB RSP, 44\n") ;
		#endif


		#if defined __linux__ || defined _WIN64
			codePrint ("\t\tCALL printf\n") ;
			#ifdef _WIN64
				codePrint ("\t\tADD RSP, 44\n") ;
			#endif
		#endif
		#ifdef __MACH__
			codePrint ("\t\tCALL _printf\n") ;
		#endif


		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tPOP DX\n") ;
			codePrint ("\t\tPOP CX\n") ;
			codePrint ("\t\tPOP BX\n") ;
			codePrint ("\t\tPOP AX\n") ;
			codePrint ("\t\tPOP RSI\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tPOP DI\n") ;
			codePrint ("\t\tPOP SI\n") ;
		#endif


		#if defined __linux__ || defined __MACH__
			codePrint ("\n\t\tMOV RDI, @rightRange\n") ;
			codePrint ("\t\tMOVSX RSI, CX\n") ;
			codePrint ("\t\tXOR RAX, RAX\n") ;
			codePrint ("\t\tPUSH RSI\n") ;
			codePrint ("\t\tPUSH AX\n") ;
			codePrint ("\t\tPUSH BX\n") ;
			codePrint ("\t\tPUSH CX\n") ;
			codePrint ("\t\tPUSH DX\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\n\t\tMOV RCX, @rightRange\n") ;
			codePrint ("\t\tMOVSX RDX, DI\n") ;
			codePrint ("\t\tPUSH SI\n") ;
			codePrint ("\t\tPUSH DI\n") ;
			codePrint ("\t\tSUB RSP, 44\n") ;
		#endif

		#if defined __linux__ || defined _WIN64
			codePrint ("\t\tCALL printf\n") ;
			#ifdef _WIN64
				codePrint ("\t\tADD RSP, 44\n") ;
			#endif
		#endif
		#ifdef __MACH__
			codePrint ("\t\tCALL _printf\n") ;
		#endif


		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tPOP DX\n") ;
			codePrint ("\t\tPOP CX\n") ;
			codePrint ("\t\tPOP BX\n") ;
			codePrint ("\t\tPOP AX\n") ;
			codePrint ("\t\tPOP RSI\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tPOP DI\n") ;
			codePrint ("\t\tPOP SI\n") ;
		#endif

		RSPRestore () ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf, getArr))
	{
		codePrint ("\n@getArr:\n");
		RSPAlign  () ;

		codePrint ("\n\t\tPUSH RDI\n") ;

		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tMOV RCX, 0\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tMOV RAX, 0\n") ;
		#endif

		codePrint ("\n\t.getArrLoop:") ;
		codeComment (8, "getting array") ;

		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tMOV RDI, @inputInt\n") ;
			codePrint ("\t\tMOV RSI, RSP\n") ;
			codePrint ("\t\tSUB RSI, 24\n") ;
			codePrint ("\t\tPUSH RCX\n") ;
			codePrint ("\t\tPUSH RDX\n") ;
			codePrint ("\t\tPUSH RSI\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tMOV RCX, @inputInt\n") ;
			codePrint ("\t\tMOV RDX, RSP\n") ;
			codePrint ("\t\tSUB RDX, 24\n") ;
			codePrint ("\t\tPUSH RAX\n") ;
			codePrint ("\t\tPUSH RBX\n") ;
			codePrint ("\t\tSUB RSP, 48\n") ;
		#endif

		#if defined __linux__ || defined _WIN64
			codePrint ("\t\tCALL scanf\n") ;
			#ifdef _WIN64
				codePrint ("\t\tADD RSP, 48\n") ;
			#endif
		#endif
		#ifdef __MACH__
			codePrint ("\t\tCALL _scanf\n") ;
		#endif

		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tPOP RSI\n") ;
			codePrint ("\t\tPOP RDX\n") ;
			codePrint ("\t\tPOP RCX\n") ;
			codePrint ("\t\tMOV RBX, RCX\n") ;
			codePrint ("\t\tMOV AX, [RSP - 24]\n") ;
			codePrint ("\t\tPOP RDI\n") ;
			codePrint ("\t\tPUSH RDI\n") ;
			codePrint ("\t\tMOV [RDI + RBX], AX\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tPOP RBX\n") ;
			codePrint ("\t\tPOP RAX\n") ;
			codePrint ("\t\tMOV CX, [RSP - 24]\n") ;
			codePrint ("\t\tPOP RDI\n") ;
			codePrint ("\t\tPUSH RBX\n") ;
			codePrint ("\t\tMOV RBX, RAX\n") ;
			codePrint ("\t\tMOV [RDI + RBX], CX\n") ;
			codePrint ("\t\tPOP RBX\n") ;
			codePrint ("\t\tPUSH RDI\n") ;
		#endif

		#if defined __linux__ || defined __MACH__
			codePrint ("\n\t\tADD RCX, 2\n") ;
			codePrint ("\t\tCMP RCX, RDX\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tADD RAX, 2\n") ;
			codePrint ("\t\tCMP RAX, RBX\n") ;
		#endif
		codePrint ("\t\tJNE .getArrLoop\n\n") ;

		codePrint ("\t\tPOP RDI") ;
		RSPRestore () ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf, printInteger))
	{
		df |= 1 << printFormat ;

		codePrint ("\n@printInteger:\n") ;
		RSPAlign () ;

		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tMOV RDI, @printFormat\n") ;
			codePrint ("\t\tMOVSX RSI, SI\n") ;
			codePrint ("\t\tXOR RAX, RAX\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tMOV RCX, @printFormat\n") ;
			codePrint ("\t\tMOVSX RDX, DX\n") ;
			codePrint ("\t\tSUB RSP, 32\n") ;
		#endif

		#if defined __linux__ || defined _WIN64
			codePrint ("\t\tCALL printf\n") ;
			#ifdef _WIN64
				codePrint ("\t\tADD RSP, 32\n") ;
			#endif
		#endif
		#ifdef __MACH__
			codePrint ("\t\tCALL _printf\n") ;
		#endif

		RSPRestore () ;

		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf , printBoolean))
	{
		df |= 1 << printFalse ;
		df |= 1 << printTrue ;

		codePrint ("\n@printBoolean:\n") ;
		RSPAlign () ;

		char promptReg[5] ;
		#if defined __linux__ || defined __MACH__
			strcpy (promptReg, "RDI") ;
		#endif
		#ifdef _WIN64
			strcpy (promptReg, "RCX") ;
		#endif

		codePrint ("\t\tCMP SI, 0\n") ;
		codePrint ("\t\tJE .boolFalse\n") ;
		codePrint ("\t\tMOV %s, @printTrue\n", promptReg) ;
		codePrint ("\t\tJMP .boolPrint\n") ;

		codePrint ("\n\t.boolFalse:\n") ;
		codePrint ("\t\tMOV %s, @printFalse\n", promptReg) ;
		codePrint ("\n\t.boolPrint:\n") ;

		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tXOR RSI, RSI\n") ;
			codePrint ("\t\tXOR RAX, RAX\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tXOR RDX, RDX\n") ;
			codePrint ("\t\tSUB RSP, 32\n") ;
		#endif

		#if defined __linux__ || defined _WIN64
			codePrint ("\t\tCALL printf\n") ;
			#ifdef _WIN64
				codePrint ("\t\tADD RSP, 32\n") ;
			#endif
		#endif
		#ifdef __MACH__
			codePrint ("\t\tCALL _printf\n") ;
		#endif

		RSPRestore () ;
		codePrint ("\n\t\tret\n") ;
	}

	if (isFlagSet (tf, printIntegerArr))
	{
		df |= 1 << printFormatArray ;
		df |= 1 << printNewLine ;
		df |= 1 << printInt ;

		codePrint ("\n@printIntegerArr:\n") ;
		RSPAlign () ;
		printArrStart () ;

		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tPUSH RDI\n") ;
			codePrint ("\t\tMOVSX RBX, CX\n") ;
			codePrint ("\t\tMOV SI, [RDI + RBX]\n") ;
			codePrint ("\t\tMOV RDI, @printInt\n") ;
			codePrint ("\t\tMOVSX RSI, SI\n") ;
			codePrint ("\t\tXOR RAX, RAX\n") ;
			codePrint ("\t\tPUSH AX\n") ;
			codePrint ("\t\tPUSH BX\n") ;
			codePrint ("\t\tPUSH CX\n") ;
			codePrint ("\t\tPUSH DX\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tMOV RCX, @printInt\n") ;
			codePrint ("\t\tPUSH BX\n") ;
			codePrint ("\t\tMOVSX RBX, AX\n") ;
			codePrint ("\t\tMOV DX, [RDI + RBX]\n") ;
			codePrint ("\t\tMOVSX RDX, DX\n") ;
			codePrint ("\t\tPOP BX\n") ;
			codePrint ("\t\tPUSH RDI\n") ;
			codePrint ("\t\tPUSH AX\n") ;
			codePrint ("\t\tPUSH BX\n") ;
			codePrint ("\t\tSUB RSP, 36\n") ;
		#endif

		printArrEnd () ;
	}

	if (isFlagSet (tf, printBooleanArr))
	{
		df |= 1 << printFormatArray ;
		df |= 1 << DATA_true ;
		df |= 1 << DATA_false ;
		df |= 1 << printNewLine ;

		char cmpReg[3], boolReg[5] ;

		codePrint ("\n@printBooleanArr:\n") ;
		RSPAlign () ;
		printArrStart () ;

		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tPUSH RDI\n") ;
			codePrint ("\t\tMOVSX RBX, CX\n") ;
			codePrint ("\t\tMOV SI, [RDI + RBX]\n") ;

			strcpy (cmpReg, "SI") ;
			strcpy (boolReg, "RDI") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tPUSH BX\n") ;
			codePrint ("\t\tMOVSX RBX, AX\n") ;
			codePrint ("\t\tMOV DX, [RDI + RBX]\n") ;
			codePrint ("\t\tMOVSX RDX, DX\n") ;
			codePrint ("\t\tPOP BX\n") ;

			strcpy (cmpReg, "DX") ;
			strcpy (boolReg, "RCX") ;
		#endif

		codePrint ("\n\t\tCMP %s, 0\n", cmpReg) ;
		codePrint ("\t\tJE .arrFalse\n") ;
		codePrint ("\t\tMOV %s, @true\n", boolReg) ;
		codePrint ("\t\tJMP .afterBool\n") ;
		
		codePrint ("\n\t.arrFalse:\n") ;
		codePrint ("\t\tMOV %s, @false\n", boolReg) ;
		codePrint ("\n\t.afterBool:\n") ;

		#if defined __linux__ || defined __MACH__
			codePrint ("\t\tXOR RSI, RSI\n") ;
			codePrint ("\t\tXOR RAX, RAX\n") ;
			codePrint ("\t\tPUSH AX\n") ;
			codePrint ("\t\tPUSH BX\n") ;
			codePrint ("\t\tPUSH CX\n") ;
			codePrint ("\t\tPUSH DX\n") ;
		#endif
		#ifdef _WIN64
			codePrint ("\t\tXOR RDX, RDX\n") ;
			codePrint ("\t\tPUSH RDI\n") ;
			codePrint ("\t\tPUSH AX\n") ;
			codePrint ("\t\tPUSH BX\n") ;
			codePrint ("\t\tSUB RSP, 36\n") ;
		#endif

		printArrEnd () ;
	}

	if (isFlagSet (tf, asgnArr))
	{
		codePrint ("\n@asgnArr:\n") ;
		codePrint ("\t\tMOV CX, 0\n") ;

		codePrint ("\n\t.assnLoop:\n") ;
		codePrint ("\t\tMOVSX RBX, CX\n") ;
		codePrint ("\t\tMOV AX, [RSI + RBX]\n") ;
		codePrint ("\t\tMOV [RDI + RBX], AX\n") ;

		codePrint ("\n\t\tADD CX, 2\n") ;
		codePrint ("\t\tCMP CX, DX\n") ;
		codePrint ("\t\tJNE .assnLoop\n") ;

		codePrint ("\n\t\tret\n") ;
	}

	/* ----------------------------------------- String data ---------------------------------------------------- */
	printCommentLineNASM () ;

	if (df)
		codePrint ("\nsection .data\n") ;

	if (isFlagSet (df, boundPrint))
	{
		codePrint ("\t\t@boundPrint : ") ;
		#ifdef _WIN64
			codePrint ("db \"Runtime Error --> Array out of bounds. Halt!\" , 10, 0\n") ;	
		#endif
		#if defined __linux__ || defined __MACH__
			codePrint ("db \"\033[1m\033[31mRuntime Error \033[0m\033[1m--> \033[0mArray out of bounds. Halt!\" , 10, 0\n") ;	
		#endif			
	}
	
	if (isFlagSet (df, declPrint))
	{
		codePrint ("\t\t@declPrint : ") ;
		#ifdef _WIN64
			codePrint ("db \"Runtime Error --> Invalid order of bounds in dynamic array declaration. Halt!\" , 10, 0\n") ;		
		#endif
		#if defined __linux__ || defined __MACH__
			codePrint ("db \"\033[1m\033[31mRuntime Error \033[0m\033[1m--> \033[0mInvalid order of bounds in dynamic array declaration. Halt!\" , 10, 0\n") ;		
		#endif			
	}
	
	if (isFlagSet (df, declNeg))
	{
		codePrint ("\t\t@declNeg : ") ;
		#ifdef _WIN64
			codePrint ("db \"Runtime Error --> Negative bound in dynamic array declaration. Halt!\" , 10, 0\n") ;	
		#endif
		#if defined __linux__ || defined __MACH__
			codePrint ("db \"\033[1m\033[31mRuntime Error \033[0m\033[1m--> \033[0mNegative bound in dynamic array declaration. Halt!\" , 10, 0\n") ;	
		#endif			
	}

	if (isFlagSet (df, arrArgMismatch))
	{
		codePrint ("\t\t@arrArgMismatch: ") ;
		#ifdef _WIN64
			codePrint ("db \"Runtime Error --> Mismatch of limits in formal and actual array argument. Halt!\" , 10, 0\n") ;
		#endif
		#if defined __linux__ || defined __MACH__
			codePrint ("db \"\033[1m\033[31mRuntime Error \033[0m\033[1m--> \033[0mMismatch of limits in formal and actual array argument. Halt!\" , 10, 0\n") ;
		#endif			
	}

	if (isFlagSet (df, asgnArgMismatch))
	{
		codePrint ("\t\t@asgnArgMismatch: ") ;
		#ifdef _WIN64
			codePrint ("db \"Runtime Error --> Mismatch of limits in array assignment. Halt!\" , 10, 0\n") ;
		#endif
		#if defined __linux__ || defined __MACH__
			codePrint ("db \"\033[1m\033[31mRuntime Error \033[0m\033[1m--> \033[0mMismatch of limits in array assignment. Halt!\" , 10, 0\n") ;
		#endif			
	}

	if (isFlagSet (df, divZero))
	{
		codePrint ("\t\t@divZero: ") ;
		#ifdef _WIN64
			codePrint ("db \"Runtime Error --> Division by zero detected. Halt!\" , 10, 0\n") ;
		#endif
		#if defined __linux__ || defined __MACH__
			codePrint ("db \"\033[1m\033[31mRuntime Error \033[0m\033[1m--> \033[0mDivision by zero detected. Halt!\" , 10, 0\n") ;
		#endif			
	}

	if (isFlagSet (df, printFormatArray))
	{
		codePrint ("\t\t@printFormatArray : ") ;
		codePrint ("db \"Output : \" , 0\n") ;
	}

	if (isFlagSet (df, printInt))
	{
		codePrint ("\t\t@printInt : ") ;
		codePrint ("db \"%%d \", 0\n") ;
	}

	if (isFlagSet (df, printNewLine))
	{
		codePrint ("\t\t@printNewLine : ") ;
		codePrint ("db 10, 0\n") ;
	}

	if (isFlagSet (df, printFormat))
	{
		codePrint ("\t\t@printFormat : ") ;
		codePrint ("db \"Output :  %%d\" , 10, 0\n") ;
	}

	if (isFlagSet (df, printTrue))
	{
		codePrint ("\t\t@printTrue : ") ;
		codePrint ("db \"Output : true\" , 10, 0\n") ;
	}

	if (isFlagSet (df, printFalse))
	{
		codePrint ("\t\t@printFalse : ") ;
		codePrint ("db \"Output : false\" , 10, 0\n") ;
	}

	if (isFlagSet (df, DATA_true))
	{
		codePrint ("\t\t@true : ") ;
		codePrint ("db \"true \" , 0\n") ;
	}

	if (isFlagSet (df, DATA_false))
	{
		codePrint ("\t\t@false : ") ;
		codePrint ("db \"false \" , 0\n") ;
	}

	if (isFlagSet (df, inputIntPrompt))
	{
		codePrint ("\t\t@inputIntPrompt : ") ;
		codePrint ("db \"Enter an integer : \" , 0\n") ;
	}

	if (isFlagSet (df, inputBoolPrompt))
	{
		codePrint ("\t\t@inputBoolPrompt : ") ;
		codePrint ("db \"Enter a boolean (1 for true, 0 for false) : \" , 0\n") ;
	}

	if (isFlagSet (df, inputIntArrPrompt))
	{
		codePrint ("\t\t@inputIntArrPrompt : ") ;
		codePrint ("db \"Enter %%d array elements of integer type for range \", 0\n") ;
	}

	if (isFlagSet (df, inputBoolArrPrompt))
	{
		codePrint ("\t\t@inputBoolArrPrompt : ") ;
		codePrint ("db \"Enter %%d array elements of boolean type for range \", 0\n") ;
	}

	if (isFlagSet (df, leftRange))
	{
		codePrint ("\t\t@leftRange : ") ;
		codePrint ("db \"%%d to \" , 0\n") ;
	}

	if (isFlagSet (df, rightRange))
	{
		codePrint ("\t\t@rightRange : ") ;
		codePrint ("db \"%%d\" ,10, 0\n") ;
	}

	if (isFlagSet (df, inputInt))
	{
		codePrint ("\t\t@inputInt : ") ;
		codePrint ("db \"%%d\", 0\n") ;
	}
}

void printCommentLineNASM ()
{
	codePrint ("\n;--------------------------------------------------------------------------------------------------\n") ;
}
